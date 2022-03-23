#include <TimeLib.h>
#include <TFT_eSPI.h> 
#include <SPI.h>
#include <TJpg_Decoder.h>

#include "img/firework_img.h" 

#define USE_DMA 1

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite clk = TFT_eSprite(&tft);


uint16_t  PROGMEM dmaBuffer1[32*32]; // Toggle buffer for 32*32 MCU block, 1024bytes
uint16_t  PROGMEM dmaBuffer2[32*32]; // Toggle buffer for 32*32 MCU block, 1024bytes
uint16_t* dmaBufferPtr = dmaBuffer1;
bool dmaBufferSel = 0;

// This next function will be called during decoding of the jpeg file to
// render each block to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.

#if USE_DMA
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if ( y >= tft.height() ) return 0;
 
  // Double buffering is used, the bitmap is copied to the buffer by pushImageDMA() the
  // bitmap can then be updated by the jpeg decoder while DMA is in progress
  if (dmaBufferSel) dmaBufferPtr = dmaBuffer2;
  else dmaBufferPtr = dmaBuffer1;
  dmaBufferSel = !dmaBufferSel; // Toggle buffer selection
  //  pushImageDMA() will clip the image block at screen boundaries before initiating DMA
  tft.pushImageDMA(x, y, w, h, bitmap, dmaBufferPtr); // Initiate DMA - blocking only if last DMA is not complete
  return 1;
}

#else
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  // tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}

#endif

void setup()
{

    //背光调节
  ledcSetup(0,5000,8);
  ledcAttachPin(22,0);
  ledcWrite(0,220);

  Serial.begin(115200);
  Serial.println("\n\n Testing dma library");

  // Initialise the TFT
  tft.begin();
#if USE_DMA
  tft.initDMA();
#endif
  tft.setRotation(2);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.fillScreen(TFT_BLACK);

  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);

  // The byte order can be swapped (set true for TFT_eSPI)
  TJpgDec.setSwapBytes(true);

  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);
}

void loop()
{
  tft.fillScreen(TFT_RED);

  // Time recorded for test purposes
  uint32_t t = millis();

  // Get the width and height in pixels of the jpeg if you wish
  uint16_t w = 0, h = 0;
  TJpgDec.getJpgSize(&w, &h, img_0, sizeof(img_0));
  Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);

#if USE_DMA
  tft.startWrite();//必须先使用startWrite，以便TFT芯片选择保持低的DMA和SPI通道设置保持配置
  //TJpgDec.drawJpg(0,0,firework_img[imgNum], sizeof(firework_img[imgNum]));//在左上角的0,0处绘制图像——在这个草图中，DMA请求在回调tft_output()中处理
  //tft.endWrite();//必须使用endWrite来释放TFT芯片选择和释放SPI通道吗
  //memset(&img_buff,0,sizeof(img_buff));//清空buff
#endif

  // Draw the image, top left at 0,0
  TJpgDec.drawJpg(0, 0, img_0, sizeof(img_0));

  // How much time did rendering take (ESP8266 80MHz 262ms, 160MHz 149ms, ESP32 SPI 111ms, 8bit parallel 90ms
  t = millis() - t;
  Serial.print(t); Serial.println(" ms");

  // Wait before drawing again
  delay(2000);
}
