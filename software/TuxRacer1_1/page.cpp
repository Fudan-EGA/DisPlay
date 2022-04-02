#include "page.h"

extern TFT_eSPI tft;       // Invoke custom library
extern TFT_eSprite clk;

const String set_level[]={"0", "1", "2", "3", "4", "5"};

void home_page(uint8_t c) //首页
{
    clk.setColorDepth(8);
    clk.createSprite(240, 240);
    clk.fillSprite(TFT_WHITE);

    if(c == GAME_SELECT){
        clk.drawRect(75, 114, 95, 30, 0xb63b);
    } 
    if(c == SETTING){
        clk.drawRect(95, 164, 50, 30, 0xb63b);
    } 
    
    clk.setTextColor(TFT_BLACK);
    clk.drawCentreString("DisPlay", 120, 40, 4); // Draw text centre at position 120, 0 using font 4

    clk.loadFont(ZdyLwFont_20); //加载font/ZdyLwFont_20字体
    clk.setTextDatum(CC_DATUM);
    clk.setTextColor(TFT_BLACK, TFT_WHITE); 
    clk.drawString("选择游戏", 120,130);
    clk.drawString("设置", 120,180);
    clk.setTextColor(TFT_GREY, TFT_WHITE); 
    clk.drawString("D键: 选择", 60,225);
    clk.drawString("C键: 返回", 180,225);

    clk.unloadFont(); //释放加载字体资源
    clk.pushSprite(0,0);
    clk.deleteSprite();
}

void setting_page(uint8_t c, uint8_t vol, uint8_t light)  //设置页面
{
    clk.setColorDepth(8);
    clk.createSprite(240, 240);
    clk.fillSprite(TFT_WHITE);
    clk.loadFont(ZdyLwFont_20); //加载font/ZdyLwFont_20字体
    clk.setTextDatum(CC_DATUM);
    clk.setTextColor(TFT_BLACK, TFT_WHITE); 

    clk.drawString(set_level[vol], 172,100);
    clk.drawString(set_level[light], 172,140);

    if(c == VOLUME_SELECT){
        clk.drawRect(45, 84, 50, 30, 0xb63b);
    } 
    if(c == LIGHT_SELECT){
        clk.drawRect(45, 124, 50, 30, 0xb63b);
    } 

    if(c == VOLUME_UP){
        clk.drawRect(185, 85, 30, 30, 0xb63b);
    } 
    if(c == VOLUME_DOWN){
        clk.drawRect(135, 85, 30, 30, 0xb63b);
    } 

    if(c == LIGHT_UP){
        clk.drawRect(185, 125, 30, 30, 0xb63b);
    } 
    if(c == LIGHT_DOWN){
        clk.drawRect(135, 125, 30, 30, 0xb63b);
    } 
    
    clk.setTextColor(TFT_BLACK);
    clk.drawString("设置", 120, 40); // Draw text centre at position 120, 0 using font 4

    clk.drawString("音量", 70, 100);clk.drawString("-", 150, 100);clk.drawString("+", 200, 100);
    clk.drawString("亮度", 70, 140);clk.drawString("-", 150, 140);clk.drawString("+", 200, 140);
    clk.setTextColor(TFT_GREY, TFT_WHITE);

    clk.drawString("左键: 减小", 60,200);clk.drawString("右键: 增大", 180,200);
    clk.drawString("C键: 返回", 180,228);

    clk.unloadFont(); //释放加载字体资源
    clk.pushSprite(0,0);
    clk.deleteSprite();
}

void game_select_page(uint8_t c) //游戏选择页面
{
    clk.setColorDepth(8);
    clk.createSprite(240, 240);
    clk.fillSprite(TFT_WHITE);
    
    if(c == GAME_1){
        clk.drawRect(70, 105, 100, 30, 0xb63b);
    } 
    if(c == GAME_2){
        clk.drawRect(70, 155, 100, 30, 0xb63b);
    } 

    clk.loadFont(ZdyLwFont_20); //加载font/ZdyLwFont_20字体
    clk.setTextDatum(CC_DATUM);
    clk.setTextColor(TFT_BLACK, TFT_WHITE); 
    clk.drawString("选择游戏", 120,50);
    clk.drawString("Tux Racer", 120,120);
    clk.drawString("待开发...", 120,170);
    clk.setTextColor(TFT_GREY, TFT_WHITE); 
    clk.drawString("D键: 选择", 60,225);
    clk.drawString("C键: 返回", 180,225);

    clk.unloadFont(); //释放加载字体资源
    clk.pushSprite(0,0);
    clk.deleteSprite();
}
