#include "page.h"

extern TFT_eSPI tft;       // Invoke custom library
extern TFT_eSprite clk;
extern uint8_t volume ; /*音量：0-5档*/
extern uint8_t light ; /*背光亮度 0-5档*/

const String set_level[]={"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
static uint16_t page_state=1;

void home_page(uint8_t c) //首页
{
    clk.setColorDepth(8);
    clk.createSprite(240, 240);
    clk.fillSprite(TFT_BLACK);

    if(c == GAME_SELECT){
        clk.drawRect(75, 114, 95, 30, 0xb63b);
    } 
    if(c == SETTING){
        clk.drawRect(95, 164, 50, 30, 0xb63b);
    } 
    
    clk.setTextColor(TFT_WHITE);
    clk.drawCentreString("DisPlay", 120, 40, 4); // Draw text centre at position 120, 0 using font 4

    clk.loadFont(ZdyLwFont_20); //加载font/ZdyLwFont_20字体
    clk.setTextDatum(CC_DATUM);
    clk.setTextColor(TFT_WHITE, TFT_BLACK); 
    clk.drawString("选择游戏", 120,130);
    clk.drawString("设置", 120,180);
    clk.setTextColor(TFT_GREY, TFT_BLACK); 
    clk.drawString("[D]选择", 60,225);
    clk.drawString("[A]返回", 180,225);

    clk.unloadFont(); //释放加载字体资源
    clk.pushSprite(0,0);
    clk.deleteSprite();
}

void setting_page(uint8_t c, uint8_t vol, uint8_t light)  //设置页面
{
    clk.setColorDepth(8);
    clk.createSprite(240, 240);
    clk.fillSprite(TFT_BLACK);
    clk.loadFont(ZdyLwFont_20); //加载font/ZdyLwFont_20字体
    clk.setTextDatum(CC_DATUM);
    clk.setTextColor(TFT_WHITE, TFT_BLACK); 

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
    
    clk.setTextColor(TFT_WHITE);
    clk.drawString("设置", 120, 40); // Draw text centre at position 120, 0 using font 4

    clk.drawString("音量", 70, 100);clk.drawString("-", 150, 100);clk.drawString("+", 200, 100);
    clk.drawString("亮度", 70, 140);clk.drawString("-", 150, 140);clk.drawString("+", 200, 140);
    clk.setTextColor(TFT_GREY, TFT_BLACK);

    clk.drawString("[左]减小", 60,200);clk.drawString("[右]增大", 180,200);
    clk.drawString("[D]选择", 60,225);clk.drawString("[A]返回", 180,228);

    clk.unloadFont(); //释放加载字体资源
    clk.pushSprite(0,0);
    clk.deleteSprite();
}

void game_select_page(uint8_t c) //游戏选择页面
{
    clk.setColorDepth(8);
    clk.createSprite(240, 240);
    clk.fillSprite(TFT_BLACK);
    
    if(c == GAME_1){
        clk.drawRect(70, 105, 100, 30, 0xb63b);
    } 
    if(c == GAME_2){
        clk.drawRect(70, 155, 100, 30, 0xb63b);
    } 

    clk.loadFont(ZdyLwFont_20); //加载font/ZdyLwFont_20字体
    clk.setTextDatum(CC_DATUM);
    clk.setTextColor(TFT_WHITE, TFT_BLACK); 
    clk.drawString("选择游戏", 120,50);
    clk.drawString("Tux Racer", 120,120);
    clk.drawString("待开发...", 120,170);
    clk.setTextColor(TFT_GREY, TFT_BLACK); 
    clk.drawString("[D]选择", 60,225);
    clk.drawString("[A]返回", 180,225);

    clk.unloadFont(); //释放加载字体资源
    clk.pushSprite(0,0);
    clk.deleteSprite();
}

int page_run(uint8_t key)  //页面调度状态机运行
{
    switch(page_state){
        case 1:  //首页选择游戏
            if((key == 'G') ){
                home_page(SETTING);
                page_state = 2;
                break;
            }
            if((key == 'D')){
                game_select_page(GAME_1);
                page_state = 3;
                break;
            }
            home_page(GAME_SELECT);
            break;
        case 2:   //首页选择设置
            if((key == 'F') ){
                home_page(GAME_SELECT);
                page_state = 1;
                break;
            }
            if((key == 'D')){
                setting_page(VOLUME_SELECT, volume, light);
                page_state = 5;
                break;
            }
            home_page(SETTING);
            break;
        case 3:   //选择游戏1
            if((key == 'A')){
                home_page(GAME_SELECT);
                page_state = 1;
                break;
            }
            if((key == 'G') ){
                game_select_page(GAME_2);
                page_state=4;
                break;
            }
            if((key == 'D')){
                game_select_page(GAME_1);
                page_state = 3;
                return GAME_1;  //返回参数，启动游戏1 
                break;
            }
            game_select_page(GAME_1);
            break;
        case 4:   //选择游戏2
            if((key == 'A')){
                home_page(GAME_SELECT);
                page_state = 1;
                break;
            }
            if((key == 'F')){
                game_select_page(GAME_1);
                page_state = 4;
                break;
            }
            if((key == 'D')){
                game_select_page(GAME_2);
                page_state = 4;
                return GAME_2;  //返回参数，启动游戏2
                break;
            }
            game_select_page(GAME_2);
            break;
        case 5:   //设置页音量修改
            if((key == 'A')){
                home_page(SETTING);
                page_state = 2;
                break;
            }
            if(key == 'R'){
                setting_page(VOLUME_UP, LIMIT_PLUS(volume), light);
                return 1;   //返回信息，设置更改
                break;
            }
            if(key == 'L'){
                setting_page(VOLUME_DOWN, LIMIT_SUB(volume), light);
                return 1;
                break;
            }
            if((key == 'G') ){
                setting_page(LIGHT_SELECT, volume, light);
                page_state=6;
                break;
            }
            setting_page(VOLUME_SELECT, volume, light);
            break;
        case 6:   //设置页亮度调节
            if((key == 'A')){
                home_page(SETTING);
                page_state = 2;
                break;
            }
            if(key == 'R'){
                setting_page(LIGHT_UP, volume, LIMIT_PLUS(light));
                return 2;   //返回信息，设置更改
                break;
            }
            if(key == 'L'){
                setting_page(LIGHT_DOWN, volume, LIMIT_SUB(light));
                return 2;
                break;
            }
            if((key == 'F') ){
                setting_page(VOLUME_SELECT, volume, light);
                page_state=5;
                break;
            }
            setting_page(LIGHT_SELECT, volume, light);
            break;
        default:
            break;
    }
    return 0;
}

void page_game_quit(char game) //游戏退出
{
    if(game == GAME_1)
        page_state = 3;
    if(game == GAME_2)
        page_state = 4;
}