#include "lcd.h"

#include "stdlib.h"
#include "math.h"
#include "string.h"
#include "stdio.h"
#include "image.h"

//#include "gpio.h"
//#include "stm32f0xx_hal.h"

#define  YMAX 320
#define  XMAX 240

#define min(x,y) ((x<y)?x:y)
#define max(x,y) ((x>y)?x:y)


#define COLOR_GRADE1 	0x2244//0x19c3//0x2ae5//0x5deb//				0x0748			//0x59e2 //1864
#define COLOR_GRADE2 	0xe6e0//0x62c3//0x7b83//0xff07//0xd561								//0x61D5  //54625
#define COLOR_GRADE3 	0xfcc6//0xfcc6//0xf4c0				//	0x7e05	///62656
#define COLOR_GRADE4 	0x78a1//0xf963//0xf96a								//0xcf78	//63850
#define COLOR_GRADE5 0x4865//0x90ca//0xe259								//57945
#define COLOR_GRADE6 0x1824//0x2823//0x5047//0xcd0a								//52490

#define COLOR_BLUE  0x02af//0x057f								//1407

PORT_PIN_T RS = {GPIOA,GPIO_PIN_11};
PORT_PIN_T WR = {GPIOA,GPIO_PIN_12};
PORT_PIN_T RD = {GPIOF,GPIO_PIN_6};
PORT_PIN_T CS = {GPIOA,GPIO_PIN_10};
PORT_PIN_T RST = {GPIOA,GPIO_PIN_8};
PORT_PIN_T LCD_EN = {GPIOA,GPIO_PIN_15};

uint8_t notes0[] = {0x81,0x02,0x17,0x85,0x02,0x06,0x13,0x17,0x8D,0x02,0x05,0x13,0x17,0x8E,0x03,0x04,0x15,0x16,0x8F,0x90,0x03,0x04,0x15,0x16,0x91,0x02,0x05,0x14,0x17,0x92,0x02,0x06,0x13,0x17,0x9C,0x02,0x05,0x06,0x13,0x14,0x17,0x9D,0x02,0x04,0x05,0x14,0x15,0x17,0x9E,0x02,0x03,0x04,0x15,0x16,0x17,0x9F,0x03,0x16,0xA0,0xFF};
uint8_t notes1[] = {0x81,0x07,0x08,0x82,0x06,0x08,0x83,0x05,0x08,0x84,0x04,0x08,0x8E,0x05,0x08,0x8F,0x06,0x07,0x90,0x91,0x06,0x07,0x92,0x05,0x08,0x93,0x04,0x08,0x9D,0x05,0x08,0x9E,0x06,0x08,0x9F,0x07,0x08,0xA0,0xFF};
uint8_t notes2[] = {0x81,0x03,0x16,0x82,0x04,0x15,0x16,0x17,0x83,0x05,0x14,0x15,0x17,0x84,0x06,0x17,0x85,0x13,0x17,0x8E,0x05,0x17,0x8F,0x05,0x16,0x90,0x05,0x15,0x91,0x03,0x04,0x05,0x14,0x92,0x02,0x05,0x93,0x02,0x06,0x9C,0x02,0x05,0x06,0x13,0x9D,0x02,0x04,0x05,0x14,0x9E,0x02,0x03,0x04,0x15,0x9F,0x03,0x16,0xA0,0xFF};
uint8_t notes3[] = {0x81,0x02,0x15,0x82,0x03,0x14,0x15,0x16,0x83,0x04,0x13,0x14,0x16,0x84,0x05,0x12,0x13,0x16,0x85,0x12,0x16,0x8E,0x04,0x12,0x13,0x16,0x8F,0x04,0x13,0x14,0x15,0x90,0x04,0x14,0x91,0x04,0x12,0x14,0x15,0x92,0x13,0x16,0x93,0x12,0x16,0x9C,0x05,0x12,0x13,0x16,0x9D,0x04,0x13,0x14,0x16,0x9E,0x03,0x14,0x15,0x16,0x9F,0x02,0x15,0xA0,0xFF};
uint8_t notes4[] = {0x81,0x02,0x03,0x82,0x02,0x04,0x16,0x17,0x83,0x02,0x05,0x15,0x17,0x84,0x02,0x06,0x14,0x17,0x85,0x02,0x06,0x13,0x17,0x8E,0x02,0x17,0x8F,0x03,0x04,0x05,0x16,0x90,0x04,0x15,0x91,0x05,0x14,0x15,0x16,0x92,0x14,0x17,0x93,0x13,0x17,0x9D,0x14,0x17,0x9E,0x15,0x17,0x9F,0x16,0x17,0xA0,0xFF};
uint8_t notes5[] = {0x81,0x03,0x16,0x82,0x02,0x03,0x04,0x15,0x83,0x02,0x04,0x05,0x14,0x84,0x02,0x13,0x85,0x02,0x06,0x8E,0x02,0x14,0x8F,0x03,0x04,0x05,0x15,0x90,0x04,0x15,0x91,0x05,0x14,0x15,0x16,0x92,0x14,0x17,0x93,0x13,0x17,0x9C,0x06,0x13,0x14,0x17,0x9D,0x05,0x14,0x15,0x17,0x9E,0x04,0x15,0x16,0x17,0x9F,0x03,0x16,0xA0,0xFF};
uint8_t notes6[] = {0x81,0x03,0x16,0x82,0x02,0x03,0x04,0x15,0x83,0x02,0x04,0x05,0x14,0x84,0x02,0x05,0x06,0x13,0x85,0x02,0x06,0x8E,0x02,0x14,0x8F,0x03,0x04,0x05,0x15,0x90,0x04,0x15,0x91,0x03,0x04,0x05,0x14,0x15,0x16,0x92,0x02,0x05,0x14,0x17,0x93,0x02,0x06,0x13,0x17,0x9C,0x02,0x05,0x06,0x13,0x14,0x17,0x9D,0x02,0x04,0x05,0x14,0x15,0x17,0x9E,0x02,0x03,0x04,0x15,0x16,0x17,0x9F,0x03,0x16,0xA0,0xFF};
uint8_t notes7[] = {0x81,0x02,0x16,0x82,0x03,0x16,0x83,0x04,0x16,0x84,0x05,0x16,0x85,0x12,0x16,0x8D,0x13,0x16,0x8E,0x14,0x15,0x8F,0x90,0x14,0x15,0x91,0x13,0x16,0x92,0x12,0x16,0x9D,0x13,0x16,0x9E,0x14,0x16,0x9F,0x15,0x16,0xA0,0xFF};
uint8_t notes8[] = {0x81,0x03,0x17,0x82,0x02,0x03,0x04,0x17,0x83,0x02,0x04,0x05,0x17,0x84,0x02,0x05,0x06,0x17,0x85,0x02,0x06,0x13,0x17,0x8E,0x02,0x17,0x8F,0x03,0x04,0x05,0x16,0x90,0x04,0x15,0x91,0x03,0x04,0x05,0x14,0x15,0x16,0x92,0x02,0x05,0x14,0x17,0x93,0x02,0x06,0x13,0x17,0x9C,0x02,0x05,0x06,0x13,0x14,0x17,0x9D,0x02,0x04,0x05,0x14,0x15,0x17,0x9E,0x02,0x03,0x04,0x15,0x16,0x17,0x9F,0x03,0x16,0xA0,0xFF};
uint8_t notes9[] = {0x81,0x03,0x16,0x82,0x02,0x03,0x04,0x15,0x16,0x17,0x83,0x02,0x04,0x05,0x14,0x15,0x17,0x84,0x02,0x05,0x06,0x13,0x14,0x17,0x85,0x02,0x06,0x13,0x17,0x8E,0x02,0x17,0x8F,0x03,0x04,0x05,0x16,0x90,0x04,0x15,0x91,0x05,0x14,0x15,0x16,0x92,0x14,0x17,0x93,0x13,0x17,0x9C,0x06,0x13,0x14,0x17,0x9D,0x05,0x14,0x15,0x17,0x9E,0x04,0x15,0x16,0x17,0x9F,0x03,0x16,0xA0,0xFF};
uint16_t color1[] = {COLOR_GRADE4,COLOR_GRADE6,COLOR_GRADE5,COLOR_GRADE4,COLOR_GRADE4,COLOR_GRADE5,COLOR_GRADE6,COLOR_BLUE};
uint16_t color0[] = {COLOR_GRADE1,COLOR_GRADE1,COLOR_GRADE2,COLOR_GRADE3,COLOR_GRADE4,COLOR_GRADE5,COLOR_GRADE6,COLOR_BLUE};
uint16_t bg_color[] = {COLOR_GRADE1,COLOR_GRADE2,COLOR_GRADE3,COLOR_GRADE4,COLOR_GRADE5,COLOR_GRADE6};

