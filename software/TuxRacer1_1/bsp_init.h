/*
 * 描述：板载外设功能初始化
 * 
 * 
 */

#ifndef _BSP_INIT_H_
#define _BSP_INIT_H_

#include "Arduino.h"
#include <TFT_eSPI.h> 
#include <TJpg_Decoder.h>

//#define USE_DMA

//各个管脚接口宏定义：
#define KEY_F 14
#define KEY_G 25
#define KEY_L 32
#define KEY_R 33

#define KEY_A 35
#define KEY_B 34
#define KEY_C 19
#define KEY_D 21

#define BEEP 15  //蜂鸣器
#define BL   22  //背光

//屏幕方向与按键方向不一致，需要重新定义
#define FORWARD   'L'
#define BACKWARD  'R'
#define LEFT      'F'
#define RIGHT     'G'

void KEY_Init();  //按键管脚初始化
void BEEP_Init(); //蜂鸣器初始化
void tft_DMA_Init(); //TFT屏幕DMA刷新初始化
void volume_set(uint8_t v);  //音量设置，最大值：5*20 = 100
void backlight_set(uint8_t bl);  //背光亮度设置，最大值：5*20 = 100

#endif/* _BSP_INIT_H_ */
