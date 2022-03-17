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
#include <SPI.h>

#define TFT_GREY 0x5AEB

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

//各按键中断处理函数:
void KEY_F_Handler();
void KEY_G_Handler();
void KEY_R_Handler();
void KEY_L_Handler();
void KEY_A_Handler();
void KEY_B_Handler();
void KEY_C_Handler();
void KEY_D_Handler();


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200); //串口打印，波特率115200
  
  ledcSetup(8, 1000, 10);  //设置LEDC通道8频率为1000，分辨率为10位，即占空比可选0~1023
  ledcAttachPin(BEEP, 8); //设置LEDC通道8在IO上输出
  ledcWrite(8, 0);   //设置输出PWM占空比为0，即关闭蜂鸣器
  
  KEY_Init(); //按键初始化
  
  tft.init(); //屏幕初始化
  tft.setRotation(0); //设置屏幕旋转角度，参数为：0, 1, 2, 3 分别代表 0°、90°、180°、270°，可设置4为镜像
  tft.fillScreen(TFT_BLACK); //填充全屏黑色，TFT_BLACK为TFT库内部定义，可直接使用
}

uint16_t dt=40; //设置屏幕刷新时间间隔（单位ms）
uint16_t lastTime=0;

void loop() {
  // put your main code here, to run repeatedly:
  static uint16_t x = 0, y = 0; //定义方块中心位置

  /*绘制文字*/
  clk.setColorDepth(8);
  clk.createSprite(240,240);    //创建显存区域

  clk.fillScreen(TFT_BLACK); //填充全屏黑色，TFT_BLACK为TFT库内部定义，可直接使用


  
  //屏幕显示文字内容：
  clk.setTextColor(TFT_WHITE);
  clk.drawCentreString("Display&key test", 120, 2, 4); // Draw text centre at position 120, 0 using font 4

  if(millis()-lastTime > dt){  //获取当前时间，并与之前时间进行比较，若大于间隔，则刷新屏幕
    lastTime = millis();
    if(keyNum){
      Serial.println(keyNum);  //串口输出按键值
      switch(keyNum){
        case FORWARD:
          //可添加按下按键对应的操作
          if(x>2){
            x-=2;
          }

          break;
        case BACKWARD:
          if(x<220){
            x+=2;
          }

          break;
        case RIGHT:
          if(y<220){
            y+=2;
          }

          break;
        case LEFT:
          if(y>2){
            y-=2;
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

  clk.fillRect(x, y, 20, 20, TFT_WHITE); //矩形填充，参数：(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
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

void KEY_F_Handler()
{
  delay(50);  //消除抖动，延时50ms后再判断一次是否按下，达到防干扰效果
  if(digitalRead(KEY_F) == LOW){
    keyNum='F';   //保存按下按键的值
  }
}

void KEY_G_Handler()
{
  delay(50);
  if(digitalRead(KEY_G) == LOW){
    keyNum='G';
  }
}

void KEY_R_Handler()
{
  
  delay(50);
  if(digitalRead(KEY_R) == LOW){
    keyNum='R';
  }
  
}

void KEY_L_Handler()
{
  delay(50);
  if(digitalRead(KEY_L) == LOW){
    keyNum='L';
  }
}

void KEY_A_Handler()
{
  
  delay(50);
  if(digitalRead(KEY_A) == LOW){
    keyNum='A';
  }
}

void KEY_B_Handler()
{
  
  delay(50);
  if(digitalRead(KEY_B) == LOW){
    keyNum='B';
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