const int16_t d_pm25[] = {0,35,75,115,150,250,500,INT16_MAX};
const int16_t d_angle[] = {225,180,135,90,45,0,-45,-46};

BOOL isPowOnJust = TRUE;
/*
Rect_t allscreen_pos =
{
	.x = 0,
	.y = 0,
	.w = 320,
	.h = 240,
};
*/
Rect_t num_pos_horizontal = 
{
	.x = 30,
	.y = 80,
	.w = 64,
	.h = 70,
};
Rect_t pm25_tag_pos_horizontal = 
{
	.x = 10,
	.y = 30,
	.w = 53,
	.h = 10,
};
Rect_t uint_tag_pos_horizontal = 
{
	.x = 250,
	.y = 27,
	.w = 55 ,
	.h = 17,
};
Rect_t line1_pos_horizontal = 
{
	.x = 0,
	.y = 48,
	.w = 320,
	.h = 1,
};
Rect_t logo_pos_horizontal = 
{
	.x = 105,
	.y = 20,
	.w = 106,
	.h = 22,
};
Rect_t line2_pos_horizontal = 
{
	.x = 0,
	.y = 182,
	.w = 320,
	.h = 1,
};
Rect_t bc_pos_horizontal = 
{
	.x = 0,
	.y = 184,
	.w = 320,
	.h = 60,
};
Rect_t airlevel_pos_horizontal = 
{
	.x = 100,
	.y = 193,
	.w = 120,
	.h = 30,
};

Rect_t num_clear_pos_horizontal = 
{
	.x = 98,
	.y = 100,
	.w = 120,
	.h = 39,
};
Rect_t airlevel_posclear_horizontal = 
{
	.x = 100,
	.y = 200,
	.w = 120,
	.h = 30,
};

Rect_t init_interface_horizontal = 
{
	.x = 27,
	.y = 60,
	.w = 266,
	.h = 55,
};
Rect_t init_czmb_horizontal = 
{
	.x = 88,
	.y = 163,
	.w = 152,
	.h = 23,
};


