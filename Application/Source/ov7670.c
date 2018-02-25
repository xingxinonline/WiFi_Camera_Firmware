#include "stm32f4xx_hal.h" 
#include "ov7670.h"
#include "ov7670config.h"	  
#include "delay.h"			 
#include "sccb.h"	


#define CONFIG_DELAY        10
#define OV2640_DELAY_US     10
//////////////////////////////////////////////////////////////////////////////////			    			    
//初始化OV7670
//返回0:成功
//返回其他值:错误代码
uint8_t OV7670_Init(void)
{
    uint16_t i=0;
    uint16_t reg=0;
    
    OV7670_DOWN_LOW;	//POWER ON
    delay_ms(10);
    OV7670_RESET_LOW;	//复位OV7670
    delay_ms(200);
    OV7670_RESET_HIGH;	//结束复位 
    SCCB_Init();        		//初始化SCCB 的IO口	
    SCCB_WR_Reg(0X12, 0x80);	//软复位OV7670
    SCCB_WR_Reg(0X12, 0x80);	//软复位OV7670
    delay_ms(50); 
    reg=SCCB_RD_Reg(0X1c);	//读取厂家ID 高八位
    reg<<=8;
    reg|=SCCB_RD_Reg(0X1d);	//读取厂家ID 低八位
    if(reg!=OV7670_MID)
    {
        printf("MID:%d\r\n",reg);
        return 1;
    }
    reg=SCCB_RD_Reg(0X0a);	//读取厂家ID 高八位
    reg<<=8;
    reg|=SCCB_RD_Reg(0X0b);	//读取厂家ID 低八位
    if(reg!=OV7670_PID)
    {
        printf("HID:%d\r\n",reg);
        return 2;
    }   
    //初始化 OV7670,采用QVGA分辨率(320*240)  
    for(i=0;i<sizeof(OV2640_QVGA)/sizeof(OV2640_QVGA[0]);i++)
    {
        SCCB_WR_Reg(OV2640_QVGA[i][0],OV2640_QVGA[i][1]);
        delay_us(50);
    } 
    return 0x00; 	//ok
} 
////////////////////////////////////////////////////////////////////////////
//OV7670功能设置
//白平衡设置
//0:自动
//1:太阳sunny
//2,阴天cloudy
//3,办公室office
//4,家里home
void OV7670_Light_Mode(uint8_t mode)
{
	uint8_t reg13val=0XE7;//默认就是设置为自动白平衡
	uint8_t reg01val=0;
	uint8_t reg02val=0;
	switch(mode)
	{
    case 1://sunny
        reg13val=0XE5;
        reg01val=0X5A;
        reg02val=0X5C;
        break;	
    case 2://cloudy
        reg13val=0XE5;
        reg01val=0X58;
        reg02val=0X60;
        break;	
    case 3://office
        reg13val=0XE5;
        reg01val=0X84;
        reg02val=0X4c;
        break;	
    case 4://home
        reg13val=0XE5;
        reg01val=0X96;
        reg02val=0X40;
        break;	
	}
	SCCB_WR_Reg(0X13,reg13val);//COM8设置 
	SCCB_WR_Reg(0X01,reg01val);//AWB蓝色通道增益 
	SCCB_WR_Reg(0X02,reg02val);//AWB红色通道增益 
}				  
//色度设置
//0:-2
//1:-1
//2,0
//3,1
//4,2
void OV7670_Color_Saturation(uint8_t sat)
{
	uint8_t reg4f5054val=0X80;//默认就是sat=2,即不调节色度的设置
 	uint8_t reg52val=0X22;
	uint8_t reg53val=0X5E;
 	switch(sat)
	{
    case 0://-2
        reg4f5054val=0X40;  	 
        reg52val=0X11;
        reg53val=0X2F;	 	 
        break;	
    case 1://-1
        reg4f5054val=0X66;	    
        reg52val=0X1B;
        reg53val=0X4B;	  
        break;	
    case 3://1
        reg4f5054val=0X99;	   
        reg52val=0X28;
        reg53val=0X71;	   
        break;	
    case 4://2
        reg4f5054val=0XC0;	   
        reg52val=0X33;
        reg53val=0X8D;	   
        break;	
	}
	SCCB_WR_Reg(0X4F,reg4f5054val);	//色彩矩阵系数1
	SCCB_WR_Reg(0X50,reg4f5054val);	//色彩矩阵系数2 
	SCCB_WR_Reg(0X51,0X00);			//色彩矩阵系数3  
	SCCB_WR_Reg(0X52,reg52val);		//色彩矩阵系数4 
	SCCB_WR_Reg(0X53,reg53val);		//色彩矩阵系数5 
	SCCB_WR_Reg(0X54,reg4f5054val);	//色彩矩阵系数6  
	SCCB_WR_Reg(0X58,0X9E);			//MTXS 
}
//亮度设置
//0:-2
//1:-1
//2,0
//3,1
//4,2
void OV7670_Brightness(uint8_t bright)
{
	uint8_t reg55val=0X00;//默认就是bright=2
  	switch(bright)
	{
    case 0://-2
        reg55val=0XB0;	 	 
        break;	
    case 1://-1
        reg55val=0X98;	 	 
        break;	
    case 3://1
        reg55val=0X18;	 	 
        break;	
    case 4://2
        reg55val=0X30;	 	 
        break;	
	}
	SCCB_WR_Reg(0X55,reg55val);	//亮度调节 
}
//对比度设置
//0:-2
//1:-1
//2,0
//3,1
//4,2
void OV7670_Contrast(uint8_t contrast)
{
	uint8_t reg56val=0X40;//默认就是contrast=2
  	switch(contrast)
	{
    case 0://-2
        reg56val=0X30;	 	 
        break;	
    case 1://-1
        reg56val=0X38;	 	 
        break;	
    case 3://1
        reg56val=0X50;	 	 
        break;	
    case 4://2
        reg56val=0X60;	 	 
        break;	
	}
	SCCB_WR_Reg(0X56,reg56val);	//对比度调节 
}
//特效设置
//0:普通模式    
//1,负片
//2,黑白   
//3,偏红色
//4,偏绿色
//5,偏蓝色
//6,复古	    
void OV7670_Special_Effects(uint8_t eft)
{
	uint8_t reg3aval=0X04;//默认为普通模式
	uint8_t reg67val=0XC0;
	uint8_t reg68val=0X80;
	switch(eft)
	{
    case 1://负片
        reg3aval=0X24;
        reg67val=0X80;
        reg68val=0X80;
        break;	
    case 2://黑白
        reg3aval=0X14;
        reg67val=0X80;
        reg68val=0X80;
        break;	
    case 3://偏红色
        reg3aval=0X14;
        reg67val=0Xc0;
        reg68val=0X80;
        break;	
    case 4://偏绿色
        reg3aval=0X14;
        reg67val=0X40;
        reg68val=0X40;
        break;	
    case 5://偏蓝色
        reg3aval=0X14;
        reg67val=0X80;
        reg68val=0XC0;
        break;	
    case 6://复古
        reg3aval=0X14;
        reg67val=0XA0;
        reg68val=0X40;
        break;	 
	}
	SCCB_WR_Reg(0X3A,reg3aval);//TSLB设置 
	SCCB_WR_Reg(0X68,reg67val);//MANU,手动U值 
	SCCB_WR_Reg(0X67,reg68val);//MANV,手动V值 
}	
//设置图像输出窗口
//对QVGA设置。
void OV7670_Window_Set(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
	uint16_t endx;
	uint16_t endy;
	uint8_t temp; 
	endx=sx+width*2;	//V
 	endy=sy+height*2;
	if(endy>784)endy-=784;
	temp=SCCB_RD_Reg(0X03);				//读取Vref之前的值
	temp&=0XF0;
	temp|=((endx&0X03)<<2)|(sx&0X03);
	SCCB_WR_Reg(0X03,temp);				//设置Vref的start和end的最低2位
	SCCB_WR_Reg(0X19,sx>>2);			//设置Vref的start高8位
	SCCB_WR_Reg(0X1A,endx>>2);			//设置Vref的end的高8位
    
	temp=SCCB_RD_Reg(0X32);				//读取Href之前的值
	temp&=0XC0;
	temp|=((endy&0X07)<<3)|(sy&0X07);
	SCCB_WR_Reg(0X32,temp);
	SCCB_WR_Reg(0X17,sy>>3);			//设置Href的start高8位
	SCCB_WR_Reg(0X18,endy>>3);			//设置Href的end的高8位
}




