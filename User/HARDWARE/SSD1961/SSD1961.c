
#include  "main.h"
#include "FONT.h"

_lcd_dev lcddev;

/********************************************************************
*	功能	：	LCD的画笔颜色和背景色
******************************************************************************/
u16 BACK_COLOR=WHITE;  //所设定显示字符区域背景色 	0XFFFF
u16 POINT_COLOR=BLACK;	//画笔颜色[打点颜色] 		0X0000

/********************************************************************
*	功能	：	写寄存器函数
*	参数	：	regval--将写入的寄存器序号
******************************************************************************/	 
void LCD_WR_REG(u16 regval)
{   
	LCD->LCD_REG=regval;
}
/********************************************************************
*	功能	：	写LCD数据
*	参数	：	data--写入寄存器的值
******************************************************************************/
void LCD_WR_DATA(u16 data)
{	 
	LCD->LCD_RAM=data;		 
}
/********************************************************************
*	功能	：	读LCD数据
******************************************************************************/
u16 LCD_RD_DATA(void)
{
	vu16 ram;			//防止被优化
	ram=LCD->LCD_RAM;
	return ram;	 
}
/********************************************************************
*	功能	：	写寄存器
*	参数	：	LCD_Reg--寄存器序号
				LCD_RegValue--要写入数据
******************************************************************************/
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{	
	LCD->LCD_REG = LCD_Reg;
	LCD->LCD_RAM = LCD_RegValue;
}
/********************************************************************
*	功能	：	读寄存器
*	参数	：	LCD_Reg--读取寄存器序号
******************************************************************************/
u16 LCD_ReadReg(u16 LCD_Reg)
{										   
	LCD_WR_REG(LCD_Reg);
	delay_us(5);		  
	return LCD_RD_DATA();
}
/********************************************************************
*	功能	：	开始写GRAM
******************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=lcddev.wramcmd;	  
}
/********************************************************************
*	功能	：	LCD写GRAM
*	参数	：	RGB_Code--颜色值
******************************************************************************/
void LCD_WriteRAM(u16 RGB_Code)
{							    
	LCD->LCD_RAM = RGB_Code;//写十六位GRAM
}
/********************************************************************
*	功能	：	读出的数据为GBR格式，而写入的时候为RGB格式，通过该函数转换
*	参数	：	c--GBR格式颜色值
******************************************************************************/
u16 LCD_BGR2RGB(u16 c)
{
	u16  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
}
/********************************************************************
*	功能	：	当mdk -O1时间优化时需要设置，延时i
******************************************************************************/
void opt_delay(u8 i)
{
	while(i--);
}
/********************************************************************
*	功能	：	读取个某点的颜色值
*	参数	：	x, y--坐标
******************************************************************************/
u16 LCD_ReadPoint(u16 x,u16 y)
{
 	u16 r=0,g=0,b=0;
	WDG_Feed();
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回		   
	LCD_SetCursor(x,y);	    
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X1963)LCD_WR_REG(0X2E);//9341/6804/3510/1963 发送读GRAM指令
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2E00);	//5510 发送读GRAM指令
	else LCD_WR_REG(0X22);      		 			//其他IC发送读GRAM指令
	if(lcddev.id==0X9320)opt_delay(2);				//FOR 9320,延时2us	    
 	else{}
	r=LCD_RD_DATA();								//dummy Read	   
	if(lcddev.id==0X1963)return r;					//1963直接读就可以 
	else{}
	opt_delay(2);	  
 	r=LCD_RD_DATA();  		  						//实际坐标颜色
 	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)		//9341/NT35310/NT35510要分2次读出
 	{
		opt_delay(2);	  
		b=LCD_RD_DATA(); 
		g=r&0XFF;		//对于9341/5310/5510,第一次读取的是RG的值,R在前,G在后,各占8位
		g<<=8;
	}
	else{}	
	if(lcddev.id==0X9325||lcddev.id==0X4535||lcddev.id==0X4531||lcddev.id==0XB505||lcddev.id==0XC505)return r;	//这几种IC直接返回颜色值
	else if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));//ILI9341/NT35310/NT35510需要公式转换一下
	else return LCD_BGR2RGB(r);						//其他IC
}
/********************************************************************
*	功能	：	LCD开启显示
******************************************************************************/