Rect_t num_pos_vertical = 
{
	.x = 10,
	.y = 127,
	.w = 56,
	.h = 70,
};
Rect_t pm25_tag_pos_vertical = 
{
	.x = 93,
	.y = 70,
	.w = 53,
	.h = 10,
};
Rect_t uint_tag_pos_vertical = 
{
	.x = 96,
	.y = 227,
	.w = 55 ,
	.h = 17,
};
Rect_t line1_pos_vertical = 
{
	.x = 0,
	.y = 48,
	.w = 240,
	.h = 1,
};
Rect_t logo_pos_vertical = 
{
	.x = 65,
	.y = 20,
	.w = 106,
	.h = 22,
};
Rect_t line2_pos_vertical = 
{
	.x = 0,
	.y = 258,
	.w = 240,
	.h = 1,
};
Rect_t bc_pos_vertical = 
{
	.x = 0,
	.y = 260,
	.w = 240,
	.h = 60,
};
Rect_t airlevel_pos_vertical = 
{
	.x = 60,
	.y = 270,
	.w = 120,
	.h = 30,
};
Rect_t num_clear_pos_vertical = 
{
	.x = 98,
	.y = 100,
	.w = 120,
	.h = 39,
};
Rect_t airlevel_posclear_vertical = 
{
	.x = 100,
	.y = 200,
	.w = 120,
	.h = 30,
};
Rect_t init_interface_vertical = 
{
	.x = 27,
	.y = 100,
	.w = 182,
	.h = 39,
};
Rect_t init_czmb_vertical = 
{
	.x = 55,
	.y = 170,
	.w = 131,
	.h = 20,
};

Rect_t *position_list_h[] = {&num_pos_horizontal,&pm25_tag_pos_horizontal,&uint_tag_pos_horizontal,&line1_pos_horizontal,&logo_pos_horizontal,&line2_pos_horizontal,&bc_pos_horizontal,&airlevel_pos_horizontal,&init_interface_horizontal,&init_czmb_horizontal,&num_clear_pos_horizontal,&airlevel_posclear_horizontal};
Rect_t *position_list_v[] = {&num_pos_vertical,&pm25_tag_pos_vertical,&uint_tag_pos_vertical,&line1_pos_vertical,&logo_pos_vertical,&line2_pos_vertical,&bc_pos_vertical,&airlevel_pos_vertical,&init_interface_vertical,&init_czmb_vertical,&num_clear_pos_vertical,&airlevel_posclear_vertical};

enum POSITION
{
	NUM,
	PM25_TAG,
	UINT_TAG,
	LINE1,
	LOGO,
	LINE2,
	BACKGROUND,
	AIRLEVEL,
	INIT_INTERFACE = 8,
	INIT_CZMB,
	NUM_CLEAR,
	AIRLEVEL_CLEAR,
//	PM25_TAG,
};

uint16_t g_bg_color = 0x0;

void lcd_enterSleep();
void lcd_exitSleep();

void lcd_setHigh(PORT_PIN_T PORT)
{
	HAL_GPIO_WritePin(PORT.GPIOx,PORT.GPIO_Pin,GPIO_PIN_SET);
}

void lcd_setLow(PORT_PIN_T PORT)
{
	HAL_GPIO_WritePin(PORT.GPIOx,PORT.GPIO_Pin,GPIO_PIN_RESET);
}	

void lcd_writeCommand(uint16_t dat)
{
	GPIOB->MODER = 0x55555555;
//	lcd_setHigh(RD);
	lcd_setLow(CS);
	lcd_setLow(RS);
	GPIOB->ODR = dat;
	lcd_setLow(WR);
	lcd_setHigh(WR);
//	lcd_setHigh(RD);
	lcd_setHigh(CS);
//	HAL_Delay(1);

}

void lcd_writeData(uint16_t dat)
{
//	__disable_irq();
//	GPIOB->MODER = 0x55555555;
//	lcd_setHigh(RD);
//	lcd_setHigh(RST);
//	lcd_setLow(CS);
	GPIOA->BRR |= GPIO_PIN_10;
//	lcd_setHigh(RS);
	GPIOA->BSRR |= GPIO_PIN_11;
	GPIOB->ODR = dat;
//	lcd_setLow(WR);
	GPIOA->BRR |= GPIO_PIN_12;
//	lcd_setHigh(WR);
	GPIOA->BSRR |= GPIO_PIN_12;
//	lcd_setHigh(RD);
//	lcd_setHigh(CS);
	GPIOA->BSRR |= GPIO_PIN_10;
//	__enable_irq();
}
uint16_t lcd_readData()
{
	uint16_t data;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOB->MODER = 0x00;
	lcd_setHigh(RD);
	lcd_setHigh(WR);
	lcd_setHigh(RST);
	lcd_setLow(CS);
	lcd_setHigh(RS);
	data = GPIOB->IDR;
	lcd_setLow(RD);
	lcd_setHigh(RD);
//	lcd_setHigh(RD);
	lcd_setHigh(CS);
	return data;
}

void lcd_reset()
{
	lcd_setHigh(RST);
//	HAL_Delay(1);
	__nop();
	lcd_setLow(RST);
	HAL_Delay(20);
	lcd_setHigh(RST);
	HAL_Delay(120);
}

