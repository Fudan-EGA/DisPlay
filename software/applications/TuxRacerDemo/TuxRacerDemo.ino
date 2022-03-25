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
#include "img/penguin.h"

#define TFT_GREY        0x5AEB
#define COLOR_ICE       0xb63b
#define COLOR_TREE      0x2444
#define COLOR_TREEROOT  0x5345
#define COLOR_STAR      0xfea0


//#define USE_DMA

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
TFT_eSprite clk = TFT_eSprite(&tft); //sprite类实例化（实现内置显存功能）

//各个管脚接口宏定义：
#define KEY_F 14
#define KEY_G 25
#define KEY_L 32
#define KEY_R 33

#define KEY_A 35
#define KEY_B 34
#define KEY_C 19
#define KEY_D 21

#define BEEP 15


//屏幕方向与按键方向不一致，需要重新定义
#define FORWARD   'L'
#define BACKWARD  'R'
#define LEFT      'F'
#define RIGHT     'G'

/**********按键相关函数声明*************/
char keyNum=1; //保存按键值

void KEY_Init();  //按键管脚初始化
void BEEP_Init(); //蜂鸣器初始化
void tft_DMA_Init(); //TFT屏幕DMA刷新初始化

//各按键中断处理函数:
void KEY_F_Handler();
void KEY_G_Handler();
void KEY_R_Handler();
void KEY_L_Handler();
void KEY_A_Handler();
void KEY_B_Handler();
void KEY_C_Handler();
void KEY_D_Handler();


/************图形绘制函数声明******************/
void draw_penguinL(uint8_t x, uint8_t y); //绘制朝左企鹅
void draw_penguinR(uint8_t x, uint8_t y); //绘制朝右企鹅
void draw_star(uint8_t x, uint8_t y); //绘制星星图案
void draw_stone(uint8_t x, uint8_t y); //绘制石头障碍物
void draw_tree(uint8_t x ,uint8_t y); //绘制障碍物树


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200); //串口打印，波特率115200

  tft_DMA_Init();
  BEEP_Init();
  KEY_Init(); //按键初始化

  /*绘制文字*/
  clk.setColorDepth(8);
  clk.createSprite(240,240);    //创建显存区域
  clk.fillScreen(TFT_WHITE); //填充全屏白色，TFT_BLACK为TFT库内部定义，可直接使用
  //屏幕显示文字内容：
  clk.setTextColor(TFT_BLACK);
  clk.drawCentreString("Tux Racer inital test", 120, 2, 4); // Draw text centre at position 120, 0 using font 4
  // Draw the image, top left at 0,0
  clk.pushSprite(0, 0); //左上角位置
  clk.deleteSprite();

}

uint16_t dt=20; //设置屏幕刷新时间间隔（单位ms）
uint32_t lastTime=0;

void loop() {
   static uint16_t x = 0, y = 0, y1=0; //定义方块中心位置

  // Draw the image, top left at 0,0
  //屏幕显示文字内容：
  clk.createSprite(240,240);    //创建显存区域
  clk.fillScreen(TFT_WHITE); //填充全屏白色，TFT_BLACK为TFT库内部定义，可直接使用
  
  if(millis()-lastTime > dt){  //获取当前时间，并与之前时间进行比较，若大于间隔，则刷新屏幕
    lastTime = millis();
    if(y1<240) y1+=3;
    else y1=0;
    
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
    }
  
  }
  if(keyNum==FORWARD){
    draw_penguinL(x,y);
  }
  else 
    draw_penguinR(x,y);

  clk.setTextColor(TFT_BLACK);
  clk.drawCentreString("Tux Racer inital test", 120, 2, 4); // Draw text centre at position 120, 0 using font 4
  //tft.endWrite();//必须使用endWrite来释放TFT芯片选择和释放SPI通道吗

  draw_stone(50,160-y1);
  draw_tree(80,240-y1);
  draw_stone(160,180-y1);
  draw_tree(180,130-y1);
  
  draw_star(120,120);
  
  clk.pushSprite(0, 0); //左上角位置
  clk.deleteSprite();

}

