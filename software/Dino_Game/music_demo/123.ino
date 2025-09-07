#include <Arduino.h>
#define BEEP 15
#define 1_ 523
#define 2_ 587
#define 3_ 659
#define 4_ 698
#define 5_ 784
#define 6_ 880
#define 7_ 988



unsigned long currentTime = 0;
unsigned long previousTime = 0;
unsigned long interval = 200; //每1秒触发一次定时器

int music[8]={523,587,659,698,784,880,988,1046}; //音乐频率数组，中音1-7+高音1
int music2[7]={523,698,698,698,659,587,523}; //音乐频率数组，中音1-7+高音1
int music3[7]={}; //音乐频率数组，中音1-7+高音1

void music_set(char note) //设置音符：1、2、3、4、5、6、7、8（升1）
{
    ledcSetup(8, music2[note-1], 10);  //设置LEDC通道8频率为music数组，即不同音高
    ledcAttachPin(BEEP, 8); //设置LEDC通道8在IO上输出
    ledcWrite(8, 10); //设置输出PWM占空比
    //delay(200);  //延时200毫秒
}

void setup() {
  Serial.begin(9600);
  ledcSetup(8, 1000, 10);  //设置LEDC通道8频率为1000，分辨率为10位，即占空比可选0~1023
  ledcAttachPin(BEEP, 8); //设置LEDC通道8在IO上输出
}

void loop() {
  currentTime = millis(); //获取当前的时间戳
  static int i = 0;
  if (currentTime - previousTime >= interval) { //如果时间间隔达到设定的触发间隔
    previousTime = currentTime; //更新时间戳
    Serial.println("定时器触发！"); //在串口中输出一条消息
    music_set(i++ % 8);
  }
}