void lcd_init()
{
//	lcd_reset();
//	lcd_writeCommand(0xCB); //AP[2:0] 
//	lcd_writeData(0x01); 
//-----------------------------------ST7789S reset sequence------------------------------------// 
		lcd_setHigh(RST);
	HAL_Delay(10);
	__nop();
	lcd_setLow(RST);
	HAL_Delay(100);
	lcd_setHigh(RST);
	HAL_Delay(520);
	lcd_setHigh(RD);
	
//---------------------------------------------------------------------------------------------------//
//	lcd_writeCommand(0x1);
	lcd_writeCommand(0x11);
	HAL_Delay(120);
//--------------------------------Display and color format setting----------------------------// 	
//	lcd_writeCommand(0x36);
//	lcd_writeData(0x00);
	lcd_writeCommand(0x3a);
	lcd_writeData(0x55);		//IF 0X06 WILL WRONG
//--------------------------------ST7789S Frame rate setting----------------------------------// 
	lcd_writeCommand(0xb2);
	lcd_writeData(0x0c);
	lcd_writeData(0x0c);
	lcd_writeData(0x00);
	lcd_writeData(0x33);
	lcd_writeData(0x33);
	lcd_writeCommand(0xb7);
	lcd_writeData(0x35);
//---------------------------------ST7789S Power setting--------------------------------------// 		
	lcd_writeCommand(0xbb); 
	lcd_writeData(0x35); 

	lcd_writeCommand(0xC0); 
	lcd_writeData(0x2c);  
	
	lcd_writeCommand(0xC2);     
	lcd_writeData(0x01);  
	
	lcd_writeCommand(0xC3);  
	lcd_writeData(0x11);
//	lcd_writeData(0x13);
	
	lcd_writeCommand(0xC4); 
	lcd_writeData(0x20); 	
	
//	lcd_writeCommand(0xC5);     //VCM control 
//	lcd_writeData(0x31); 			//0x31
//	lcd_writeData(0x3C); 			//0x3c

//	lcd_writeCommand(0xca);			//
//	lcd_writeData(0x0f);				//
	
//	lcd_writeCommand(0xc8);			//
//	lcd_writeData(0x08);				//
	
//	lcd_writeCommand(0x55);			//
//	lcd_writeData(0x90);				//
	
	lcd_writeCommand(0xd0); 
	lcd_writeData(0xa4);
	lcd_writeData(0xa1);

//	lcd_writeCommand(0x26);     //Gamma curve selected 
	//lcd_writeData(0x01); 



//		lcd_writeCommand(0x36); 
//	lcd_writeData(0x68); //0x48 //
//	lcd_writeData(0xc8); //0x48
//		lcd_writeData(0xc0); //0x48
//		lcd_writeData(0x0); //0x48
//		lcd_writeData(0xa0); //0x48
//		lcd_writeData(0x60); //0x48
//		lcd_writeData(0xe0); 
//		lcd_writeCommand(0x3A); 
//	lcd_writeData(0x55); 
//	lcd_writeCommand(0xC7); 
//	lcd_writeData(0xa8); 
//		lcd_writeCommand(0x35); 
//		lcd_writeCommand(0x38);
//--------------------------------ST7789S gamma setting---------------------------------------// 	

	lcd_writeCommand(0xE0);     //Set Gamma 
	lcd_writeData(0xd0); 
	lcd_writeData(0x00); 
	lcd_writeData(0x06); 
	lcd_writeData(0x09); 
	lcd_writeData(0x0b); 
	lcd_writeData(0x2a); 
	lcd_writeData(0x3c); 
	lcd_writeData(0x55); 
	lcd_writeData(0x4b); 
	lcd_writeData(0x08);
	lcd_writeData(0x16); 
	lcd_writeData(0x14); 
	lcd_writeData(0x19); 
	lcd_writeData(0x20); 

	lcd_writeCommand(0XE1);     //Set Gamma 
	lcd_writeData(0xd0); 
	lcd_writeData(0x00); 
	lcd_writeData(0x06); 
	lcd_writeData(0x09); 
	lcd_writeData(0x0b); 
	lcd_writeData(0x29); 
	lcd_writeData(0x36); 
	lcd_writeData(0x54); 
	lcd_writeData(0x4B); 
	lcd_writeData(0x0d); 
	lcd_writeData(0x16); 
	lcd_writeData(0x14); 
	lcd_writeData(0x21); 
	lcd_writeData(0x20); 
//	lcd_writeData(0x0F); 
//	lcd_writeCommand (0x2A);  
//	lcd_writeData(0x00); 
//	lcd_writeData(0x00); 
//	lcd_writeData(0x00); 
//	lcd_writeData(0xEF); 
//	lcd_writeCommand(0x2B);  
//	lcd_writeData(0x00); 
//	lcd_writeData(0x00); 
//	lcd_writeData(0x01); 
//	lcd_writeData(0x3F);
//	lcd_writeCommand (0x21); 
//	lcd_writeCommand (0x11);     //Exit Sleep 
	HAL_Delay(120); 
//	lcd_writeCommand(0x20);
	lcd_writeCommand(0x29); //display on 
//	lcd_writeData(0x2C); 

}

void lcd_enterSleep()
{
	lcd_writeCommand(0x00);
	lcd_writeCommand(0x10);
}

void lcd_exitSleep()
{
	lcd_writeCommand(0x11);
	HAL_Delay(120);
	lcd_writeCommand(0x29);
}

void lcd_turnOnDisplay()
{
	lcd_writeCommand(0x29);
}

void lcd_turnOffDisplay()
{
	lcd_writeCommand(0x28);
}

