#ifndef _GAME_H_
#define _GAME_H_

#include "Arduino.h"
#include <TFT_eSPI.h> 
#include <TJpg_Decoder.h>
#include "GUI_draw.h"

#define GAME_START 0
#define GAME_RUN   1
#define GAME_END   2

typedef struct {
    uint8_t axis_x;  //横坐标
    uint8_t axis_y;  //纵坐标
    bool display;    //显示标志
}element_t; //元素位置信息结构体


void game_start();    //游戏开始

char game_run(char key);      //游戏运行

char game_stop(char key);     //游戏结束


#endif/* _GAME_H_ */