// int init_rgb565_qvga_25fps_new(void)
//{
//  
//  delay_ms(100);
//  SCCB_WR_Reg(0x3a, 0x04);
//    SCCB_WR_Reg(0x40, 0xd0);
//    SCCB_WR_Reg(0x12, 0x14);
//    SCCB_WR_Reg(0x32, 0x80);
//    SCCB_WR_Reg(0x17, 0x16);
//    SCCB_WR_Reg(0x18, 0x04);
//    SCCB_WR_Reg(0x19, 0x02);
//    SCCB_WR_Reg(0x1a, 0x7b);
//    SCCB_WR_Reg(0x03, 0x06);
//    SCCB_WR_Reg(0x0c, 0x04);
//    SCCB_WR_Reg(0x3e, 0x00);
//    SCCB_WR_Reg(0x70, 0x3a);
//    SCCB_WR_Reg(0x71, 0x35);
//    SCCB_WR_Reg(0x72, 0x11);
//    SCCB_WR_Reg(0x73, 0x00);
//    SCCB_WR_Reg(0xa2, 0x02);
//    SCCB_WR_Reg(0x11, 0x81);
//     
//    SCCB_WR_Reg(0x7a, 0x20);
//    SCCB_WR_Reg(0x7b, 0x1c);
//    SCCB_WR_Reg(0x7c, 0x28);
//    SCCB_WR_Reg(0x7d, 0x3c);
//    SCCB_WR_Reg(0x7e, 0x55);
//    SCCB_WR_Reg(0x7f, 0x68);
//    SCCB_WR_Reg(0x80, 0x76);
//    SCCB_WR_Reg(0x81, 0x80);
//    SCCB_WR_Reg(0x82, 0x88);
//    SCCB_WR_Reg(0x83, 0x8f);
//    SCCB_WR_Reg(0x84, 0x96);
//    SCCB_WR_Reg(0x85, 0xa3);
//    SCCB_WR_Reg(0x86, 0xaf);
//    SCCB_WR_Reg(0x87, 0xc4);
//    SCCB_WR_Reg(0x88, 0xd7);
//    SCCB_WR_Reg(0x89, 0xe8);
//     
//    SCCB_WR_Reg(0x13, 0xe0);
//    SCCB_WR_Reg(0x00, 0x00);
//     
//    SCCB_WR_Reg(0x10, 0x00);
//    SCCB_WR_Reg(0x0d, 0x00);
//    SCCB_WR_Reg(0x14, 0x28);
//    SCCB_WR_Reg(0xa5, 0x05);
//    SCCB_WR_Reg(0xab, 0x07);
//    SCCB_WR_Reg(0x24, 0x75);
//    SCCB_WR_Reg(0x25, 0x63);
//    SCCB_WR_Reg(0x26, 0xA5);
//    SCCB_WR_Reg(0x9f, 0x78);
//    SCCB_WR_Reg(0xa0, 0x68);
//    SCCB_WR_Reg(0xa1, 0x03);
//    SCCB_WR_Reg(0xa6, 0xdf);
//    SCCB_WR_Reg(0xa7, 0xdf);
//    SCCB_WR_Reg(0xa8, 0xf0);
//    SCCB_WR_Reg(0xa9, 0x90);
//    SCCB_WR_Reg(0xaa, 0x94);
//    SCCB_WR_Reg(0x13, 0xe5);
// 
//    SCCB_WR_Reg(0x0e, 0x61);
//    SCCB_WR_Reg(0x0f, 0x4b);
//    SCCB_WR_Reg(0x16, 0x02);
//    SCCB_WR_Reg(0x1e, 0x07);
//    SCCB_WR_Reg(0x21, 0x02);
//    SCCB_WR_Reg(0x22, 0x91);
//    SCCB_WR_Reg(0x29, 0x07);
//    SCCB_WR_Reg(0x33, 0x0b);
//    SCCB_WR_Reg(0x35, 0x0b);
//    SCCB_WR_Reg(0x37, 0x1d);
//    SCCB_WR_Reg(0x38, 0x71);
//    SCCB_WR_Reg(0x39, 0x2a);
//    SCCB_WR_Reg(0x3c, 0x78);
//    SCCB_WR_Reg(0x4d, 0x40);
//    SCCB_WR_Reg(0x4e, 0x20);
//    SCCB_WR_Reg(0x69, 0x00);
//    SCCB_WR_Reg(0x6b, 0x60);
//    SCCB_WR_Reg(0x74, 0x19);
//    SCCB_WR_Reg(0x8d, 0x4f);
//    SCCB_WR_Reg(0x8e, 0x00);
//    SCCB_WR_Reg(0x8f, 0x00);
//    SCCB_WR_Reg(0x90, 0x00);
//    SCCB_WR_Reg(0x91, 0x00);
//    SCCB_WR_Reg(0x92, 0x00);
//    SCCB_WR_Reg(0x96, 0x00);
//    SCCB_WR_Reg(0x9a, 0x80);
//    SCCB_WR_Reg(0xb0, 0x84);
//    SCCB_WR_Reg(0xb1, 0x0c);
//    SCCB_WR_Reg(0xb2, 0x0e);
//    SCCB_WR_Reg(0xb3, 0x82);
//    SCCB_WR_Reg(0xb8, 0x0a);
// 
// 
// 
//    SCCB_WR_Reg(0x43, 0x14);
//    SCCB_WR_Reg(0x44, 0xf0);
//    SCCB_WR_Reg(0x45, 0x34);
//    SCCB_WR_Reg(0x46, 0x58);
//    SCCB_WR_Reg(0x47, 0x28);
//    SCCB_WR_Reg(0x48, 0x3a);
//    SCCB_WR_Reg(0x59, 0x88);
//    SCCB_WR_Reg(0x5a, 0x88);
//    SCCB_WR_Reg(0x5b, 0x44);
//    SCCB_WR_Reg(0x5c, 0x67);
//    SCCB_WR_Reg(0x5d, 0x49);
//    SCCB_WR_Reg(0x5e, 0x0e);
//    SCCB_WR_Reg(0x64, 0x04);
//    SCCB_WR_Reg(0x65, 0x20);
//    SCCB_WR_Reg(0x66, 0x05);
//    SCCB_WR_Reg(0x94, 0x04);
//    SCCB_WR_Reg(0x95, 0x08);
//    SCCB_WR_Reg(0x6c, 0x0a);
//    SCCB_WR_Reg(0x6d, 0x55);
//    SCCB_WR_Reg(0x6e, 0x11);
//    SCCB_WR_Reg(0x6f, 0x9f);
//    SCCB_WR_Reg(0x6a, 0x40);
//    SCCB_WR_Reg(0x01, 0x40);
//    SCCB_WR_Reg(0x02, 0x40);
//    SCCB_WR_Reg(0x13, 0xe7);
//    SCCB_WR_Reg(0x15, 0x02);  
//     
//     
//    SCCB_WR_Reg(0x4f, 0x80);
//    SCCB_WR_Reg(0x50, 0x80);
//    SCCB_WR_Reg(0x51, 0x00);
//    SCCB_WR_Reg(0x52, 0x22);
//    SCCB_WR_Reg(0x53, 0x5e);
//    SCCB_WR_Reg(0x54, 0x80);
//    SCCB_WR_Reg(0x58, 0x9e);
//     
//    SCCB_WR_Reg(0x41, 0x08);
//    SCCB_WR_Reg(0x3f, 0x00);
//    SCCB_WR_Reg(0x75, 0x05);
//    SCCB_WR_Reg(0x76, 0xe1);
//    SCCB_WR_Reg(0x4c, 0x00);
//    SCCB_WR_Reg(0x77, 0x01);
//    SCCB_WR_Reg(0x3d, 0xc2); 
//    SCCB_WR_Reg(0x4b, 0x09);
//    SCCB_WR_Reg(0xc9, 0x60);
//    SCCB_WR_Reg(0x41, 0x38);
//    SCCB_WR_Reg(0x56, 0x40);
//     
//    SCCB_WR_Reg(0x34, 0x11);
//    SCCB_WR_Reg(0x3b, 0x02); 
//                                 
//    SCCB_WR_Reg(0xa4, 0x89);
//    SCCB_WR_Reg(0x96, 0x00);
//    SCCB_WR_Reg(0x97, 0x30);
//    SCCB_WR_Reg(0x98, 0x20);
//    SCCB_WR_Reg(0x99, 0x30);
//    SCCB_WR_Reg(0x9a, 0x84);
//    SCCB_WR_Reg(0x9b, 0x29);
//    SCCB_WR_Reg(0x9c, 0x03);
//    SCCB_WR_Reg(0x9d, 0x4c);
//    SCCB_WR_Reg(0x9e, 0x3f);
//    SCCB_WR_Reg(0x78, 0x04);
//     
//    SCCB_WR_Reg(0x79, 0x01);
//    SCCB_WR_Reg(0xc8, 0xf0);
//    SCCB_WR_Reg(0x79, 0x0f);
//    SCCB_WR_Reg(0xc8, 0x00);
//    SCCB_WR_Reg(0x79, 0x10);
//    SCCB_WR_Reg(0xc8, 0x7e);
//    SCCB_WR_Reg(0x79, 0x0a);
//    SCCB_WR_Reg(0xc8, 0x80);
//    SCCB_WR_Reg(0x79, 0x0b);
//    SCCB_WR_Reg(0xc8, 0x01);
//    SCCB_WR_Reg(0x79, 0x0c);
//    SCCB_WR_Reg(0xc8, 0x0f);
//    SCCB_WR_Reg(0x79, 0x0d);
//    SCCB_WR_Reg(0xc8, 0x20);
//    SCCB_WR_Reg(0x79, 0x09);
//    SCCB_WR_Reg(0xc8, 0x80);
//    SCCB_WR_Reg(0x79, 0x02);
//    SCCB_WR_Reg(0xc8, 0xc0);
//    SCCB_WR_Reg(0x79, 0x03);
//    SCCB_WR_Reg(0xc8, 0x40);
//    SCCB_WR_Reg(0x79, 0x05);
//    SCCB_WR_Reg(0xc8, 0x30);
//    SCCB_WR_Reg(0x79, 0x26); 
//    SCCB_WR_Reg(0x09, 0x00);
//  OV7670_Window_Set(276,12,320,240);
//}
//
// void init_rgb565_qvga_25fps(void)
//{
//	SCCB_WR_Reg(0x12, 0x80);
//	delay_us(5000);
//	SCCB_WR_Reg(0x11, 0x80);
//	SCCB_WR_Reg(0x92, 0x7f);
//	SCCB_WR_Reg(0x93, 0x00);
//	SCCB_WR_Reg(0x9d, 0x9e);
//	SCCB_WR_Reg(0x3b, 0x0a);
//	SCCB_WR_Reg(0x13, 0xf2);
//	SCCB_WR_Reg(0x10, 0x00);
//	SCCB_WR_Reg(0x00, 0x00);
//	SCCB_WR_Reg(0x01, 0x80);
//	SCCB_WR_Reg(0x02, 0x80);
//	SCCB_WR_Reg(0x13, 0xf7);
//	//
//	SCCB_WR_Reg(0x12, 0x14);
//	SCCB_WR_Reg(0x04, 0x00);
//	SCCB_WR_Reg(0x18, 0x4b);
//	SCCB_WR_Reg(0x17, 0x23);
//	SCCB_WR_Reg(0x32, 0xbf);
//	SCCB_WR_Reg(0x19, 0x02);
//	SCCB_WR_Reg(0x1a, 0x7a);
//	SCCB_WR_Reg(0x03, 0x00);
//	//
//	SCCB_WR_Reg(0x0e, 0x84);
//	SCCB_WR_Reg(0x0f, 0x62);
//	SCCB_WR_Reg(0x15, 0x02);
//	SCCB_WR_Reg(0x16, 0x02);
//	SCCB_WR_Reg(0x1b, 0x01);
//	SCCB_WR_Reg(0x1e, 0x01);
//	SCCB_WR_Reg(0x29, 0x3c);
//	SCCB_WR_Reg(0x33, 0x00);
//	SCCB_WR_Reg(0x34, 0x07);
//	SCCB_WR_Reg(0x35, 0x84);
//	SCCB_WR_Reg(0x36, 0x00);
//	SCCB_WR_Reg(0x38, 0x13);
//	SCCB_WR_Reg(0x39, 0x43);
//	SCCB_WR_Reg(0x3a, 0x00);
//	SCCB_WR_Reg(0x3c, 0x6c);
//	SCCB_WR_Reg(0x3d, 0x90);
//	SCCB_WR_Reg(0x3f, 0x29);
//	SCCB_WR_Reg(0x40, 0xd1);
//	SCCB_WR_Reg(0x41, 0x20);
//	SCCB_WR_Reg(0x6b, 0x0a);
//	SCCB_WR_Reg(0xa1, 0xc8);
//	//
//	SCCB_WR_Reg(0x69, 0x80);
//	SCCB_WR_Reg(0x43, 0xf0);
//	SCCB_WR_Reg(0x44, 0x10);
//	SCCB_WR_Reg(0x45, 0x78);
//	SCCB_WR_Reg(0x46, 0xa8);
//	SCCB_WR_Reg(0x47, 0x60);
//	SCCB_WR_Reg(0x48, 0x80);
//	SCCB_WR_Reg(0x59, 0xba);
//	SCCB_WR_Reg(0x5a, 0x9a);
//	SCCB_WR_Reg(0x5b, 0x22);
//	SCCB_WR_Reg(0x5c, 0xb9);
//	SCCB_WR_Reg(0x5d, 0x9b);
//	SCCB_WR_Reg(0x5e, 0x10);
//	SCCB_WR_Reg(0x5f, 0xe0);
//	SCCB_WR_Reg(0x60, 0x85);
//	SCCB_WR_Reg(0x61, 0x60);
//	SCCB_WR_Reg(0x9f, 0x9d);
//	SCCB_WR_Reg(0xa0, 0xa0);
//	//
//	SCCB_WR_Reg(0x4f, 0xae);
//	SCCB_WR_Reg(0x50, 0x26);
//	SCCB_WR_Reg(0x51, 0x08);
//	SCCB_WR_Reg(0x52, 0x1a);
//	SCCB_WR_Reg(0x53, 0xa9);
//	SCCB_WR_Reg(0x54, 0x0f);
//	SCCB_WR_Reg(0x55, 0x05);
//	SCCB_WR_Reg(0x56, 0x46);
//	SCCB_WR_Reg(0x57, 0xcb);
//	SCCB_WR_Reg(0x58, 0x77);
//	//
//	SCCB_WR_Reg(0x8b, 0xcc);
//	SCCB_WR_Reg(0x8c, 0xcc);
//	SCCB_WR_Reg(0x8d, 0xcf);
//	//
//	SCCB_WR_Reg(0x6c, 0x40);
//	SCCB_WR_Reg(0x6d, 0x30);
//	SCCB_WR_Reg(0x6e, 0x4b);
//	SCCB_WR_Reg(0x6f, 0x60);
//	SCCB_WR_Reg(0x70, 0x70);
//	SCCB_WR_Reg(0x71, 0x70);
//	SCCB_WR_Reg(0x72, 0x70);
//	SCCB_WR_Reg(0x73, 0x70);
//	SCCB_WR_Reg(0x74, 0x60);
//	SCCB_WR_Reg(0x75, 0x60);
//	SCCB_WR_Reg(0x76, 0x50);
//	SCCB_WR_Reg(0x77, 0x48);
//	SCCB_WR_Reg(0x78, 0x3a);
//	SCCB_WR_Reg(0x79, 0x2e);
//	SCCB_WR_Reg(0x7a, 0x28);
//	SCCB_WR_Reg(0x7b, 0x22);
//	SCCB_WR_Reg(0x7c, 0x04);
//	SCCB_WR_Reg(0x7d, 0x07);
//	SCCB_WR_Reg(0x7e, 0x10);
//	SCCB_WR_Reg(0x7f, 0x28);
//	SCCB_WR_Reg(0x80, 0x36);
//	SCCB_WR_Reg(0x81, 0x44);
//	SCCB_WR_Reg(0x82, 0x52);
//	SCCB_WR_Reg(0x83, 0x60);
//	SCCB_WR_Reg(0x84, 0x6c);
//	SCCB_WR_Reg(0x85, 0x78);
//	SCCB_WR_Reg(0x86, 0x8c);
//	SCCB_WR_Reg(0x87, 0x9e);
//	SCCB_WR_Reg(0x88, 0xbb);
//	SCCB_WR_Reg(0x89, 0xd2);
//	SCCB_WR_Reg(0x8a, 0xe6);
//	//
//	SCCB_WR_Reg(0x14, 0x2e);
//	SCCB_WR_Reg(0x24, 0xb8);
//	SCCB_WR_Reg(0x25, 0xa8);
//}
//
//static void init_rgb565_qvga_12fps(void)
//{
//	SCCB_WR_Reg(0x12, 0x80);
//	delay_us(5000);
//	SCCB_WR_Reg(0x11, 0x83);
//	SCCB_WR_Reg(0x9e, 0x29);
//	SCCB_WR_Reg(0x3b, 0x02);
//	SCCB_WR_Reg(0x13, 0xf2);
//	SCCB_WR_Reg(0x00, 0x00);
//	SCCB_WR_Reg(0x10, 0x00);
//	SCCB_WR_Reg(0x01, 0x80);
//	SCCB_WR_Reg(0x02, 0x80);
//	SCCB_WR_Reg(0x13, 0xf7);
//	//
////	SCCB_WR_Reg(0x12, 0x15);
//	SCCB_WR_Reg(0x12, 0x14); // modified by me
//	SCCB_WR_Reg(0x04, 0x00);
//	SCCB_WR_Reg(0x0c, 0x04);
//	SCCB_WR_Reg(0x18, 0x4A);
//	SCCB_WR_Reg(0x17, 0x22);
//	SCCB_WR_Reg(0x32, 0x89);
//	SCCB_WR_Reg(0x19, 0x02);
//	SCCB_WR_Reg(0x1a, 0x7a);
//	SCCB_WR_Reg(0x03, 0x00);
//	//
//	SCCB_WR_Reg(0x0e, 0x84);
//	SCCB_WR_Reg(0x0f, 0x62);
//	SCCB_WR_Reg(0x15, 0x02);
//	SCCB_WR_Reg(0x1b, 0x01);
//	SCCB_WR_Reg(0x1e, 0x01);
//	SCCB_WR_Reg(0x29, 0x3c);
//	SCCB_WR_Reg(0x33, 0x00);
//	SCCB_WR_Reg(0x34, 0x07);
//	SCCB_WR_Reg(0x35, 0x84);
//	SCCB_WR_Reg(0x36, 0x00);
//	SCCB_WR_Reg(0x38, 0x13);
//	SCCB_WR_Reg(0x39, 0x43);
//	SCCB_WR_Reg(0x3c, 0x68);
//	SCCB_WR_Reg(0x3d, 0x19);
////	SCCB_WR_Reg(0x40, 0xc1);
//	SCCB_WR_Reg(0x40, 0xd1); // modified by me
//	SCCB_WR_Reg(0x69, 0x80);
//	SCCB_WR_Reg(0x6b, 0x0a);
//	SCCB_WR_Reg(0xa1, 0x08);
//	//
//	SCCB_WR_Reg(0x8b, 0xcc);
//	SCCB_WR_Reg(0x8c, 0xcc);
//	SCCB_WR_Reg(0x8d, 0xcf);
//	//
//	SCCB_WR_Reg(0x14, 0x2e);
//	SCCB_WR_Reg(0x25, 0x58);
//	SCCB_WR_Reg(0x24, 0x68);
//}
//
//static void init_yuv_12fps(void)
//{
//	SCCB_WR_Reg(0x12, 0x80);
//	delay_us(20000);
//	SCCB_WR_Reg(0x11, 0x00);
//	SCCB_WR_Reg(0x92, 0x66);
//	SCCB_WR_Reg(0x93, 0x00);
//	SCCB_WR_Reg(0x9d, 0x4c);
//	SCCB_WR_Reg(0x9e, 0x3f);
//	SCCB_WR_Reg(0x3b, 0x0a);
//	SCCB_WR_Reg(0x13, 0xf0);
//	SCCB_WR_Reg(0x10, 0x00);
//	SCCB_WR_Reg(0x00, 0x00);
////	SCCB_WR_Reg(0x00, 0xff); // modified by me
////	SCCB_WR_Reg(0x07, 0x3f); // added by me
//	SCCB_WR_Reg(0x01, 0x80);
//	SCCB_WR_Reg(0x02, 0x80);
//	SCCB_WR_Reg(0x13, 0xf5);
//	//
//	SCCB_WR_Reg(0x12, 0x00);
//	SCCB_WR_Reg(0x04, 0x00);
//	SCCB_WR_Reg(0x18, 0x01);
//	SCCB_WR_Reg(0x17, 0x13);
//	SCCB_WR_Reg(0x32, 0x92);
//	SCCB_WR_Reg(0x19, 0x02);
//	SCCB_WR_Reg(0x1a, 0x7a);
//	SCCB_WR_Reg(0x03, 0x00);
//	//
//	SCCB_WR_Reg(0x0e, 0x84);
//	SCCB_WR_Reg(0x0f, 0x62);
//	SCCB_WR_Reg(0x15, 0x02);
//	SCCB_WR_Reg(0x16, 0x02);
//	SCCB_WR_Reg(0x1b, 0x01);
//	SCCB_WR_Reg(0x1e, 0x01);
//	SCCB_WR_Reg(0x29, 0x3c);
//	SCCB_WR_Reg(0x33, 0x00);
//	SCCB_WR_Reg(0x34, 0x07);
//	SCCB_WR_Reg(0x35, 0x84);
//	SCCB_WR_Reg(0x36, 0x00);
//	SCCB_WR_Reg(0x38, 0x13);
//	SCCB_WR_Reg(0x39, 0x43);
//	SCCB_WR_Reg(0x3a, 0x00);
//	SCCB_WR_Reg(0x3c, 0x6c);
//	SCCB_WR_Reg(0x3d, 0x90);
//	SCCB_WR_Reg(0x3f, 0x29);
//	SCCB_WR_Reg(0x40, 0xc1);
//	SCCB_WR_Reg(0x41, 0x20);
//	SCCB_WR_Reg(0x6b, 0x0a);
//	SCCB_WR_Reg(0xa1, 0xc8);
//	//
//	SCCB_WR_Reg(0x69, 0x80);
//	SCCB_WR_Reg(0x43, 0xf0);
//	SCCB_WR_Reg(0x44, 0x10);
//	SCCB_WR_Reg(0x45, 0x78);
//	SCCB_WR_Reg(0x46, 0xa8);
//	SCCB_WR_Reg(0x47, 0x60);
//	SCCB_WR_Reg(0x48, 0x80);
//	SCCB_WR_Reg(0x59, 0xba);
//	SCCB_WR_Reg(0x5a, 0x9a);
//	SCCB_WR_Reg(0x5b, 0x22);
//	SCCB_WR_Reg(0x5c, 0xb9);
//	SCCB_WR_Reg(0x5d, 0x9b);
//	SCCB_WR_Reg(0x5e, 0x10);
//	SCCB_WR_Reg(0x5f, 0xe0);
//	SCCB_WR_Reg(0x60, 0x85);
//	SCCB_WR_Reg(0x61, 0x60);
//	SCCB_WR_Reg(0x9f, 0x9d);
//	SCCB_WR_Reg(0xa0, 0xa0);
//	//
//	SCCB_WR_Reg(0x4f, 0x66);
//	SCCB_WR_Reg(0x50, 0x6b);
//	SCCB_WR_Reg(0x51, 0x05);
//	SCCB_WR_Reg(0x52, 0x19);
//	SCCB_WR_Reg(0x53, 0x40);
//	SCCB_WR_Reg(0x54, 0x59);
//	SCCB_WR_Reg(0x55, 0x40);
//	SCCB_WR_Reg(0x56, 0x40);
//	SCCB_WR_Reg(0x57, 0x40);
//	SCCB_WR_Reg(0x58, 0x0d);
//	//
//	SCCB_WR_Reg(0x8b, 0xcc);
//	SCCB_WR_Reg(0x8c, 0xcc);
//	SCCB_WR_Reg(0x8d, 0xcf);
//	//
//	SCCB_WR_Reg(0x6c, 0x40);
//	SCCB_WR_Reg(0x6d, 0x30);
//	SCCB_WR_Reg(0x6e, 0x4b);
//	SCCB_WR_Reg(0x6f, 0x60);
//	SCCB_WR_Reg(0x70, 0x70);
//	SCCB_WR_Reg(0x71, 0x70);
//	SCCB_WR_Reg(0x72, 0x70);
//	SCCB_WR_Reg(0x73, 0x70);
//	SCCB_WR_Reg(0x74, 0x60);
//	SCCB_WR_Reg(0x75, 0x60);
//	SCCB_WR_Reg(0x76, 0x50);
//	SCCB_WR_Reg(0x77, 0x48);
//	SCCB_WR_Reg(0x78, 0x3a);
//	SCCB_WR_Reg(0x79, 0x2e);
//	SCCB_WR_Reg(0x7a, 0x28);
//	SCCB_WR_Reg(0x7b, 0x22);
//	SCCB_WR_Reg(0x7c, 0x04);
//	SCCB_WR_Reg(0x7d, 0x07);
//	SCCB_WR_Reg(0x7e, 0x10);
//	SCCB_WR_Reg(0x7f, 0x28);
//	SCCB_WR_Reg(0x80, 0x36);
//	SCCB_WR_Reg(0x81, 0x44);
//	SCCB_WR_Reg(0x82, 0x52);
//	SCCB_WR_Reg(0x83, 0x60);
//	SCCB_WR_Reg(0x84, 0x6c);
//	SCCB_WR_Reg(0x85, 0x78);
//	SCCB_WR_Reg(0x86, 0x8c);
//	SCCB_WR_Reg(0x87, 0x9e);
//	SCCB_WR_Reg(0x88, 0xbb);
//	SCCB_WR_Reg(0x89, 0xd2);
//	SCCB_WR_Reg(0x8a, 0xe6);
//	//
//	SCCB_WR_Reg(0x14, 0x1a);
//	SCCB_WR_Reg(0x24, 0x78);
//	SCCB_WR_Reg(0x25, 0x6c);
//	SCCB_WR_Reg(0x62, 0x00);
//	SCCB_WR_Reg(0x63, 0x00);
//	SCCB_WR_Reg(0x64, 0x18);
//	SCCB_WR_Reg(0x65, 0x30);
//	SCCB_WR_Reg(0x66, 0x05);
//	SCCB_WR_Reg(0x94, 0x18);
//	SCCB_WR_Reg(0x95, 0x1c);
//	delay_us(8000);
//	SCCB_WR_Reg(0x13, 0xf7);
//}
//
//static void init_yuv_25fps(void)
//{
//	SCCB_WR_Reg(0x12, 0x80);
//	delay_us(5000);
//	SCCB_WR_Reg(0x11, 0x80);
//	SCCB_WR_Reg(0x92, 0x66);
//	SCCB_WR_Reg(0x93, 0x00);
//	SCCB_WR_Reg(0x9d, 0x4c);
//	SCCB_WR_Reg(0x9e, 0x3f);
//	SCCB_WR_Reg(0x3b, 0x0a);
//	SCCB_WR_Reg(0x13, 0xf0);
//	SCCB_WR_Reg(0x10, 0xff);
//	SCCB_WR_Reg(0x00, 0xff);
//	SCCB_WR_Reg(0x01, 0x80);
//	SCCB_WR_Reg(0x02, 0x80);
//	SCCB_WR_Reg(0x13, 0xf5);
//	//
//	SCCB_WR_Reg(0x12, 0x00);
//	SCCB_WR_Reg(0x04, 0x00);
//	SCCB_WR_Reg(0x18, 0x01);
//	SCCB_WR_Reg(0x17, 0x13);
//	SCCB_WR_Reg(0x32, 0x92);
//	SCCB_WR_Reg(0x19, 0x02);
//	SCCB_WR_Reg(0x1a, 0x7a);
//	SCCB_WR_Reg(0x03, 0x00);
//	//
//	SCCB_WR_Reg(0x0e, 0x84);
//	SCCB_WR_Reg(0x0f, 0x62);
//	SCCB_WR_Reg(0x15, 0x02);
//	SCCB_WR_Reg(0x16, 0x02);
//	SCCB_WR_Reg(0x1b, 0x01);
//	SCCB_WR_Reg(0x1e, 0x01);
//	SCCB_WR_Reg(0x29, 0x3c);
//	SCCB_WR_Reg(0x33, 0x00);
//	SCCB_WR_Reg(0x34, 0x07);
//	SCCB_WR_Reg(0x35, 0x84);
//	SCCB_WR_Reg(0x36, 0x00);
//	SCCB_WR_Reg(0x38, 0x13);
//	SCCB_WR_Reg(0x39, 0x43);
//	SCCB_WR_Reg(0x3a, 0x00);
//	SCCB_WR_Reg(0x3c, 0x6c);
//	SCCB_WR_Reg(0x3d, 0x90);
//	SCCB_WR_Reg(0x3f, 0x29);
//	SCCB_WR_Reg(0x40, 0xc1);
//	SCCB_WR_Reg(0x41, 0x20);
//	SCCB_WR_Reg(0x6b, 0x0a);
//	SCCB_WR_Reg(0xa1, 0xc8);
//	//
//	SCCB_WR_Reg(0x69, 0x80);
//	SCCB_WR_Reg(0x43, 0xf0);
//	SCCB_WR_Reg(0x44, 0x10);
//	SCCB_WR_Reg(0x45, 0x78);
//	SCCB_WR_Reg(0x46, 0xa8);
//	SCCB_WR_Reg(0x47, 0x60);
//	SCCB_WR_Reg(0x48, 0x80);
//	SCCB_WR_Reg(0x59, 0xba);
//	SCCB_WR_Reg(0x5a, 0x9a);
//	SCCB_WR_Reg(0x5b, 0x22);
//	SCCB_WR_Reg(0x5c, 0xb9);
//	SCCB_WR_Reg(0x5d, 0x9b);
//	SCCB_WR_Reg(0x5e, 0x10);
//	SCCB_WR_Reg(0x5f, 0xe0);
//	SCCB_WR_Reg(0x60, 0x85);
//	SCCB_WR_Reg(0x61, 0x60);
//	SCCB_WR_Reg(0x9f, 0x9d);
//	SCCB_WR_Reg(0xa0, 0xa0);
//	//
//	SCCB_WR_Reg(0x4f, 0x66);
//	SCCB_WR_Reg(0x50, 0x6b);
//	SCCB_WR_Reg(0x51, 0x05);
//	SCCB_WR_Reg(0x52, 0x19);
//	SCCB_WR_Reg(0x53, 0x40);
//	SCCB_WR_Reg(0x54, 0x59);
//	SCCB_WR_Reg(0x55, 0x40);
//	SCCB_WR_Reg(0x56, 0x40);
//	SCCB_WR_Reg(0x57, 0x40);
//	SCCB_WR_Reg(0x58, 0x0d);
//	//
//	SCCB_WR_Reg(0x8b, 0xcc);
//	SCCB_WR_Reg(0x8c, 0xcc);
//	SCCB_WR_Reg(0x8d, 0xcf);
//	//
//	SCCB_WR_Reg(0x6c, 0x40);
//	SCCB_WR_Reg(0x6d, 0x30);
//	SCCB_WR_Reg(0x6e, 0x4b);
//	SCCB_WR_Reg(0x6f, 0x60);
//	SCCB_WR_Reg(0x70, 0x70);
//	SCCB_WR_Reg(0x71, 0x70);
//	SCCB_WR_Reg(0x72, 0x70);
//	SCCB_WR_Reg(0x73, 0x70);
//	SCCB_WR_Reg(0x74, 0x60);
//	SCCB_WR_Reg(0x75, 0x60);
//	SCCB_WR_Reg(0x76, 0x50);
//	SCCB_WR_Reg(0x77, 0x48);
//	SCCB_WR_Reg(0x78, 0x3a);
//	SCCB_WR_Reg(0x79, 0x2e);
//	SCCB_WR_Reg(0x7a, 0x28);
//	SCCB_WR_Reg(0x7b, 0x22);
//	SCCB_WR_Reg(0x7c, 0x04);
//	SCCB_WR_Reg(0x7d, 0x07);
//	SCCB_WR_Reg(0x7e, 0x10);
//	SCCB_WR_Reg(0x7f, 0x28);
//	SCCB_WR_Reg(0x80, 0x36);
//	SCCB_WR_Reg(0x81, 0x44);
//	SCCB_WR_Reg(0x82, 0x52);
//	SCCB_WR_Reg(0x83, 0x60);
//	SCCB_WR_Reg(0x84, 0x6c);
//	SCCB_WR_Reg(0x85, 0x78);
//	SCCB_WR_Reg(0x86, 0x8c);
//	SCCB_WR_Reg(0x87, 0x9e);
//	SCCB_WR_Reg(0x88, 0xbb);
//	SCCB_WR_Reg(0x89, 0xd2);
//	SCCB_WR_Reg(0x8a, 0xe6);
//	//
//	SCCB_WR_Reg(0x14, 0x1a);
//	SCCB_WR_Reg(0x24, 0x7b);
//	SCCB_WR_Reg(0x25, 0x6c);
//	SCCB_WR_Reg(0x26, 0xc4);
//	//
//	SCCB_WR_Reg(0x62, 0x00);
//	SCCB_WR_Reg(0x63, 0x00);
//	SCCB_WR_Reg(0x64, 0x18);
//	SCCB_WR_Reg(0x65, 0x30);
//	SCCB_WR_Reg(0x66, 0x05);
//	SCCB_WR_Reg(0x94, 0x18);
//	SCCB_WR_Reg(0x95, 0x1c);
//	delay_us(4000);
//	SCCB_WR_Reg(0x13, 0xf7);
//}
/**
* @brief  Configures the OV2640 camera in JPEG mode.
* @param  JPEGImageSize: JPEG image size
* @retval None
*/
void OV2640_JPEGConfig(ImageFormat_TypeDef ImageFormat)
{
    uint32_t i;
    
    OV2640_Reset();
    delay_ms(CONFIG_DELAY);
    
    /* Initialize OV2640 */
    for(i=0; i<(sizeof(OV2640_JPEG_INIT)/2); i++)
    {
        SCCB_WR_Reg(OV2640_JPEG_INIT[i][0], OV2640_JPEG_INIT[i][1]);
        delay_us(OV2640_DELAY_US);
    }
    
    /* Set to output YUV422 */
    for(i=0; i<(sizeof(OV2640_YUV422)/2); i++)
    {
        SCCB_WR_Reg(OV2640_YUV422[i][0], OV2640_YUV422[i][1]);
        delay_us(OV2640_DELAY_US);
    }
    
    SCCB_WR_Reg(0xff, 0x01);
    SCCB_WR_Reg(0x15, 0x00);
    
    /* Set to output JPEG */
    for(i=0; i<(sizeof(OV2640_JPEG)/2); i++)
    {
        SCCB_WR_Reg(OV2640_JPEG[i][0], OV2640_JPEG[i][1]);
        delay_us(OV2640_DELAY_US);
    }
    
    delay_ms(CONFIG_DELAY);
    
    switch(ImageFormat)
    {
        //    case JPEG_160x120:
        //        {
        //            for(i=0; i<(sizeof(OV2640_160x120_JPEG)/2); i++)
        //            {
        //                SCCB_WR_Reg(OV2640_160x120_JPEG[i][0], OV2640_160x120_JPEG[i][1]);
        //		delay_ms(1);
        //            }
        //            break;
        //        }
    case JPEG_176x144:
        {
            for(i=0; i<(sizeof(OV2640_176x144_JPEG)/2); i++)
            {
                SCCB_WR_Reg(OV2640_176x144_JPEG[i][0], OV2640_176x144_JPEG[i][1]);
                delay_us(OV2640_DELAY_US);
            }
            break;
        }
    case JPEG_320x240:
        {
            for(i=0; i<(sizeof(OV2640_320x240_JPEG)/2); i++)
            {
                SCCB_WR_Reg(OV2640_320x240_JPEG[i][0], OV2640_320x240_JPEG[i][1]);
                delay_us(OV2640_DELAY_US);
            }
            break;
        }
    case JPEG_352x288:
        {
            for(i=0; i<(sizeof(OV2640_352x288_JPEG)/2); i++)
            {
                SCCB_WR_Reg(OV2640_352x288_JPEG[i][0], OV2640_352x288_JPEG[i][1]);
                delay_us(OV2640_DELAY_US);
            }
            break;
        }
        /////////////////////////////////////////////////////////////////////////
    case JPEG_640x480:
        {
            for(i=0; i<(sizeof(ov2640_640x480_jpeg)/2); i++)
            {
                SCCB_WR_Reg(ov2640_640x480_jpeg[i][0], ov2640_640x480_jpeg[i][1]);
                delay_us(OV2640_DELAY_US);
            }
            break;
        }
    case JPEG_800x600:
        {
            for(i = 0; i<(sizeof(ov2640_800x600_jpeg)/2); i++)
            {
                SCCB_WR_Reg(ov2640_800x600_jpeg[i][0], ov2640_800x600_jpeg[i][1]);
                delay_us(OV2640_DELAY_US);
            }
            break;
        }
    case JPEG_1024x768:
        {
            for(i = 0; i<(sizeof(ov2640_1024x768_jpeg)/2); i++)
            {
                SCCB_WR_Reg(ov2640_1024x768_jpeg[i][0], ov2640_1024x768_jpeg[i][1]);
                delay_us(OV2640_DELAY_US);
            }
            break;
        }
        ////////////////////////////////////////////////////////////////////
        
    default:
        {
            for(i=0; i<(sizeof(OV2640_160x120_JPEG)/2); i++)
            {
                SCCB_WR_Reg(OV2640_160x120_JPEG[i][0], OV2640_160x120_JPEG[i][1]);
                delay_us(OV2640_DELAY_US);
            }
            break;
        }
    }
}