void lcd_setPos(uint16_t x,uint16_t y)
{
	uint16_t column,row;
	column = y;
	row = x;
	lcd_writeCommand(0x2A);
	lcd_writeData(column>>8);
	lcd_writeData(column&0x0f);
	lcd_writeData(column>>8);
	lcd_writeData(column&0x0f);
	lcd_writeCommand(0x2B);
		lcd_writeData(row>>8);
	lcd_writeData(row&0x0f);
	lcd_writeData(row>>8);
	lcd_writeData(row&0x0f);
}

void lcd_brushRed()
{
	uint16_t i,j;
	lcd_writeCommand (0x2A);  
	lcd_writeData(0x00); 
	lcd_writeData(0x00); 
	lcd_writeData(0x00); 
	lcd_writeData(0x55); 
	lcd_writeCommand(0x2B);  
	lcd_writeData(0x00); 
	lcd_writeData(0x00); 
	lcd_writeData(0x00); 
	lcd_writeData(0x3F); 
	lcd_writeCommand(0x2c);
	for(i=0;i<240;i++)
	{
		for(j=0;j<320;j++)
		{
			lcd_writeData(0xf800);
		}
	}
//	lcd_writeCommand(0x29);
	lcd_writeCommand(0x00);
}

void lcd_blackLightOn()
{
	lcd_setHigh(LCD_EN);
}
void lcd_blackLightOff()
{
	lcd_setLow(LCD_EN);
}	
void lcd_set_pos(uint16_t x,uint16_t y)
{
	uint16_t column,row;
	row = x;
//	column = XMAX - y -1;
	column = y;
	lcd_writeCommand(0x2A);
	lcd_writeData(column>>8);
	lcd_writeData(column);
	lcd_writeData(column>>8);
	lcd_writeData(column);
	lcd_writeCommand(0x2B);
	lcd_writeData(row>>8);
	lcd_writeData(row);
	lcd_writeData(row>>8);
	lcd_writeData(row);
	lcd_writeCommand(0x2c);
}
void lcd_set_showfield(uint16_t x,uint16_t y,uint16_t w ,uint16_t h)
{
		uint16_t column,row;
	row = x;
//	column = XMAX - y -1;
	column = y;
	lcd_writeCommand(0x2A);
//	lcd_writeData(y>>8);
//	lcd_writeData(y);
//	lcd_writeData((y+w-1)>>8);
//	lcd_writeData((y+w-1));
		lcd_writeData(x>>8);
	lcd_writeData(x);
	lcd_writeData((x+w-1)>>8);
	lcd_writeData(x+w-1);
	lcd_writeCommand(0x2B);
//	lcd_writeData(x>>8);
//	lcd_writeData(x);
//	lcd_writeData((x+h-1)>>8);
//	lcd_writeData(x+h-1);
		lcd_writeData(y>>8);
	lcd_writeData(y);
	lcd_writeData((y+h-1)>>8);
	lcd_writeData((y+h-1));
	lcd_writeCommand(0x2c);
}
	
void lcd_set_rectangle(Rect_t *pos)
{
	uint16_t column1,column2,row1,row2,i,j;
//	row1 = pos->x;
//	row2 = pos->x + pos->w;
//	column1 = XMAX - pos->y - pos->h -1;
//	column2 = XMAX - pos->y-1 ;
	row1 = pos->x;
	row2 = pos->x + pos->w;
	column1 =pos->y;
	column2 =pos->y+pos->h ;
	lcd_writeCommand(0x2A);
	lcd_writeData(column1>>8);
	lcd_writeData(column1);
	lcd_writeData(column2>>8);
	lcd_writeData(column2);
	lcd_writeCommand(0x2B);
	lcd_writeData(row1>>8);
	lcd_writeData(row1);
	lcd_writeData(row2>>8);
	lcd_writeData(row2);
	lcd_writeCommand(0x2c);
}	

void lcd_draw_rectangle(Rect_t *pos,uint16_t color)
{
	uint16_t column1,column2,row1,row2,i,j;
//	row1 = pos->x;
//	row2 = pos->x + pos->w;
//	column1 = XMAX - pos->y - pos->h;
//	column2 = XMAX - pos->y;
	row1 = pos->y;
	row2 = pos->y + pos->h;
	column1 =pos->x;
	column2 =pos->x+pos->w ;
	lcd_writeCommand(0x2A);
	lcd_writeData(column1>>8);
	lcd_writeData(column1);
	lcd_writeData(column2>>8);
	lcd_writeData(column2);
	lcd_writeCommand(0x2B);
	lcd_writeData(row1>>8);
	lcd_writeData(row1);
	lcd_writeData(row2>>8);
	lcd_writeData(row2);
	lcd_writeCommand(0x2c);
	for (i=0;i<=pos->h;i++)
	{
		for (j=0;j<=pos->w;j++)
		{
//				lcd_writeData(color >>8);
				lcd_writeData(color);
		}
	}

	
}

uint16_t lcd_color_set(uint16_t R,uint16_t G,uint16_t B)
{
	uint16_t color = (((R<<5)>>8)<<11) | (((G<<6)>>8)<<5) |((B<<5)>>8);
	
	return color;
}

