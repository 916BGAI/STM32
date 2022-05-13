//******************************************************************************/
//-------------------------------------------------------------------------------
//              GND    电源地
//              VCC  接5V或3.3v电源
//              SDA  接PB7
//              SCL  接PB6
//-------------------------------------------------------------------------------
//******************************************************************************/

#ifndef __OLED_H
#define __OLED_H

#include "stdlib.h"
#include "main.h"
#include "gpio.h"
#include "u8g2.h"
#include "u8x8.h"

#define OLED_ADDRESS 0x78

uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
void u8g2Init(u8g2_t *u8g2);
void draw(u8g2_t *u8g2);
#endif
