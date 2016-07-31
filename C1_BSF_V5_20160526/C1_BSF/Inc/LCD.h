#ifndef LCD_H_
#define LCD_H_

#include "stm32f0xx_hal.h"
#include "typedef.h"
typedef struct Rect
{
	int16_t x,y,w,h;
} Rect_t;

typedef struct Point
{
	int16_t x,y;
}Point_t;

#define BACKGROUND_COLOR 0x00
#define TITLE_COLOR 0x6786
#define LINE_COLOR 0xffff
#define CLOCK_COLOR	0xc683
#define LEVEL_EXCELLENT_COLOR	0x6786
#define LEVEL_GOOD_COLOR		0xf786
#define LEVEL_LIGHTPOLLTED_COLOR	0xfc05
#define LEVEL_MODERATELYPOLLUTED	0xa2f8
#define LEVEL_SERVERPOLLUTED  0xe27f
#define LEVEL_SERIOUSPOLLUTED 0xcd2a

void lcd_blackLightOn(void);
void lcd_blackLightOff(void);
void lcd_init(void);
void lcd_brushRed(void);
void lcd_reset(void);
void lcd_update_pm25(uint32_t num,_Bool dd,uint8_t dir);
void lcd_show_image(const uint8_t *map, Rect_t * position);
void lcd_showmap(const uint8_t *map, Rect_t * position, int8_t scale,uint16_t bg_color,uint16_t fg_color);
void lcd_show_init(uint8_t num);
void showdial(const uint8_t *map, Rect_t * position, int8_t scale,uint16_t bg_color);
void showHalfdial(const uint8_t *map, Rect_t * position, int8_t dir,uint16_t bg_color,uint16_t *color);
void lcd_clear_all(uint8_t dir);
void lcd_show_rtData(uint32_t num);
void lcd_light_all(void);
void lcd_showInitInterFace(uint8_t stat);
void lcd_rtInterFace_init(void);
void lcd_showQrcode(void);
void lcd_clear_image(void);
void lcd_showTest(void);
void lcd_setDir(uint8_t d);
void lcd_showStaticImage(uint8_t isPowOnJust,uint8_t dir);
	
#endif