void lcd_showmap(const uint8_t *map, Rect_t * position, int8_t scale,uint16_t bg_color,uint16_t fg_color)
{
	volatile uint16_t i, j;
	volatile uint16_t x = position->x, y = position->y;
	volatile uint16_t bitperline = ((((position->w -1)>>3)+1)<<3)>>scale;
	volatile uint16_t mapindex;
	volatile uint16_t mapmask;
	volatile BOOL paint;
	volatile uint16_t tmpbyte;
	lcd_set_showfield(x,y,position->w ,position->h);
	for (i=0;i<position->h;i++)
	{
		tmpbyte = 0;
		for (j=0;j<position->w;j++)
		{
//			lcd_set_pos(x+j,y+i);
			mapindex = (((i>>scale)*bitperline)+(j>>scale))>>(3);
			mapmask = 0x80>>((j>>scale)&0x07);
			if (map[mapindex]&mapmask)
			{
			//	paint = TRUE;
				tmpbyte =fg_color;
			}
			else
			{
			//	paint = FALSE;
				tmpbyte =bg_color;
			}
			
//			if (paint)
//			{
//				tmpbyte =fg_color;
//			}
//			else
//			{
//				tmpbyte =bg_color;
//			}
			
			lcd_writeData(tmpbyte);
		
		}
		
	}
	
}

void lcd_shownote( const uint8_t *note, Rect_t *position, float xscale_bit,float yscale_bit,uint16_t bg_color,uint16_t fg_color)
{
	uint16_t i,j,k,index=0;
	uint16_t *tmpbytes = (uint16_t *)malloc((position->w)*sizeof(uint16_t));
	uint8_t xtmp,ytmp;
	Rect_t show_rect = {.x = position->x,.y = position->y,.w = position->w,.h = 0};
//	memset(tmpbytes,bg_color,(position->w>>1)*sizeof(uint8_t));
//	memset(tmpbytes,g_bg_color,(position->w));
	for (i=0;i<position->w;i++)
	{
	tmpbytes[i] = bg_color;
	}
	for (i=0;i<position->h;i++)
	{
		while(!(*(note+index)&0x80))
		{
			index++;
		}
		if (i+position->y<0)
		{
			continue;
		}
		ytmp = round(((*(note+index)&0x7f))*yscale_bit);
		while (ytmp<0)
		{
			index++;
			if(((*(note+index)&0x80)))
			{
				if (round(((*(note+index)&0x7f))*yscale_bit)>=0)
				ytmp = i;
			}
		}
		if (ytmp<i)
		{
			i--;
		}
		show_rect.y = i+position->y;
//		lcd_set_rectangle(&show_rect);
		lcd_set_showfield(show_rect.x,show_rect.y,show_rect.w ,show_rect.h);
		if (i == ytmp)
		{
			BOOL paint = FALSE;
			index++;
			for (j=0; j<(position->w); j++)
			{
			//	lcd_set_pos(j+position->x,i+position->y);
				uint16_t tbyte = 0;

				{
					if(!((*(note+index)&0x80)))
					{
						xtmp = round((*(note+index))*xscale_bit);
						while (xtmp<0)
						{
							index++;
							paint = !paint;
							if ((*(note+index+1)&0x80))
							{
								break;
							}
							xtmp = round((*(note+index))*xscale_bit);
						}
						if (j==xtmp)
						{
							if (!(*(note+index+1)&0x80)&&(xtmp == round((*(note+index+1))*xscale_bit)))
							{
								index+=2;
							}
							else
							{
								paint = !paint;
								index++;
							}
						}
					}
					if (paint)
					{
						tbyte =fg_color;
					}
					
					else
					{
						tbyte =bg_color;
					}
				}
				tmpbytes[j] = tbyte;
				lcd_writeData(tbyte);
			}
		}
		else
		{
			for (j=0; j<(position->w); j++)
			{
				lcd_writeData(tmpbytes[j]);
			}
		}
	}
	free(tmpbytes);
}
void oled_show_num(uint32_t num, BOOL leftJust, float xscale, float yscale, uint8_t width, uint8_t dot_position, Rect_t const * position,uint16_t bg_color,uint16_t fg_color,BOOL dir)
{
	uint8_t nbit[5] = {0};
	uint8_t i;
	uint8_t dot[] = {63,192,127,224,255,240,255,240,255,240,255,240,255,240,255,240,127,224,63,192};
	uint8_t tmp[180] = {0};
	BOOL firstzero = TRUE;
	uint32_t divnum = 10000;
	uint8_t currentx = 0;
	uint8_t notesnull[] = {0xff};
//	 uint8_t* notes[] = {notes0,notes1,notes2,notes3,notes4,notes5,notes6,notes7,notes8,notes9};
	const uint8_t *nums[] = {num0,num1,num2,num3,num4,num5,num6,num7,num8,num9};
	Rect_t tops = {.x = position->x, .y = position->y, .w = position->w, .h = position->h};
	width = min(width, sizeof(nbit)/sizeof(uint8_t));

	for (i=0; i<sizeof(nbit)/sizeof(uint8_t); i++)
	{
		nbit[i] = (num/divnum)%10;
		divnum /=10;
	}
	for (i = 0;i<width; i++)
	{
//		tops.x = position->x +currentx+4;
		tops.x = position->x +currentx+20;
		if (i == dot_position)
		{
			firstzero = FALSE;
		}
		if (!firstzero||nbit[i]!=0)
		{
//			lcd_showmap( nums[nbit[i]],&tops,xscale,0x00,fg_color);
			lcd_shownote(nums[nbit[i]], &tops, xscale,yscale,0x00,fg_color);
			firstzero = FALSE;
//			currentx += tops.w+4;
			currentx += tops.w+4;
		}
		else if(!leftJust)
		{
			currentx += tops.w-4;
		}
		if (i == dot_position && dot_position <2)
		{
			Rect_t dot_pos = {.w = 12,.h = 10,.y=position->y+53};
			if(dir)
			{
				dot_pos.y = position->y+42;
			}
			dot_pos.x = tops.x + tops.w + 4;
			currentx += 8+4+7;
//			lcd_draw_rectangle(&dot_pos,fg_color);
			lcd_showmap(dot, &dot_pos, 0,0x00,fg_color);
		}
	}
	if (firstzero)
	{

		lcd_showmap( nums[0],&tops,0,0x00,fg_color);
	}
}
void lcd_clear_showmap( uint8_t *map, uint8_t *tmp, Rect_t * position, int8_t scale,uint16_t bg_color,uint16_t fg_color)
{
	//oled_clear(position);
//	lcd_getdataFromPgm(tmp,map);
	lcd_showmap(map, position, scale,bg_color,fg_color);
}
	

