#ifndef PAGE_H_
#define PAGE_H_

#include <Arduino.h>
#include <TFT_eSPI.h> 
#include <TJpg_Decoder.h>
#include <SPI.h>

#include "img/penguin.h"
#include "font/ZdyLwFont_20.h"

/**********颜色值宏定义************/
#define TFT_GREY        0x5AEB
#define COLOR_ICE       0xb63b
#define COLOR_TREE      0x2444
#define COLOR_TREEROOT  0x5345
#define COLOR_STAR      0xfea0

/*******页面状态传递宏定义*****/
#define ENTER_PAGE      0
#define GAME_SELECT     1   
#define SETTING         2

#define VOLUME_SELECT   3 
#define VOLUME_UP       5   
#define VOLUME_DOWN     6   

#define LIGHT_SELECT    4
#define LIGHT_UP        7
#define LIGHT_DOWN      8

#define GAME_1          9
#define GAME_2          10

#define LMT             5   //限制最大值

#define LIMIT_PLUS(n)    (n >= LMT) ? (LMT) : (++n)
#define LIMIT_SUB(n)    (n <= 0) ? (0) : (--n)

void home_page(uint8_t c); //首页
void setting_page(uint8_t c, uint8_t vol, uint8_t light);  //设置页面
void game_select_page(uint8_t c); //游戏选择页面
int page_run(uint8_t key);  //页面调度状态机运行
void page_game_quit(char game); //游戏退出



#endif /* PAGE_H_ */