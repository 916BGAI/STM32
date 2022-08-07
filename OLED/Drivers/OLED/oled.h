#ifndef __OLED_H
#define __OLED_H	
//              ----------------------------------------------------------------
//              GND    ��Դ��
//              VCC  ��5V��3.3v��Դ
//              D0   ��PA5��SCL��
//              D1   ��PA7��SDA��
//              RES  ��PA4
//              DC   ��PA3
//              CS   ��PA2               
//              ----------------------------------------------------------------
//******************************************************************************/
#include "stdlib.h"	  
#include "main.h"
#include "gpio.h"
#include "u8g2.h"
#include "u8x8.h"

#define CS_Pin GPIO_PIN_2
#define CS_GPIO_Port GPIOA
#define RES_Pin GPIO_PIN_4
#define RES_GPIO_Port GPIOA
#define DC_Pin GPIO_PIN_3
#define DC_GPIO_Port GPIOA
//OLEDģʽ����
//0:4�ߴ���ģʽ
//1:����8080ģʽ
#define OLED_MODE 0
#define SIZE 16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    
//-----------------����LED�˿ڶ���---------------- 
#define LED1_ON HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET)//TEST_LED1
#define LED1_OFF HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET)
#define LED2_ON HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET)//TEST_LED2
#define LED2_OFF HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET)

//-----------------OLED�˿ڶ���----------------  					   
#define OLED_RST_Clr() HAL_GPIO_WritePin(RES_GPIO_Port,RES_Pin,GPIO_PIN_RESET)//RES
#define OLED_RST_Set() HAL_GPIO_WritePin(RES_GPIO_Port,RES_Pin,GPIO_PIN_SET)

//OLED�����ú���
uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,void *arg_ptr);
uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8,U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,U8X8_UNUSED void *arg_ptr) ;
void u8g2Init(u8g2_t *u8g2);
void draw(u8g2_t *u8g2);
#endif  
	 



