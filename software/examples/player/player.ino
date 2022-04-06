/*
 * 功能描述：按键电子琴演示例程，分别按下8个按键，将发出八种音符声音，模拟音乐键盘
 * 
 * 
 */


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


int music[8]={523,587,659,698,784,880,988,1046}; //音乐频率数组，中音1-7+高音1

void music_set(char note); //设置蜂鸣器音符：1、2、3、4、5、6、7、8（升1）

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
  ledcSetup(8, 1000, 10);  //设置LEDC通道8频率为1000，分辨率为10位，即占空比可选0~1023
  ledcAttachPin(BEEP, 8); //设置LEDC通道8在IO上输出
  ledcWrite(8, 0);   //设置输出PWM占空比为0，即关闭蜂鸣器
  KEY_Init();
  Serial.begin(115200); //串口打印，波特率115200
  
  /*响度测试
  ledcWrite(8,2);
  delay(1000);
  ledcWrite(8,10);
  delay(1000);
  ledcWrite(8,20);
  delay(1000);
  ledcWrite(8,50);
  delay(1000);
  ledcWrite(8,100);
  delay(1000);
  ledcWrite(8,200);
  */
}

int set_flag=0;

void loop() {
  // put your main code here, to run repeatedly:
  ledcWrite(8, 0);   //设置输出PWM占空比为0，即关闭蜂鸣器
    
  if(keyNum){
    Serial.println(keyNum);  //串口输出按键值
    switch(keyNum){
      case 'F':
        //可添加按下按键对应的操作
      case 'G':
      
      case 'R':
      
      case 'L':
      
      case 'A':
      
      case 'B':
      
      case 'C':
      
      case 'D':
      
      default:break;
    }
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

void KEY_F_Handler()
{
  
  delay(50);  //消除抖动，50ms后再判断一次是否按下，防干扰
  if(digitalRead(KEY_F) == LOW){
    keyNum='F';
    music_set(1);
  }
}

void KEY_G_Handler()
{
  delay(50);
  if(digitalRead(KEY_G) == LOW){
    keyNum='G';
    music_set(2);
  }
}

void KEY_R_Handler()
{
  
  delay(50);
  if(digitalRead(KEY_R) == LOW){
    keyNum='R';
    music_set(3);
  }
  
}

void KEY_L_Handler()
{
  
  delay(50);
  if(digitalRead(KEY_L) == LOW){
    keyNum='L';
    music_set(4);
  }
}

void KEY_A_Handler()
{
  
  delay(50);
  if(digitalRead(KEY_A) == LOW){
    keyNum='A';
    music_set(5);
  }
}

void KEY_B_Handler()
{
  
  delay(50);
  if(digitalRead(KEY_B) == LOW){
    keyNum='B';
    music_set(6);
  }
}

void KEY_C_Handler()
{
  
  delay(50);
  if(digitalRead(KEY_C) == LOW){
    keyNum='C';
    music_set(7);
  }
}

void KEY_D_Handler()
{
  
  delay(50);
  if(digitalRead(KEY_D) == LOW){
    keyNum='D';
    music_set(8);
  }
}

void music_set(char note) //设置音符：1、2、3、4、5、6、7、8（升1）
{
    ledcSetup(8, music[note-1], 10);  //设置LEDC通道8频率为music数组，即不同音高
    ledcAttachPin(BEEP, 8); //设置LEDC通道8在IO上输出
    ledcWrite(8, 20); //设置输出PWM占空比
    delay(200);  //延时200毫秒
}
