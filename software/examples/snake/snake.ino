#include "GUI_draw.h"
#include "bsp_init.h"
#include "game.h"

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
TFT_eSprite clk = TFT_eSprite(&tft); //sprite类实例化（实现内置显存功能）
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //串口打印，波特率115200
  BEEP_Init();
  KEY_Init(); //按键初始化
  tft_DMA_Init(); //TFT屏幕DMA刷新初始化

  clk.setColorDepth(8);
  clk.createSprite(240,240);    //创建显存区域
  clk.fillScreen(TFT_WHITE); //填充全屏白色，TFT_BLACK为TFT库内部定义，可直接使用
  //屏幕显示文字内容：
  clk.setTextColor(TFT_BLACK);
  
  clk.pushSprite(0, 0); //左上角位置
  clk.deleteSprite();

  init_snake_and_food();
}

void loop() {
  // put your main code here, to run repeatedly:
    clk.setColorDepth(8);
    clk.createSprite(240, 240);
    clk.fillSprite(TFT_BLACK);
    run_snake();
    clk.pushSprite(0,0);
    clk.deleteSprite();

    delay(50);
}
