/*
 * 功能描述：
 * 
 * 使用按键控制显示在屏幕上的方块移动
 * 
 * 屏幕显示 + 按键交互功能演示例程
 * 
 */

/*
A few colour codes:

code  color
0x0000  Black
0xFFFF  White
0xBDF7  Light Gray
0x7BEF  Dark Gray
0xF800  Red
0xFFE0  Yellow
0xFBE0  Orange
0x79E0  Brown
0x7E0 Green
0x7FF Cyan
0x1F  Blue
0xF81F  Pink

 */

#include <TFT_eSPI.h> // Hardware-specific library
#include <TJpg_Decoder.h>
#include <SPI.h>
#include<vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include<iostream>
#include<string>

#include "GUI_draw.h"
#include "bsp_init.h"
#include "page.h"
//#include "game.h"

using namespace std;

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
TFT_eSprite clk = TFT_eSprite(&tft); //sprite类实例化（实现内置显存功能）

/**********按键相关函数声明*************/
char keyNum=1; //保存按键值

class something
{
  public:
  int n;
  int x;
  int y;
};


/**********游戏结构相关函数声明*************/
int state=0;
int Menuidx=0;//菜单选项
int score=0;//得分
int endgameidx=0;//游戏结束时的选项
vector<void(*)()> gamestate;//游戏状态

void Menu();//菜单函数
void Game();//游戏进行
void EndGame();//游戏结束

/**********游戏相关函数声明*************/
   vector<something> v;
int cj(vector<something> &v,int x,int y);//判断玩家是否碰撞以及碰撞情况
void initsomething(vector<something> &v,uint16_t &cd);//创建新东西
void refresh(vector<something> &v);//刷新位置


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200); //串口打印，波特率115200
  

  BEEP_Init();
  KEY_Init(); //按键初始化
  tft_DMA_Init(); //TFT屏幕DMA刷新初始化


  /*绘制文字*/
  clk.setColorDepth(8);
  clk.createSprite(240,240);    //创建显存区域
  clk.fillScreen(TFT_WHITE); //填充全屏白色，TFT_BLACK为TFT库内部定义，可直接使用
  //屏幕显示文字内容：
  clk.setTextColor(TFT_BLACK);
  clk.drawCentreString("Tux Racer initial test", 120, 2, 4); // Draw text centre at position 120, 0 using font 4
  // Draw the image, top left at 0,0
  clk.pushSprite(0, 0); //左上角位置
  clk.deleteSprite();
  gamestate.push_back(Menu);
  gamestate.push_back(Game);
  gamestate.push_back(EndGame);

}

uint16_t dt=20; //设置屏幕刷新时间间隔（单位ms）
uint32_t lastTime=0;

void loop() {
  // put your main code here, to run repeatedly:

  // Draw the image, top left at 0,0
  //屏幕显示文字内容：
  //clk.createSprite(240,240);    //创建显存区域
  //clk.fillScreen(TFT_WHITE); //填充全屏白色，TFT_BLACK为TFT库内部定义，可直接使用
  

//  tft.startWrite();//必须先使用startWrite，以便TFT芯片选择保持低的DMA和SPI通道设置保持配置
  if(millis()-lastTime > dt){  //获取当前时间，并与之前时间进行比较，若大于间隔，则刷新屏幕
    lastTime = millis();
    //gamestate[state]();
    home_page(GAME_SELECT);
    delay(2000);
    home_page(SETTING);
    delay(2000);
    game_select_page(GAME_1);
    delay(2000);
    game_select_page(GAME_2);
    delay(2000);
    setting_page(VOLUME_SELECT, 3, 4);  //设置页面
    delay(2000);
    setting_page(LIGHT_SELECT, 3, 4);  //设置页面
    delay(2000);
    setting_page(VOLUME_UP, 3, 4);  //设置页面
    delay(2000);
    setting_page(LIGHT_DOWN, 3, 4);  //设置页面
    delay(2000);

    /*if(keyNum){
      Serial.println(keyNum);  //串口输出按键值
      switch(keyNum){
        case FORWARD:
          //可添加按下按键对应的操作
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
        y+=10;
        
        default:break;
      }
    }*/
  
  }
  /*if(keyNum==FORWARD){
    TJpgDec.drawJpg( x, y, penguinL_60, sizeof(penguinL_60));//在左上角的x,y处绘制图像——在这个草图中，DMA请求在回调tft_output()中处理
  }
  else 
    TJpgDec.drawJpg( x, y,penguinR_60, sizeof(penguinR_60));//在左上角的x,y处绘制图像——在这个草图中，DMA请求在回调tft_output()中处理*/
}



void Menu()
{
  clk.setTextColor(TFT_BLACK);
  clk.drawCentreString("Tux Racer Menu", 120, 0, 4); // Draw text centre at position 120, 0 using font 4
  clk.drawCentreString("Start", 180, 80, 4); // Draw text centre at position 120, 0 using font 4
  if(Menuidx==0)clk.drawRect(150, 75, 60, 30,TFT_BLACK);
  clk.drawCentreString("Exit", 180, 160, 4); // Draw text centre at position 120, 0 using font 4
  if(Menuidx==1)clk.drawRect(150, 155, 60, 30,TFT_BLACK);
  //tft.endWrite();//必须使用endWrite来释放TFT芯片选择和释放SPI通道吗
  
  if(keyNum=='D')
      {
        if(Menuidx==0)
        {
          score=0;
        keyNum=1;state=(state+1)%3;
        }
        if(Menuidx==1);
        //关机待写入
      }
  if(keyNum=='F'||keyNum=='G')
      {
        keyNum=1;Menuidx=(Menuidx+1)%2;
      }
  clk.pushSprite(0, 0); //左上角位置
  clk.deleteSprite();
}

