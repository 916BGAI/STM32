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

#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "spi.h"
#include "u8g2.h"
#include "u8x8.h"
uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,void *arg_ptr)
{
    switch (msg)
    {
        case U8X8_MSG_BYTE_SEND: /*ͨ��SPI����arg_int���ֽ�����*/
			HAL_SPI_Transmit(&hspi1,(uint8_t *)arg_ptr,arg_int,200);
            break;
        case U8X8_MSG_BYTE_INIT: /*��ʼ������*/
            break;
        case U8X8_MSG_BYTE_SET_DC: /*����DC����,�������͵������ݻ�������*/
			HAL_GPIO_WritePin(DC_GPIO_Port,DC_Pin,arg_int);
            break;
        case U8X8_MSG_BYTE_START_TRANSFER: 
            break;
        case U8X8_MSG_BYTE_END_TRANSFER: 
            break;
        default:
            return 0;
    }
    return 1;
}

uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8,
    U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
    U8X8_UNUSED void *arg_ptr) 
{
    switch (msg)
    {
        case U8X8_MSG_GPIO_AND_DELAY_INIT: /*delay��GPIO�ĳ�ʼ������main���Ѿ���ʼ�������*/
            break;
        case U8X8_MSG_DELAY_MILLI: /*��ʱ����*/
            HAL_Delay(arg_int);
            break;
        case U8X8_MSG_GPIO_CS: /*Ƭѡ�ź�*/
            break;
        case U8X8_MSG_GPIO_DC: /*����DC����,�������͵������ݻ�������*/
            HAL_GPIO_WritePin(DC_GPIO_Port,DC_Pin,arg_int);
            break;
        case U8X8_MSG_GPIO_RESET:
            break;
    }
    return 1;
}

void u8g2Init(u8g2_t *u8g2)
{
	/*U8G2_R0 :��Ļ��ת/����
	U8G2_R0
	U8G2_R1
	U8G2_R2
	U8G2_R3
	U8G2_MIRROR   u8g2_Setup_sh1106_128x64_noname_1
	*/
	u8g2_Setup_sh1106_128x64_noname_2(u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);  // ��ʼ�� u8g2 �ṹ��
	u8g2_InitDisplay(u8g2); 
	u8g2_SetPowerSave(u8g2, 0); //������ʾ
}

/*�ٷ��ṩ��Logo����demo*/
void draw(u8g2_t *u8g2)
{
    u8g2_SetFontMode(u8g2, 1); /*����ģʽѡ��*/
    u8g2_SetFontDirection(u8g2, 0); /*���巽��ѡ��*/
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf); /*�ֿ�ѡ��*/
    u8g2_DrawStr(u8g2, 0, 20, "U");
    
    u8g2_SetFontDirection(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_inb30_mn);
    u8g2_DrawStr(u8g2, 21,8,"8");
        
    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 51,30,"g");
    u8g2_DrawStr(u8g2, 67,30,"\xb2");
    
    u8g2_DrawHLine(u8g2, 2, 35, 47);
    u8g2_DrawHLine(u8g2, 3, 36, 47);
    u8g2_DrawVLine(u8g2, 45, 32, 12);
    u8g2_DrawVLine(u8g2, 46, 33, 12);
  
    u8g2_SetFont(u8g2, u8g2_font_4x6_tr);
    u8g2_DrawStr(u8g2, 1,54,"github.com/olikraus/u8g2");
}