void KEY_Init()
{
  pinMode(KEY_F, INPUT_PULLUP);
  pinMode(KEY_G, INPUT_PULLUP);
  pinMode(KEY_R, INPUT_PULLUP);
  pinMode(KEY_L, INPUT_PULLUP);
  pinMode(KEY_A, INPUT_PULLUP);
  pinMode(KEY_B, INPUT_PULLUP);
  pinMode(KEY_C, INPUT_PULLUP);
  pinMode(KEY_D, INPUT_PULLUP);
  
  attachInterrupt(KEY_F, KEY_F_Handler, FALLING);//绑定按键中断处理句柄，下降沿中断，按下时将自动进入Handler处理
  attachInterrupt(KEY_G, KEY_G_Handler, FALLING);//绑定按键中断处理句柄，下降沿中断，按下时将自动进入Handler处理
  attachInterrupt(KEY_R, KEY_R_Handler, FALLING);//绑定按键中断处理句柄，下降沿中断，按下时将自动进入Handler处理
  attachInterrupt(KEY_L, KEY_L_Handler, FALLING);//绑定按键中断处理句柄，下降沿中断，按下时将自动进入Handler处理
  attachInterrupt(KEY_A, KEY_A_Handler, FALLING);//绑定按键中断处理句柄，下降沿中断，按下时将自动进入Handler处理
  attachInterrupt(KEY_B, KEY_B_Handler, FALLING);//绑定按键中断处理句柄，下降沿中断，按下时将自动进入Handler处理
  attachInterrupt(KEY_C, KEY_C_Handler, FALLING);//绑定按键中断处理句柄，下降沿中断，按下时将自动进入Handler处理
  attachInterrupt(KEY_D, KEY_D_Handler, FALLING);//绑定按键中断处理句柄，下降沿中断，按下时将自动进入Handler处理
  
}
void BEEP_Init()
{
  ledcSetup(8, 1000, 10);  //设置LEDC通道8频率为1000，分辨率为10位，即占空比可选0~1023
  ledcAttachPin(BEEP, 8); //设置LEDC通道8在IO上输出
  ledcWrite(8, 0);   //设置输出PWM占空比为0，即关闭蜂鸣器

}


bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

#ifdef USE_DMA
  // Double buffering is used, the bitmap is copied to the buffer by pushImageDMA() the
  // bitmap can then be updated by the jpeg decoder while DMA is in progress
  if (dmaBufferSel) dmaBufferPtr = dmaBuffer2;
  else dmaBufferPtr = dmaBuffer1;
  dmaBufferSel = !dmaBufferSel; // Toggle buffer selection
  //  pushImageDMA() will clip the image block at screen boundaries before initiating DMA
  tft.pushImageDMA(x, y, w, h, bitmap, dmaBufferPtr); // Initiate DMA - blocking only if last DMA is not complete
  // The DMA transfer of image block to the TFT is now in progress...
#else
  // Non-DMA blocking alternative
  clk.pushImage(x, y, w, h, bitmap);  // Blocking, so only returns when image block is drawn
#endif
  // Return 1 to decode next block.
  return 1;
}

void tft_DMA_Init()
{
  //背光调节，通道0
  ledcSetup(0,5000,8);
  ledcAttachPin(22,0);
  ledcWrite(0,200);
  
  // Initialise the TFT
  tft.begin();
#ifdef USE_DMA
  tft.initDMA();
#endif
  tft.setRotation(0); //设置屏幕旋转方向

  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);
  // The byte order can be swapped (set true for TFT_eSPI)
  TJpgDec.setSwapBytes(true);
  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);
}

void KEY_F_Handler(){
  delay(50);  //消除抖动，延时50ms后再判断一次是否按下，达到防干扰效果
  if(digitalRead(KEY_F) == LOW){
    keyNum='F';   //保存按下按键的值
  }
}

void KEY_G_Handler(){
  delay(50);
  if(digitalRead(KEY_G) == LOW){
    keyNum='G';
  }
}

void KEY_R_Handler(){
  delay(50);
  if(digitalRead(KEY_R) == LOW){
    keyNum='R';
  }
  
}

void KEY_L_Handler(){
  delay(50);
  if(digitalRead(KEY_L) == LOW){
    keyNum='L';
  }
}

void KEY_A_Handler(){
  delay(50);
  if(digitalRead(KEY_A) == LOW){
    keyNum='A';
  }
}

void KEY_B_Handler(){
  delay(50);
  if(digitalRead(KEY_B) == LOW){
    keyNum='B';
  }
}

void KEY_C_Handler(){
  delay(50);
  if(digitalRead(KEY_C) == LOW){
    keyNum='C';
  }
}

void KEY_D_Handler(){
  delay(50);
  if(digitalRead(KEY_D) == LOW){
    keyNum='D';
  }
}


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