void LCD_DisplayOn(void)
{	
	WDG_Feed();
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X1963)LCD_WR_REG(0X29);	//开启显示
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2900);	//开启显示
	else LCD_WriteReg(0X07,0x0173); 			 	//开启显示
}
/********************************************************************
*	功能	：	LCD关闭显示
******************************************************************************/
void LCD_DisplayOff(void)
{	   
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X1963)LCD_WR_REG(0X28);	//关闭显示
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2800);	//关闭显示
	else LCD_WriteReg(0X07,0x0);//关闭显示 
}
/********************************************************************
*	功能	：	设置光标位置
*	参数	：	Xpos, Ypos--坐标
******************************************************************************/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 
 	if(lcddev.id==0X1963)
	{  	
		WDG_Feed();
		if(lcddev.dir==0)//x坐标需要变换
		{
			Xpos=lcddev.width-1-Xpos;
			LCD_WR_REG(lcddev.setxcmd); 
//			LCD_WR_DATA(0);LCD_WR_DATA(0); 		
			LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF);
			LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF);
		}else
		{
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF); 
//			LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF);
			LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);		 	 			
		}	
		LCD_WR_REG(lcddev.setycmd);
		LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF);
//		LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF); 
		LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF);
	}
	else{}
}
/********************************************************************
*	功能	：	设置LCD的自动扫描方向
*	注意	：	其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
				所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
				9320/9325/9328/4531/4535/1505/b505/5408/9341/5310/5510/1963等IC已经实际测试	
*	参数	：	dir:0~7,代表8个方向(具体定义见lcd.h)
******************************************************************************/
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;
	WDG_Feed();	
	if((lcddev.dir==1&&lcddev.id!=0X6804&&lcddev.id!=0X1963)||(lcddev.dir==0&&lcddev.id==0X1963))//横屏时，对6804和1963不改变扫描方向！竖屏时1963改变方向
	{			   
		switch(dir)//方向转换
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;	     
		}
	}
	else{}	
	if(lcddev.id==0x9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X5510||lcddev.id==0X1963)//9341/6804/5310/5510/1963,特殊处理
	{
		switch(dir)
		{
			case L2R_U2D://从左到右,从上到下
				regval|=(0<<7)|(0<<6)|(0<<5); 
				break;
			case L2R_D2U://从左到右,从下到上
				regval|=(1<<7)|(0<<6)|(0<<5); 
				break;
			case R2L_U2D://从右到左,从上到下
				regval|=(0<<7)|(1<<6)|(0<<5); 
				break;
			case R2L_D2U://从右到左,从下到上
				regval|=(1<<7)|(1<<6)|(0<<5); 
				break;	 
			case U2D_L2R://从上到下,从左到右
				regval|=(0<<7)|(0<<6)|(1<<5); 
				break;
			case U2D_R2L://从上到下,从右到左
				regval|=(0<<7)|(1<<6)|(1<<5); 
				break;
			case D2U_L2R://从下到上,从左到右
				regval|=(1<<7)|(0<<6)|(1<<5); 
				break;
			case D2U_R2L://从下到上,从右到左
				regval|=(1<<7)|(1<<6)|(1<<5); 
				break;	 
		}
		if(lcddev.id==0X5510)dirreg=0X3600;
		else dirreg=0X36;
 		if((lcddev.id!=0X5310)&&(lcddev.id!=0X5510)&&(lcddev.id!=0X1963))regval|=0X08;//5310/5510/1963不需要BGR   
		else{}
		if(lcddev.id==0X6804)regval|=0x02;//6804的BIT6和9341的反了	
		else{}			
		LCD_WriteReg(dirreg,regval);
		if(lcddev.id!=0X1963)//1963不做坐标处理
		{
			if(regval&0X20)
			{
				if(lcddev.width<lcddev.height)//交换X,Y
				{
					temp=lcddev.width;
					lcddev.width=lcddev.height;
					lcddev.height=temp;
				}
				else{}
			}else  
			{
				if(lcddev.width>lcddev.height)//交换X,Y
				{
					temp=lcddev.width;
					lcddev.width=lcddev.height;
					lcddev.height=temp;
				}
				else{}
			}  
		}
		else{}
		if(lcddev.id==0X5510)
		{
			LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA((lcddev.width-1)>>8); 
			LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA((lcddev.width-1)&0XFF); 
			LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA((lcddev.height-1)>>8); 
			LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA((lcddev.height-1)&0XFF);
		}else
		{
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);
			LCD_WR_REG(lcddev.setycmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF);  
		}
  	}else 
	{
		switch(dir)
		{
			case L2R_U2D://从左到右,从上到下
				regval|=(1<<5)|(1<<4)|(0<<3); 
				break;
			case L2R_D2U://从左到右,从下到上
				regval|=(0<<5)|(1<<4)|(0<<3); 
				break;
			case R2L_U2D://从右到左,从上到下
				regval|=(1<<5)|(0<<4)|(0<<3);
				break;
			case R2L_D2U://从右到左,从下到上
				regval|=(0<<5)|(0<<4)|(0<<3); 
				break;	 
			case U2D_L2R://从上到下,从左到右
				regval|=(1<<5)|(1<<4)|(1<<3); 
				break;
			case U2D_R2L://从上到下,从右到左
				regval|=(1<<5)|(0<<4)|(1<<3); 
				break;
			case D2U_L2R://从下到上,从左到右
				regval|=(0<<5)|(1<<4)|(1<<3); 
				break;
			case D2U_R2L://从下到上,从右到左
				regval|=(0<<5)|(0<<4)|(1<<3); 
				break;	 
		} 
		dirreg=0X03;
		regval|=1<<12; 
		LCD_WriteReg(dirreg,regval);
	}
}
/********************************************************************
*	功能	：	画点
*	参数	：	x,y--坐标
******************************************************************************/
void LCD_DrawPoint(u16 x,u16 y)
{
	WDG_Feed();
	LCD_SetCursor(x,y);		//设置光标位置 
	LCD_WriteRAM_Prepare();	//开始写入GRAM
//	LCD->LCD_RAM=POINT_COLOR; 
	LCD->LCD_RAM=COLOR_WHITE_G;
}
/********************************************************************
*	功能	：	快速画点
*	参数	：	x,y--坐标
				color--颜色
******************************************************************************/
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{	
	WDG_Feed();
	if(lcddev.id==0X1963)
	{
		if(lcddev.dir==0)x=lcddev.width-1-x;
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF);
		LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF);
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF);
		LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF);
	}
	else{}
	LCD->LCD_REG=lcddev.wramcmd;
	LCD->LCD_RAM=color;
}
/********************************************************************
*	功能	：	SSD1963 背光设置
*	参数	：	pwm--背光等级,0~100.越大越亮
******************************************************************************/
void LCD_SSD_BackLightSet(u8 pwm)
{
	WDG_Feed();
	LCD_WR_REG(0xBE);	//配置PWM输出
	LCD_WR_DATA(0x05);	//1设置PWM频率
	LCD_WR_DATA(pwm*2.55);//2设置PWM占空比
	LCD_WR_DATA(0x01);	//3设置C
	LCD_WR_DATA(0xFF);	//4设置D
	LCD_WR_DATA(0x00);	//5设置E
	LCD_WR_DATA(0x00);	//6设置F
}
/********************************************************************
*	功能	：	设置LCD显示方向
*	参数	：	dir--0,竖屏；1,横屏
******************************************************************************/
void LCD_Display_Dir(u8 dir)
{
	WDG_Feed();
	if(dir==0)			//竖屏
	{
		lcddev.dir=0;	//竖屏
		lcddev.width=240;
		lcddev.height=320;
		if(lcddev.id==0X1963)
		{
			lcddev.wramcmd=0X2C;	//设置写入GRAM的指令 
			lcddev.setxcmd=0X2B;	//设置写X坐标指令
			lcddev.setycmd=0X2A;	//设置写Y坐标指令
			lcddev.width=480;		//设置宽度480
			lcddev.height=800;		//设置高度800  
		}
		else{}
	}else 				//横屏
	{	  				
		lcddev.dir=1;	//横屏
		lcddev.width=320;
		lcddev.height=240;
		if(lcddev.id==0X1963)
		{
			lcddev.wramcmd=0X2C;	//设置写入GRAM的指令 
			lcddev.setxcmd=0X2A;	//设置写X坐标指令
			lcddev.setycmd=0X2B;	//设置写Y坐标指令
			lcddev.width=480;		//设置宽度800->480[V64]
			lcddev.height=272;		//设置高度480->272[V64]
		}
		else{}
	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}
/********************************************************************
*	功能	：	设置窗口,并自动设置画点坐标到窗口左上角(sx,sy)
*	参数	：	sx,sy--窗口起始坐标(左上角)
				width,height--窗口宽度和高度,必须大于0!!
				窗体大小:width*height
******************************************************************************/
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{    
	u8 hsareg,heareg,vsareg,veareg;
	u16 hsaval,heaval,vsaval,veaval; 
	u16 twidth,theight;
	
	WDG_Feed();
	twidth=sx+width-1;
	theight=sy+height-1;
	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X6804||(lcddev.dir==1&&lcddev.id==0X1963))
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(sx>>8); 
		LCD_WR_DATA(sx&0XFF);	 
		LCD_WR_DATA(twidth>>8); 
		LCD_WR_DATA(twidth&0XFF);  
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(sy>>8); 
		LCD_WR_DATA(sy&0XFF); 
		LCD_WR_DATA(theight>>8); 
		LCD_WR_DATA(theight&0XFF); 
	}else if(lcddev.id==0X1963)//1963竖屏特殊处理
	{
		sx=lcddev.width-width-sx; 
		height=sy+height-1; 
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(sx>>8); 
		LCD_WR_DATA(sx&0XFF);	 
		LCD_WR_DATA((sx+width-1)>>8); 
		LCD_WR_DATA((sx+width-1)&0XFF);  
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(sy>>8); 
		LCD_WR_DATA(sy&0XFF); 
		LCD_WR_DATA(height>>8); 
		LCD_WR_DATA(height&0XFF); 		
	}else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(sx>>8);  
		LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(sx&0XFF);	  
		LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA(twidth>>8);   
		LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA(twidth&0XFF);   
		LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(sy>>8);   
		LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(sy&0XFF);  
		LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA(theight>>8);   
		LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA(theight&0XFF);  
	}else	//其他驱动IC
	{
		if(lcddev.dir==1)//横屏
		{
			//窗口值
			hsaval=sy;				
			heaval=theight;
			vsaval=lcddev.width-twidth-1;
			veaval=lcddev.width-sx-1;				
		}else
		{ 
			hsaval=sx;				
			heaval=twidth;
			vsaval=sy;
			veaval=theight;
		} 
		hsareg=0X50;heareg=0X51;//水平方向窗口寄存器
		vsareg=0X52;veareg=0X53;//垂直方向窗口寄存器	   							  
		//设置寄存器值
		LCD_WriteReg(hsareg,hsaval);
		LCD_WriteReg(heareg,heaval);
		LCD_WriteReg(vsareg,vsaval);
		LCD_WriteReg(veareg,veaval);		
		LCD_SetCursor(sx,sy);	//设置光标位置
	}
}
/********************************************************************
*	功能	：	清屏函数
*	参数	：	color--要清屏的填充色
******************************************************************************/
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	
	WDG_Feed();
	
	totalpoint*=lcddev.height; 			//得到总点数
	if((lcddev.id==0X6804)&&(lcddev.dir==1))//6804横屏的时候特殊处理
	{						    
 		lcddev.dir=0;	 
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 			
		LCD_SetCursor(0x00,0x0000);		//设置光标位置
 		lcddev.dir=1;	 
  		lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;  	 
 	}else LCD_SetCursor(0x00,0x0000);	//设置光标位置
	LCD_WriteRAM_Prepare();     		//开始写入GRAM
	for(index=0;index<totalpoint;index++)
	{
		LCD->LCD_RAM=color;	
	}
}
/********************************************************************
*	功能	：	在指定区域内填充单个颜色
*	参数	：	(sx,sy),(ex,ey)--填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1) 
				color--要填充的颜色
******************************************************************************/
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	
	WDG_Feed();
	
	xlen=ex-sx+1;
	for(i=sy;i<=ey;i++)
	{
		LCD_SetCursor(sx,i);      				//设置光标位置 
		LCD_WriteRAM_Prepare();     			//开始写入GRAM	  
		for(j=0;j<xlen;j++)LCD->LCD_RAM=color;	//显示颜色
		WDG_Feed();
	}
}
/********************************************************************
*	功能	：	在指定区域内填充指定颜色块
*	参数	：	(sx,sy),(ex,ey)--填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1) 
				color--要填充的颜色
******************************************************************************/
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM
		for(j=0;j<width;j++)LCD->LCD_RAM=color[i*width+j];//写入数据 
		WDG_Feed();
	}		  
}
/********************************************************************
*	功能	：	画线
*	参数	：	x1,y1--起点坐标
				x2,y2--终点坐标
******************************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	
	WDG_Feed();
	
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}
/********************************************************************
*	功能	：	画矩形
*	参数	：	(x1,y1),(x2,y2)--矩形的对角坐标
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	WDG_Feed();
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
/********************************************************************
*	功能	：	在指定位置画一个指定大小的圆
*	参数	：	(x,y)--中心点
				r--半径
******************************************************************************/
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;

	WDG_Feed();
	
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-a,y0+b);             //1       
 		LCD_DrawPoint(x0-b,y0+a);             
		LCD_DrawPoint(x0-a,y0-b);             //2             
  		LCD_DrawPoint(x0-b,y0-a);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
}
/********************************************************************
*	功能	：	在指定位置显示一个字符
*	参数	：	x,y--起始坐标
				num--要显示的字符:" "--->"~"
				size--字体大小 12/16/24
				mode--叠加方式(1)还是非叠加方式(0)
*	注意	：	非叠加方式会进行显示区域背景色处理，屏幕整个水平方向有带状影响
******************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
	
	WDG_Feed();
	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			else{}
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//超区域了
			else{}
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				else{}
				break;
			}
			else{}
		}  	 
	}  	    	   	 	  
}
/********************************************************************
*	功能	：	m^n函数
******************************************************************************/
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	
	WDG_Feed();
	while(n--)result*=m;    
	return result;
}
/********************************************************************
*	功能	：	显示数字,高位为0,则不显示
*	参数	：	x,y --起点坐标
				len --数字的位数
				size--字体大小
				color--颜色
				num--数值(0~4294967295);				
******************************************************************************/
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;	

	WDG_Feed();
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
		else{}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
}
/********************************************************************
*	功能	：	显示数字,高位为0,还是显示
*	参数	：	x,y--起点坐标
				num--数值(0~999999999);	 
				len--长度(即要显示的位数)
				size--字体大小
				mode--	[7]:0,不填充;1,填充0.	[6:1]:保留	[0]:0,非叠加显示;1,叠加显示.
******************************************************************************/
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;
	
	WDG_Feed();
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
		else{}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
}
/********************************************************************
*	功能	：	显示字符串
*	参数	：	x,y--起点坐标
				width,height--区域大小
				size--字体大小
				*p--字符串起始地址	
******************************************************************************/
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	
	WDG_Feed();
	
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {
        if(x>=width){x=x0;y+=size;}
		else{}
        if(y>=height)break;//退出
		else{}
        LCD_ShowChar(x,y,*p,size,1);
        x+=size/2;
        p++;
    }
}