void lcd_update_pm25(uint32_t num,_Bool dd,uint8_t dir)
{
	uint8_t tmp[180] = {0};
	uint8_t i;
	uint8_t held =0;
	uint8_t dot_position = 0;
	static  uint8_t static_airLevel = 0,last_dot_position=0;
	static  uint8_t static_byteNum=0;
	uint8_t lcdFlush=0;
	float xscale = 1;
	float yscale = 1;
	uint8_t currentLevel;
	uint16_t color[] = {COLOR_GRADE1,COLOR_GRADE2,COLOR_GRADE3,COLOR_GRADE4,COLOR_GRADE5,COLOR_GRADE6};
	uint32_t colorheld[] = {3500,7500,11500,15000,25000,UINT32_MAX};
	int32_t lenthheld[] = {10000,1000,100,10,0};
	uint32_t num_show = num;
	const uint8_t *LEVEL_PIC[] = {excellent,good,lightpollution,moderatelypolluted,serverpolluted,seriouspollution};
	Rect_t num_pos  = 
	{	
		.x = 30,
		.y = 80,
		.w = 72,
		.h = 64,
	};
	Rect_t airlevel_pos = 
	{
		.x = 100,
		.y = 193,
		.w = 30,
		.h = 30,
	};
	Rect_t airlevel_posclear = 
	{	
		.x = 100,
		.y = 193,
		.w = 120,
		.h = 30,
	};	
	Rect_t num_clear_pos =
	{
		.x = 30,
		.y = 80,
		.w = 265,
		.h = 64,
	};
	Rect_t bc_pos = 
	{
		.x = 0,
		.y = 184,
		.w = 320,
		.h = 60,
	};
	for(i=0;i<5;i++)
	{
		if(num>=lenthheld[i])
		{
			held = i;
			if(held == 0)
			{
				dot_position = 2;
			}
			else if(held == 1)
			{
				dot_position = 1;
			}
			else
			{
				dot_position = 0;
			}
			break;
		}
			
	}
	if(last_dot_position != dot_position)
	{
		lcdFlush = TRUE;
		last_dot_position = dot_position;
	}
	for(i=0;i<held;i++)
	{

		num_show = num_show*10;
	}
	for(i = 3;i<=held;i++)
	{
		num_show = num_show/10;
	}
	for(i=0;i<6;i++)
	{
		if(num < colorheld[i])
			break;
	}
	currentLevel = i;
	if(dir%2)
	{
		xscale = 0.8;
		yscale = 0.8;
		num_pos.x = 5;
		num_pos.y = 125;
		num_pos.w = 56;
		num_pos.h = 52;
		
		airlevel_pos.x = 60;
		airlevel_pos.y = 270;
		airlevel_pos.w = 30;
		
		airlevel_posclear.x = 60;
		airlevel_posclear.y = 270;
		airlevel_posclear.w = 200;
		
		num_clear_pos.x = 5;
		num_clear_pos.y = 125;
		num_clear_pos.w = 235;
		
		bc_pos.y = 260;
	}
	else
	{

	}
	if(currentLevel != static_airLevel || isPowOnJust || dd)
	{
//		lcdFlush = TRUE;
		isPowOnJust = FALSE;
		static_airLevel = currentLevel;
		if(currentLevel <2)
		{
			airlevel_pos.x += 45;
			airlevel_pos.w = 30;
		}
		else
		{
			airlevel_pos.w = 120;
		}
		lcd_draw_rectangle(&bc_pos,color[i]);
		lcd_showmap(LEVEL_PIC[i],&airlevel_pos,0,color[i],0xffff);
	}

	if(lcdFlush)
	{
		lcd_draw_rectangle(&num_clear_pos,0x00);
	}
	oled_show_num(num_show,FALSE,xscale,yscale,3,dot_position,&num_pos,0xffff,0xffff,dir%2);
}




void lcd_clear_all(uint8_t dir)
{
	
	Rect_t allscreen_pos =
	{
		.x = 0,
		.y = 0,
		.w = 320,
		.h = 320,
	};
	if(dir%2)
	{
		allscreen_pos.w = 240;
		allscreen_pos.h = 320;
	}
	lcd_draw_rectangle(&allscreen_pos,0x00);
}
void lcd_clear()
{
	
}



