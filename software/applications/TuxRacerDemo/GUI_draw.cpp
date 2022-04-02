/*
 * @file      GUI_draw.c
 * 
 * @author    FDU-EGA
 * 
 * @version   V1.0
 * 
 * @date      2022-3-27
 * 
 * @brief     用于图形绘制的一些自定义函数
 * 
 */

#include "GUI_draw.h"

extern TFT_eSPI tft;       // Invoke custom library
extern TFT_eSprite clk;

void draw_penguinL(uint8_t x, uint8_t y) //绘制朝左企鹅
{
  TJpgDec.drawJpg( x-22, y-30, penguinL_60, sizeof(penguinL_60));//在左上角的x,y处绘制图像——在这个草图中，DMA请求在回调tft_output()中处理
}

void draw_penguinR(uint8_t x, uint8_t y) //绘制朝右企鹅
{
  TJpgDec.drawJpg( x-22, y-30, penguinR_60, sizeof(penguinR_60));
}

void draw_star(uint8_t x, uint8_t y) //绘制星星图案
{
  clk.fillTriangle(x, y-11, x-7, y+11, x+4, y+2, COLOR_STAR);
  clk.fillTriangle(x-11, y-3, x-2, y-3, x-4, y+2, COLOR_STAR);
  clk.fillTriangle(x+2, y-3, x+10, y-3, x+4, y+2, COLOR_STAR);
  clk.fillTriangle(x, y+5, x+4, y+2, x+7, y+11, COLOR_STAR);
}

void draw_stone(uint8_t x, uint8_t y) //绘制障碍物石头
{
  clk.fillTriangle(x, y-16, x-14, y+16, x+14, y+16, COLOR_ICE);
}

void draw_tree(uint8_t x, uint8_t y) //绘制障碍物树
{
  clk.fillTriangle(x, y-20, x-17, y, x+17, y, COLOR_TREE);
  clk.fillTriangle(x, y-5, x-24, y+22, x+24, y+22, COLOR_TREE);
  clk.fillRect(x-2,y+23,4,10,COLOR_TREEROOT);
}

/*
 * @brief 实现图形平移动画，放在循环里，实现每帧画面移动一次的效果
 * @param dir：传入需改变的参数地址
 * @param start: 起始位置
 * @param stop：终点位置
 * @param speed：每帧画面移动大小，即移动速度
 * @retval 无
 * 
 * @note 函数存在bug，勿调用
 */
void element_move(uint16_t* dir, uint16_t start, uint16_t stop, uint16_t speed)   
{
  if(abs(stop - *dir) > speed){
    if(start < stop){
      (*dir) += speed;
    }else{
      (*dir) -= speed;
    }  
  }
}