/********************************************************************
*	功能	：	在指定位置显示一个字符
*	参数	：	x,y--起始坐标
				num--要显示的字符:" "--->"~"
				size--字体大小 12/16/24
				mode--叠加方式(1)还是非叠加方式(0)
*	注意	：	非叠加方式会进行显示区域背景色处理，屏幕整个水平方向有带状影响
******************************************************************************/
static void LCD_ShowChar_WithColor(u16 x,u16 y,u8 num,u8 size,u8 mode, u16 backcolor, u16 pointcolor)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	
	WDG_Feed();
	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,pointcolor);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,backcolor);
			else{}
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//超区域了
			else{}
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				else{}
				break;
			}
			else{}
		}  	 
	}  	    	   	 	  
}

/********************************************************************
*	功能	：	显示字符串
*	参数	：	x,y--起点坐标
				width,height--区域大小
				size--字体大小
				*p--字符串起始地址	
******************************************************************************/
void LCD_ShowString_WithColor(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p, u16 backcolor, u16 pointcolor)
{         
	u8 x0=x;
	
	WDG_Feed();
	
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {
        if(x>=width){x=x0;y+=size;}
		else{}
        if(y>=height)break;//退出
		else{}
        LCD_ShowChar_WithColor(x, y, *p, size, 0, backcolor, pointcolor);
        x+=size/2;
        p++;
    }
}