/**
* @brief  Resets the OV2640 camera.
* @param  None
* @retval None
*/
void OV2640_Reset(void)
{
    SCCB_WR_Reg(OV2640_DSP_RA_DLMT, 0x01);
    SCCB_WR_Reg(OV2640_SENSOR_COM7, 0x80);

}


/**
* @brief  Configures the OV2640 camera brightness.
* @param  Brightness: Brightness value, where Brightness can be: 
*         0x40 for Brightness +2,
*         0x30 for Brightness +1,
*         0x20 for Brightness 0,
*         0x10 for Brightness -1,
*         0x00 for Brightness -2,
* @retval None
*/
void OV2640_BrightnessConfig(uint8_t Brightness)
{
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0x7c, 0x00);
    SCCB_WR_Reg(0x7d, 0x04);
    SCCB_WR_Reg(0x7c, 0x09);
    SCCB_WR_Reg(0x7d, Brightness);
    SCCB_WR_Reg(0x7d, 0x00);
}

/**
* @brief  Configures the OV2640 camera Black and white mode.
* @param  BlackWhite: BlackWhite value, where BlackWhite can be: 
*         0x18 for B&W,
*         0x40 for Negative,
*         0x58 for B&W negative,
*         0x00 for Normal,
* @retval None
*/
void OV2640_BandWConfig(uint8_t BlackWhite)
{
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0x7c, 0x00);
    SCCB_WR_Reg(0x7d, BlackWhite);
    SCCB_WR_Reg(0x7c, 0x05);
    SCCB_WR_Reg(0x7d, 0x80);
    SCCB_WR_Reg(0x7d, 0x80);
}

