#include "game.h"

extern char keyNum; //保存按键值
extern TFT_eSPI tft;       // Invoke custom library
extern TFT_eSprite clk;

element_t penguin, stars[5], obstacles[8];

void game_start()    //游戏开始
{
    penguin.axis_x=120;
    for(penguin.axis_y = 0;penguin.axis_y < 50;penguin.axis_y += 3 ){
        clk.createSprite(240,240);    //创建显存区域
        clk.fillScreen(TFT_WHITE); //填充全屏白色，TFT_BLACK为TFT库内部定义，可直接使用
        draw_penguinR(penguin.axis_x, penguin.axis_y);

        clk.pushSprite(0, 0); //左上角位置
        clk.deleteSprite();
    }
}

char game_run(char key)      //游戏运行
{
  /*
    while(key != 'A'){  //退出键未按下
        int x = 0, y = 0;
        clk.createSprite(240,240);    //创建显存区域
        clk.fillScreen(TFT_WHITE); //填充全屏白色，TFT_BLACK为TFT库内部定义，可直接使用

        clk.loadFont(ZdyLwFont_20); //加载font/ZdyLwFont_20字体
        clk.setTextDatum(CC_DATUM);
        clk.setTextColor(TFT_WHITE, TFT_BLACK); 

        switch(key){
            case FORWARD:
                if(x>2){
                x-=3;
                }

                break;
            case BACKWARD:
                if(x<220){
                x+=3;
                }

                break;
            case RIGHT:
                if(y<220){
                y+=3;
                }

                break;
            case LEFT:
                if(y>2){
                y-=3;
                }
                break;
            case 'A':
            
            case 'B':
            
            case 'C':
            
            case 'D':
            
            default:break;
            }
        
        if(key==FORWARD){
            draw_penguinL(x,y);
        }
        else 
            draw_penguinR(x,y);

        draw_stone(50,160-y);
        draw_tree(80,240-y);
        draw_stone(160,180-y);
        draw_tree(180,130-y);

        clk.setTextColor(TFT_BLACK);
        clk.drawCentreString("得分:", 120, 2, 4); // Draw text centre at position 120, 0 using font 4

        clk.unloadFont(); //释放字体资源
        clk.pushSprite(0, 0); //左上角位置
        clk.deleteSprite();
    }
    return 0;
*/
}

char game_stop(char key)     //游戏结束
{
    clk.createSprite(240,240);    //创建显存区域
    clk.fillScreen(TFT_WHITE); //填充全屏白色，TFT_BLACK为TFT库内部定义，可直接使用

    //分数
    //提示
    //重新开始
    //返回

    clk.pushSprite(0, 0); //左上角位置
    clk.deleteSprite();
}