void Game()
{
   static uint16_t x = 120, y = 50,j=0,cd=0; //定义方块中心位置
  if(keyNum){
      Serial.println(keyNum);  //串口输出按键值
      switch(keyNum){
        case FORWARD:
          //可添加按下按键对应的操作
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
          //if(y<220){
          //  y+=3;
          //}

          break;
        case LEFT:
        {
        //keyNum=1;state=(state+1)%3;
        }
          //if(y>2){
          //  y-=3;
          //}
          break;
        case 'A':
        
        case 'B':
        
        case 'C':
        
        case 'D':
        
        default:break;
      }
    }
    if(keyNum==FORWARD){
    TJpgDec.drawJpg( x, y, penguinL_60, sizeof(penguinL_60));//在左上角的x,y处绘制图像——在这个草图中，DMA请求在回调tft_output()中处理
  }
  else 
    TJpgDec.drawJpg( x, y,penguinR_60, sizeof(penguinR_60));//在左上角的x,y处绘制图像——在这个草图中，DMA请求在回调tft_output()中处理



  
  refresh(v);
  
    vector<something>::iterator it;
    it=v.begin();
    while(it!=v.end())
    {
      if(it->n==1)draw_stone(it->x,it->y);
      if(it->n==2)draw_tree(it->x,it->y);
      if(it->n==3)draw_star(it->x,it->y);
      it++;
    }
  j=cj(v,(int)x,(int)y);
  cd++;
  initsomething(v,cd);
  if(j==1)
  {
        keyNum=1;state=(state+1)%3;
  }
  if(j==2)
  {
        score++;
  }

    
  clk.setTextColor(TFT_BLACK);
  clk.drawCentreString("Score", 120, 0, 4); // Draw text centre at position 120, 0 using font 4
  clk.drawFloat((float)score, 150, 0, 4); 
  //tft.endWrite();//必须使用endWrite来释放TFT芯片选择和释放SPI通道吗
  
  clk.pushSprite(0, 0); //左上角位置
  clk.deleteSprite();
     
  
}

void EndGame()
{
  clk.setTextColor(TFT_BLACK);
  clk.drawCentreString("Tux Racer Menu", 120, 0, 4); // Draw text centre at position 120, 0 using font 4
  clk.drawCentreString("reStart", 180, 80, 4); // Draw text centre at position 120, 0 using font 4
  if(endgameidx==0)clk.drawRect(140, 75, 80, 30,TFT_BLACK);
  clk.drawCentreString("Menu", 180, 160, 4); // Draw text centre at position 120, 0 using font 4
  if(endgameidx==1)clk.drawRect(140, 155, 80, 30,TFT_BLACK);
  //tft.endWrite();//必须使用endWrite来释放TFT芯片选择和释放SPI通道吗
  
  if(keyNum=='D')
      {
        if(endgameidx==1)
        {
        keyNum=1;state=0;
        }
        if(endgameidx==0);
        {
        keyNum=1;
        v.resize(0);
        score=0;
        state--;
        }
      }
  if(keyNum=='F'||keyNum=='G')
      {
        keyNum=1;endgameidx=(endgameidx+1)%2;
      }
  clk.pushSprite(0, 0); //左上角位置
  clk.deleteSprite();
}



int cj(vector<something> &v,int x,int y)
{
    vector<something>::iterator it;
    it=v.begin();
    while(it!=v.end())
    {
    if(abs(it->x-x)<20&&abs(it->y-y)<20)//死亡或奖励的判断条件(撞击的位置判断待修补)
    {
      if(it->n==1||it->n==2)
      {
        return 1;
      }
      if(it->n==3)
      {
        v.erase(it);
        return 2;
      }
    }
    it++;
  }
  return 0;
}
void initsomething(vector<something> &v,uint16_t &cd)
{
  srand((int)time(0)); 
    if(v.empty()) 
    {
      something* p=new something;
      p->n=1+rand()%3;
      p->x=rand()%240;
      p->y=240+rand()%10;
      v.push_back(*p);
    }
    else
    {
      int i=rand()%100;
        if(i>10&&i<20&&cd>20)
        {
      something* p=new something;
      p->n=1;
      p->x=rand()%240;
      p->y=240+rand()%10;
      v.push_back(*p);
        cd=0;
          
        }
        if(i>20&&i<30&&cd>20)
        {
      something* p=new something;
      p->n=2;
      p->x=rand()%240;
      p->y=240+rand()%10;
      v.push_back(*p);
        cd=0;
          
        }
        if(i>30&&i<60&&cd>20)
        {
      something* p=new something;
      p->n=3;
      p->x=rand()%240;
      p->y=240+rand()%10;
      v.push_back(*p);
        cd=0;
          
        }
      
    }
    return ;
}
void refresh(vector<something> &v)
{
    vector<something>::iterator it;
    it=v.begin();
    while(it!=v.end())
    {
      it->y-=3;
      if(it->y<=0)it=v.erase(it);
      else it++;
    }
    
    return;
}