/********************************************************************
*	功能	：	初始化lcd
*	注意	：	该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI9320的!!!
				在其他型号的驱动芯片上没有测试! 

PD4		LCD_RD			//LCD屏幕控制，	6800模式：使能信号；	8080模式：从LCD读取数据
PD5		LCD_WR			//读写时序控制，6800模式：xxxx；		8080模式：向LCD写数据
PD7		LCD_CS			//LCD屏片选信号
PD11	LCD_RS			//数据或命令选择（0：读写命令，1：读写数据）
PE5		LCD_RST			//LCD屏复位控制信号

PD14	LCD_D0
PD15	LCD_D1
PD0		LCD_D2
PD1		LCD_D3
PE7		LCD_D4
PE8		LCD_D5
PE9		LCD_D6
PE10	LCD_D7
PE11	LCD_D8
PE12	LCD_D9
PE13	LCD_D10
PE14	LCD_D11
PE15	LCD_D12
PD8		LCD_D13
PD9		LCD_D14
PD10	LCD_D15





******************************************************************************/
void LCD_Init(void)
{ 
	u8 id1=0,id2=0,id3=0,id4=0;
	GPIO_InitTypeDef  GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE);//使能PD,PE时钟  
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);//使能FSMC时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;//PD0,1,4,5,8,9,10,14,15 AF OUT
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;//PE7~15,AF OUT
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化

	//数据线
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);	//PD0,AF12
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);	//PD1,AF12
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC); 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC); 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);	//PD15,AF12
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);	//PE7,AF12
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);	//PE15,AF12
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource7,GPIO_AF_FSMC);	//CS#   FSMC_NE1
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF_FSMC);	//RS    FSMC_A16
	
	
	//方式1，
	readWriteTiming.FSMC_AddressSetupTime = 16;	 			//地址建立时间（ADDSET）为16个HCLK 1/168M=6ns*16=96ns	
	readWriteTiming.FSMC_AddressHoldTime = 0x00;	 		//地址保持时间（ADDHLD）模式A未用到	
	readWriteTiming.FSMC_DataSetupTime = 60;				//数据保存时间为60个HCLK	=6*60=360ns
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	readWriteTiming.FSMC_CLKDivision = 0x02;
	readWriteTiming.FSMC_DataLatency = 0x00;
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 
    
	writeTiming.FSMC_AddressSetupTime =18;	      			//地址建立时间（ADDSET）为9个HCLK =54ns 
	writeTiming.FSMC_AddressHoldTime = 0x00;	 			//地址保持时间（A		
	writeTiming.FSMC_DataSetupTime = 16;		 			//数据保存时间为6ns*9个HCLK=54ns
	writeTiming.FSMC_BusTurnAroundDuration = 0x00;
	writeTiming.FSMC_CLKDivision = 0x00;
	writeTiming.FSMC_DataLatency = 0x00;
	writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 	//模式A 


	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;//  这里我们使用NE1，也就对应BTCR[0]。
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // 不复用数据地址
	FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit   
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  存储器写使能
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // 读写使用不同的时序
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //读写时序
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //写时序

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置

	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  // 使能BANK1
	
	
	WDG_Feed();
	Delay_ms(800); 					// delay 50 ms能够显示，延时1000ms也能显示，杂点少，更稳定，但延时600ms花屏，建议延时800-1000ms 
	WDG_Feed();
  	lcddev.id=LCD_ReadReg(0x0000);	//读ID（9320/9325/9328/4531/4535等IC）
	log_info("lcddev.id=0x%x\r\n",lcddev.id);

	{
		uint8_t reg_val[7] = {0},i=0;
		LCD_WR_REG(0xa1);
		delay_us(5);
		reg_val[0] = LCD_RD_DATA();
		reg_val[1] = LCD_RD_DATA();
		reg_val[2] = LCD_RD_DATA();
		reg_val[3] = LCD_RD_DATA();
		reg_val[4] = LCD_RD_DATA();
		
		
		log_info("get_lcd_mode:");
		for(i=0;i<5;i++)
			log_info(" 0x%x",reg_val[i]);
		log_info("\r\n");
		
		
	}
	
	LCD_WR_REG(0XA1);
	lcddev.id = LCD_RD_DATA();
    lcddev.id = LCD_RD_DATA();  //读回0X57
    lcddev.id <<= 8;
    lcddev.id |= LCD_RD_DATA(); //读回0X61
	log_info("lcddev.id=0x%x\r\n",lcddev.id);
	
							
	Delay_ms(10);
	WDG_Feed();
	#if(LCD_ID_PLAN == ID_PLAN_A)		//PLAN-A
		lcddev.id = 0x1963;				//不读，直接将SSD1961驱动的屏幕标记为1963，直接赋值较读取ID测试阶段能更大概率实现驱动屏幕显示
	#elif(LCD_ID_PLAN == ID_PLAN_B)		//PLAN-B
		if(lcddev.id<=0XFF||lcddev.id==0XFFFF||lcddev.id==0X9300)//读到ID不正确,新增lcddev.id==0X9300判断，因为9341在未被复位的情况下会被读成9300	//运行到此读取lcddev.id=0xff，故在移植基础添加=0xff处理【V55】
		{	
			//尝试9341 ID的读取		
//			LCD_WR_REG(0XD3);
//			lcddev.id=LCD_RD_DATA();	//dummy read 	
//			lcddev.id=LCD_RD_DATA();	//读到0X00
//			lcddev.id=LCD_RD_DATA();   	//读取93								   
//			lcddev.id<<=8;
//			lcddev.id|=LCD_RD_DATA();  	//读取41 	   			   
//			if(lcddev.id!=0X9341)		//非9341,尝试是不是6804
//			{	
//				LCD_WR_REG(0XBF);				   
//				lcddev.id=LCD_RD_DATA(); 	//dummy read 	 
//				lcddev.id=LCD_RD_DATA();   	//读回0X01			   
//				lcddev.id=LCD_RD_DATA(); 	//读回0XD0 			  	
//				lcddev.id=LCD_RD_DATA();	//这里读回0X68 
//				lcddev.id<<=8;
//				lcddev.id|=LCD_RD_DATA();	//这里读回0X04	  
//				if(lcddev.id!=0X6804)		//也不是6804,尝试看看是不是NT35310
//				{ 
//					LCD_WR_REG(0XD4);				   
//					lcddev.id=LCD_RD_DATA();//dummy read  
//					lcddev.id=LCD_RD_DATA();//读回0X01	 
//					lcddev.id=LCD_RD_DATA();//读回0X53	
//					lcddev.id<<=8;	 
//					lcddev.id|=LCD_RD_DATA();	//这里读回0X10	 
//					if(lcddev.id!=0X5310)		//也不是NT35310,尝试看看是不是NT35510
//					{
//						LCD_WR_REG(0XDA00);	
//						lcddev.id=LCD_RD_DATA();		//读回0X00	 
//						LCD_WR_REG(0XDB00);	
//						lcddev.id=LCD_RD_DATA();		//读回0X80
//						lcddev.id<<=8;	
//						LCD_WR_REG(0XDC00);	
//						lcddev.id|=LCD_RD_DATA();		//读回0X00		
//						if(lcddev.id==0x8000)lcddev.id=0x5510;//NT35510读回的ID是8000H,为方便区分,我们强制设置为5510
//						if(lcddev.id!=0X5510)			//也不是NT5510,尝试看看是不是SSD1963
//						{
							LCD_WR_REG(0XA1);
							lcddev.id=LCD_RD_DATA();
							lcddev.id=LCD_RD_DATA();	//读回0X57
							lcddev.id<<=8;	 
							lcddev.id|=LCD_RD_DATA();	//读回0X61	
							if(lcddev.id==0X5761)lcddev.id=0X1963;//SSD1963读回的ID是5761H,为方便区分,我们强制设置为1963
//						}
//					}
//				}
//			}  	
		}