/**
* @brief  Configures the OV2640 camera color effects.
* @param  value1: Color effects value1
* @param  value2: Color effects value2
*         where value1 and value2 can be: 
*         value1 = 0x40, value2 = 0xa6 for Antique,
*         value1 = 0xa0, value2 = 0x40 for Bluish,
*         value1 = 0x40, value2 = 0x40 for Greenish,
*         value1 = 0x40, value2 = 0xc0 for Reddish,
* @retval None
*/
void OV2640_ColorEffectsConfig(uint8_t value1, uint8_t value2)
{
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0x7c, 0x00);
    SCCB_WR_Reg(0x7d, 0x18);
    SCCB_WR_Reg(0x7c, 0x05);
    SCCB_WR_Reg(0x7d, value1);
    SCCB_WR_Reg(0x7d, value2);
}

/**
* @brief  Configures the OV2640 camera contrast.
* @param  value1: Contrast value1
* @param  value2: Contrast value2
*         where value1 and value2 can be: 
*         value1 = 0x28, value2 = 0x0c for Contrast +2,
*         value1 = 0x24, value2 = 0x16 for Contrast +1,
*         value1 = 0x20, value2 = 0x20 for Contrast 0,
*         value1 = 0x1c, value2 = 0x2a for Contrast -1,
*         value1 = 0x18, value2 = 0x34 for Contrast -2,
* @retval None
*/
void OV2640_ContrastConfig(uint8_t value1, uint8_t value2)
{
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0x7c, 0x00);
    SCCB_WR_Reg(0x7d, 0x04);
    SCCB_WR_Reg(0x7c, 0x07);
    SCCB_WR_Reg(0x7d, 0x20);
    SCCB_WR_Reg(0x7d, value1);
    SCCB_WR_Reg(0x7d, value2);
    SCCB_WR_Reg(0x7d, 0x06);
}


void SCCB_WriteRegs(const uint8_t* pbuf)
{
    while(1)
    {
        if((*pbuf == 0) && (*(pbuf + 1) == 0))
        {
            break;
        }
        else
        {
            SCCB_WR_Reg(*pbuf++, *pbuf++);
        }
    }
}
void OV2640_AutoExposure(uint8_t level)
{
    switch(level)
    {
    case 0:
        SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL0);
        break;
    case 1:
        SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL1);
        break;
    case 2:
        SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL2);
        break;
    case 3:
        SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL3);
        break;
    case 4:
        SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL4);
        break;
    default:
        SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL0);
        break;
    }
    
}








