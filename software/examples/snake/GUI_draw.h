/*
 * @file      GUI_draw.h
 * 
 * @author    FDU-EGA
 * 
 * @version   V1.0
 * 
 * @date      2022-3-27
 * 
 * @brief     用于图形绘制的一些自定义函数，声明头文件
 * 
 */

#ifndef GUI_DRAW_H_
#define GUI_DRAW_H_

#include <Arduino.h>
#include <TFT_eSPI.h> 
#include <TJpg_Decoder.h>
#include <SPI.h>


/**********颜色值宏定义************/
#define TFT_GREY        0x5AEB
#define COLOR_ICE       0xb63b
#define COLOR_TREE      0x2444
#define COLOR_TREEROOT  0x5345
#define COLOR_STAR      0xfea0

/************图形绘制函数声明******************/
void draw_penguinL(uint8_t x, uint8_t y); //绘制朝左企鹅
void draw_penguinR(uint8_t x, uint8_t y); //绘制朝右企鹅
void draw_star(uint8_t x, uint8_t y); //绘制星星图案
void draw_stone(uint8_t x, uint8_t y); //绘制石头障碍物
void draw_tree(uint8_t x ,uint8_t y); //绘制障碍物树

void element_move(uint16_t* dir, uint16_t start, uint16_t stop, uint16_t speed);  //线性移动函数


#endif/* GUI_DRAW_H_ */
