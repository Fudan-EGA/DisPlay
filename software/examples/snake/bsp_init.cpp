/*
 * 描述：板载外设功能初始化
 * 
 * 
 */

#include "bsp_init.h"

static uint8_t keyNum; //保存按键值
extern TFT_eSPI tft;       // Invoke custom library
extern TFT_eSprite clk;

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
  ledcAttachPin(BL,0);
  ledcWrite(0,200);
  
  // Initialise the TFT
  tft.begin();
#ifdef USE_DMA
  tft.initDMA();
#endif
  tft.setRotation(0);

  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);
  // The byte order can be swapped (set true for TFT_eSPI)
  TJpgDec.setSwapBytes(true);
  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);
}

uint8_t get_key()
{
  return keyNum;
}

void KEY_F_Handler()
{
  delay(50);  //消除抖动，延时50ms后再判断一次是否按下，达到防干扰效果
  if(digitalRead(KEY_F) == LOW){
    keyNum=1;   //保存按下按键的值
  }
}

void KEY_G_Handler()
{
  delay(50);
  if(digitalRead(KEY_G) == LOW){
    keyNum=2;
  }
}

void KEY_R_Handler()
{
  delay(50);
  if(digitalRead(KEY_R) == LOW){
    keyNum=4;
  }
}

void KEY_L_Handler()
{
  delay(50);
  if(digitalRead(KEY_L) == LOW){
    keyNum=3;
  }
}

void KEY_A_Handler()
{ 
  delay(50);
  if(digitalRead(KEY_A) == LOW){
    keyNum=4;
  }
}

void KEY_B_Handler()
{ 
  delay(50);
  if(digitalRead(KEY_B) == LOW){
    keyNum=5;
  }
}

void KEY_C_Handler()
{ 
  delay(50);
  if(digitalRead(KEY_C) == LOW){
    keyNum='C';
  }
}

void KEY_D_Handler()
{ 
  delay(50);
  if(digitalRead(KEY_D) == LOW){
    keyNum='D';
  }
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

void volume_set(uint8_t v)  //音量设置，最大值：5*20 = 100
{
  ledcSetup(8, 1000, 10);  
  ledcAttachPin(BEEP, 8); 
  ledcWrite(8, v*20);   
  delay(100);
  ledcWrite(8, 0); 
}

void backlight_set(uint8_t bl)  //背光亮度设置，最大值：5*20 = 100
{ 
  ledcWrite(0, bl*20);   
}