//		printf(" LCD ID:%x\r\n",lcddev.id); //打印LCD ID  
	#endif

	if(lcddev.id==0X1963)
	{	
		WDG_Feed();
		
		LCD_WR_REG(0xE2);		//Set PLL with OSC = 10MHz (hardware),	Multiplier N = 35, 250MHz < VCO < 800MHz = OSC*(N+1), VCO = 300MHz
		LCD_WR_DATA(0x1D);		//参数1 
		LCD_WR_DATA(0x02);		//参数2 Divider M = 2, PLL = 300/(M+1) = 100MHz
		LCD_WR_DATA(0x04);		//参数3 Validate M and N values  		
		delay_us(100);		
		LCD_WR_REG(0xE0);		// Start PLL command
		LCD_WR_DATA(0x01);		// enable PLL		
		Delay_ms(10);
		LCD_WR_REG(0xE0);		// Start PLL command again
		LCD_WR_DATA(0x03);		// now, use PLL output as system clock	
		Delay_ms(12);  
		LCD_WR_REG(0x01);		//软复位
		Delay_ms(10);
		Delay_ms(800);			// longzhigu:  [V194]
		
		LCD_WR_REG(0xE6);		//设置像素频率,33Mhz
		LCD_WR_DATA(0x2F);		//web--0x04
		LCD_WR_DATA(0xFF);		//web--0xcc
		LCD_WR_DATA(0xFF);		//web--0xc8
		
		LCD_WR_REG(0xB0);		//设置LCD模式
		LCD_WR_DATA(0x10);		//0X20--24位模式		//web--0x10 18bit[V55]
		LCD_WR_DATA(0x00);		//TFT 模式 
	
		WDG_Feed();
		
		LCD_WR_DATA((SSD_HOR_RESOLUTION-1)>>8);//设置LCD水平像素
		LCD_WR_DATA(SSD_HOR_RESOLUTION-1);		 
		LCD_WR_DATA((SSD_VER_RESOLUTION-1)>>8);//设置LCD垂直像素
		LCD_WR_DATA(SSD_VER_RESOLUTION-1);		 
		LCD_WR_DATA(0x00);		//RGB序列 
		
		LCD_WR_REG(0xB4);		//Set horizontal period
		LCD_WR_DATA((SSD_HT-1)>>8);
		LCD_WR_DATA(SSD_HT-1);
		LCD_WR_DATA(SSD_HPS>>8);
		LCD_WR_DATA(SSD_HPS);
		LCD_WR_DATA(SSD_HOR_PULSE_WIDTH-1);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		
		
		WDG_Feed();
		
		LCD_WR_REG(0xB6);		//Set vertical period
		LCD_WR_DATA((SSD_VT-1)>>8);
		LCD_WR_DATA(SSD_VT-1);
		LCD_WR_DATA(SSD_VPS>>8);
		LCD_WR_DATA(SSD_VPS);
		LCD_WR_DATA(SSD_VER_FRONT_PORCH-1);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		
		LCD_WR_REG(0xF0);	//设置SSD1963与CPU接口为16bit
		LCD_WR_DATA(0x03);	//16-bit(565 format) data for 16bpp

		LCD_WR_REG(0x29);	//开启显示
		//设置PWM输出  背光通过占空比可调 
		LCD_WR_REG(0xD0);	//设置自动白平衡DBC
		LCD_WR_DATA(0x00);	//disable
	
		LCD_WR_REG(0xBE);	//配置PWM输出
		LCD_WR_DATA(0x05);	//1设置PWM频率
		LCD_WR_DATA(0xFE);	//2设置PWM占空比
		LCD_WR_DATA(0x01);	//3设置C
		LCD_WR_DATA(0x00);	//4设置D
		LCD_WR_DATA(0x00);	//5设置E 
		LCD_WR_DATA(0x00);	//6设置F 
		
		WDG_Feed();
		
		LCD_WR_REG(0xB8);	//设置GPIO配置
		LCD_WR_DATA(0x03);	//2个IO口设置成输出			//web--0x0f
		LCD_WR_DATA(0x01);	//GPIO使用正常的IO功能
		LCD_WR_REG(0xBA);
		LCD_WR_DATA(0X01);	//GPIO[1:0]=01,控制LCD方向
		
		LCD_SSD_BackLightSet(0);//背光设置为最亮100
		Delay_ms(200);
		LCD_SSD_BackLightSet(0);//背光设置为最亮100
	}	
	LCD_Display_Dir(1);		//默认为竖屏0
	LCD_Clear(COLOR_DESKTOP);
}
/********************************************************************
*	功能	：	LCD复位引脚初始化
******************************************************************************/
void LCDRST_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//使能GPIOA时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化
	
	WDG_Feed();
	LCD_RST_L();
	Delay_ms(200);
	LCD_RST_H();
	
	WDG_Feed();
	Delay_ms(600);
	WDG_Feed();
	LCD_RST_L();
	Delay_ms(200);
	LCD_RST_H();
	WDG_Feed();
	
//	delay_us(4);
//	pLCDRST = 1;
	
}