void lcd_showTest()
{
//	Rect_t *positionList;// = (Rect_t *)malloc(sizeof(Rect_t));
//	positionList = *position_list_v;//[0];
//	uint16_t color[] = {0xef62,0xe6e0,0xe720,0xf7a1,0xffe7,0xffe0};
	uint16_t color[] = {COLOR_GRADE1,COLOR_GRADE2,COLOR_GRADE3,COLOR_GRADE4,COLOR_GRADE5,COLOR_GRADE6};
	const uint8_t *LEVEL_PIC[] = {excellent,good,lightpollution,moderatelypolluted,serverpolluted,seriouspollution};
	Rect_t num_pos = 
	{
		.x = 0,
		.y = 0,
		.w = 320,
		.h = 40,
	};
	Rect_t airlevel_pos = 
	{
		.x = 100,
		.y = 5,
		.w = 30,
		.h = 30,
	};
	lcd_clear_all(0);
	
	for(uint8_t i =0;i<6;i++)
	{
		lcd_draw_rectangle(&num_pos,color[i]);
		if(i <2)
		{
			airlevel_pos.x = 145;
			airlevel_pos.w = 30;
		}
		else
		{
			airlevel_pos.x = 100;
			airlevel_pos.w = 120;
		}
		lcd_showmap(LEVEL_PIC[i],&airlevel_pos,0,color[i],0xffff);
		num_pos.y += 40;
		airlevel_pos.y += 40;
	}
	

}
uint8_t lcd_getAirLevel(uint32_t pm)
{
	return 0;
}
void lcd_setDir(uint8_t d)
{
	uint8_t dirList[] = {0x60,0xc0,0xa0,0x00};
	lcd_writeCommand(0x36); 
	lcd_writeData(dirList[d]); 
}
void lcd_showStaticImage(uint8_t isPowOnJust,uint8_t dir)
{
	static uint8_t lastDir = 3;
	Rect_t *positionList;// = (Rect_t *)malloc(sizeof(Rect_t)*12);
	if(dir != lastDir)
	{
		lastDir = dir;
		/* clear the screen */
		
		lcd_setDir(dir);
	}
	if(dir%2)
	{
		positionList = *position_list_v;//[0];
	}
	else
	{
		positionList = *position_list_h;//[0];
	}
	
	if(isPowOnJust)
	{
		/*
//		lcd_showmap(hwlt,&positionList[8],0,0,0x057f);
		lcd_showmap(hwlt,positionList+8,0,0x00,0xffff);
		lcd_showmap(czmb,&positionList[INIT_CZMB],0,0,0x057f);
	}
	else
	{
	//	positionList2 = &pm25_tag_pos_horizontal;
		lcd_showmap(pm25_tag,&positionList[PM25_TAG],0,0x00,0xffff);
		lcd_showmap(logo_tag,&positionList[LOGO],0,0x00,COLOR_BLUE);
		lcd_showmap(uint_tag,&positionList[UINT_TAG],0,0x00,0xffff);
//		oled_show_num(12845,FALSE,1,1,3,3,&num_pos,0x0,0xf96a);
		lcd_draw_rectangle(&positionList[LINE1],0xffff);
		lcd_draw_rectangle(&positionList[LINE2],0xffff);
		lcd_draw_rectangle(&positionList[BACKGROUND],0xf96a);
		lcd_showmap(moderatelypolluted,&positionList[AIRLEVEL],0,0xf96a,0xffff);
		lcd_showmap(czmb,position_list_h[INIT_CZMB],0,0,0x057f);
		*/
			if(dir%2)
			{
				lcd_showmap(hwlt_v,position_list_v[INIT_INTERFACE],0,0x00,0xffff);
				lcd_showmap(czmb_v,position_list_v[INIT_CZMB],0,0,0x057f);
			}
			else
			{
				lcd_showmap(hwlt,position_list_h[INIT_INTERFACE],0,0x00,0xffff);
				lcd_showmap(czmb,position_list_h[INIT_CZMB],0,0,0x057f);
			}
	}
	else
	{
		if(dir%2)
			{
				lcd_showmap(pm25_tag,position_list_v[PM25_TAG],0,0x00,0xffff);
				lcd_showmap(logo_tag,position_list_v[LOGO],0,0x00,COLOR_BLUE);
				lcd_showmap(uint_tag,position_list_v[UINT_TAG],0,0x00,0xffff);
		//		oled_show_num(12845,FALSE,1,1,3,3,&num_pos,0x0,0xf96a);
				lcd_draw_rectangle(position_list_v[LINE1],0xffff);
				lcd_draw_rectangle(position_list_v[LINE2],0xffff);
//				lcd_draw_rectangle(position_list_v[BACKGROUND],0xf96a);
//				lcd_showmap(moderatelypolluted,position_list_v[AIRLEVEL],0,0xf96a,0xffff);
			}
			else
			{
				lcd_showmap(pm25_tag,position_list_h[PM25_TAG],0,0x00,0xffff);
				lcd_showmap(logo_tag,position_list_h[LOGO],0,0x00,COLOR_BLUE);
				lcd_showmap(uint_tag,position_list_h[UINT_TAG],0,0x00,0xffff);
		//		oled_show_num(12845,FALSE,1,1,3,3,&num_pos,0x0,0xf96a);
				lcd_draw_rectangle(position_list_h[LINE1],0xffff);
				lcd_draw_rectangle(position_list_h[LINE2],0xffff);
				lcd_draw_rectangle(position_list_h[BACKGROUND],0x00);
				lcd_showmap(moderatelypolluted,position_list_h[AIRLEVEL],0,0x00,0x00);
			}
	}
}
