
#ifndef __OLED_H
#define __OLED_H			  	 

#include "stdlib.h"	 
#include <stdint.h>


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#ifdef __cplusplus
extern "C"
{ 
#endif
#define OLED_MODE 0
#define SIZE 16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    

#define ESPIDF 1
#define STMKEIL !ESPIDF

#if ESPIDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define delay_ms(ms)   vTaskDelay(pdMS_TO_TICKS(ms))

#include "pins.h"
#define OLED_DC_PIN OLED_DC_Pin
#define OLED_CLK_PIN  OLED_DO_PIN
#define OLED_DIN_PIN OLED_DI_PIN
#define OLED_RES_PIN OLED_RES_Pin
#define OLED_CS_PIN OLED_CS_Pin

#define LED_ON  gpio_set_level(OLED_DC_PIN,0)//DC
#define LED_OFF gpio_set_level(OLED_DC_PIN,1)

			
#define OLED_SCLK_Clr() gpio_set_level(OLED_CLK_PIN,0)//CLK
#define OLED_SCLK_Set() gpio_set_level(OLED_CLK_PIN,1)

#define OLED_SDIN_Clr() gpio_set_level(OLED_DIN_PIN,0)//DIN
#define OLED_SDIN_Set() gpio_set_level(OLED_DIN_PIN,1)

#define OLED_RST_Clr() gpio_set_level(OLED_RES_PIN,0)//RES
#define OLED_RST_Set() gpio_set_level(OLED_RES_PIN,1)

#define OLED_DC_Clr() gpio_set_level(OLED_DC_PIN,0)//DC
#define OLED_DC_Set() gpio_set_level(OLED_DC_PIN,1)
 		     
#define OLED_CS_Clr()  gpio_set_level(OLED_CS_PIN,0)//CS
#define OLED_CS_Set()  gpio_set_level(OLED_CS_PIN,1)
#endif

#if STMKEIL
#include "sys.h"
#define LED_ON GPIO_ResetBits(GPIOB,GPIO_Pin_8)//DC
#define LED_OFF GPIO_SetBits(GPIOB,GPIO_Pin_8)

			
#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_5)//CLK
#define OLED_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_5)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_7)//DIN
#define OLED_SDIN_Set() GPIO_SetBits(GPIOA,GPIO_Pin_7)

#define OLED_RST_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_0)//RES
#define OLED_RST_Set() GPIO_SetBits(GPIOB,GPIO_Pin_0)

#define OLED_DC_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_1)//DC
#define OLED_DC_Set() GPIO_SetBits(GPIOB,GPIO_Pin_1)
 		     
#define OLED_CS_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_4)//CS
#define OLED_CS_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_4)
#endif

#define OLED_CMD  0	
#define OLED_DATA 1	


void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y, u8 *p);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
#ifdef __cplusplus
}
#endif
#endif  
	 


