#include <TFT_eSPI.h>
#include "img.h"


/******/
#include <Arduino.h>
#define BEEP 15
unsigned long currentTime = 0;
unsigned long previousTime = 0;
unsigned long interval = 200; //每1秒触发一次定时器

int music[8]={523,587,659,698,784,880,988,1046}; //音乐频率数组，中音1-7+高音1
int music2[7]={523,698,698,698,659,587,523}; //音乐频率数组，中音1-7+高音1
void music_set(char note) //设置音符：1、2、3、4、5、6、7、8（升1）
{
    ledcSetup(8, music2[note-1], 10);  //设置LEDC通道8频率为music数组，即不同音高
    ledcAttachPin(BEEP, 8); //设置LEDC通道8在IO上输出
    ledcWrite(8, 40); //设置输出PWM占空比
    //delay(200);  //延时200毫秒
}


/***PINS DEFINITION*///
#define KEY_F 14
#define KEY_G 25
#define KEY_A 35
#define KEY_B 34
#define KEY_D 21


/***GLOBAL VAR***/
  //PHYSICS
const int GROUND = 210;
const int GRAVITY = 4;
  //DINO & OBSTACLE POS
// const int X_D = 10;
// int Y_D = GROUND;
// int V_D = 0;
// int X_O = 250;
  //LOGIC
int RECORD = 0;
bool start = false;
bool det_G = false;


/***FUNCTION DECLARE*/
//IMG
void img_init();
void draw_dino_run(bool flag);
void draw_dino_jump(int Y_OLD, int Y_NEW); //for dino jump

//KEY
void KEY_Init();
void KEY_F_Handler();

//MENU
void game();
void gameover(int score);

//DINO CONTRL
void walk(int &, bool &);
void jump(int & score,int & Y_D, int & V_D, bool & isjump, bool & inair);

//LOGIC
void updateobs1(int score, int & , int);
bool detect1(int, int);
void updateobs2(int score, int & , int);
bool detect2(int, int);
void updateobs3(int score, int &);
bool detect3(int, int);


TFT_eSPI tft;
TFT_eSprite sprite_dino = TFT_eSprite(&tft);
TFT_eSprite sprite1 = TFT_eSprite(&tft);
TFT_eSprite sprite2 = TFT_eSprite(&tft);
TFT_eSprite sprite3 = TFT_eSprite(&tft);

void setup() {
  Serial.begin(115200); 
  KEY_Init(); 
  img_init(); 


  ledcSetup(8, 1000, 10);  //设置LEDC通道8频率为1000，分辨率为10位，即占空比可选0~1023
  ledcAttachPin(BEEP, 8); //设置LEDC通道8在IO上输出
}

void loop()
{
  // put your main code here, to run repeatedly:
  if(start == false && digitalRead(KEY_A) == LOW)
  {delay(20);if(digitalRead(KEY_A) == LOW)
    {
      start = false;
      tft.fillScreen(TFT_WHITE);
      game();
    }
  }
  else
  {;
  }
}


/***KEY***/
void KEY_Init()
{
  pinMode(KEY_F, INPUT_PULLUP);
  pinMode(KEY_G, INPUT_PULLUP);
  pinMode(KEY_A, INPUT_PULLUP);
  pinMode(KEY_B, INPUT_PULLUP);
  pinMode(KEY_D, INPUT_PULLUP);
}


/***IMG***/
void img_init()
{
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK);
  //dino img
  sprite_dino.setColorDepth(16);
  sprite_dino.createSprite(40, 43);
  //obstacle img
  sprite1.setColorDepth(16);
  sprite1.createSprite(15, 33);
  sprite1.pushImage(0, 0, 15, 33, tree1);

  sprite2.setColorDepth(16);
  sprite2.createSprite(23, 42);
  sprite2.pushImage(0, 0, 23, 42, tree2);
  
  sprite3.setColorDepth(16);
  sprite3.createSprite(42, 36);
  sprite3.pushImage(0, 0, 42, 36, bird1);
  
  //fill start menu
  tft.pushImage(0, 0, 240, 240, menu);
  tft.drawString("HI: " + String(RECORD), 120, 5);
}

void draw_dino_run(bool flag)
{
  if(flag)
    sprite_dino.pushImage(0, 0, 40, 43, dino1);
  else
    sprite_dino.pushImage(0, 0, 40, 43, dino2);

  sprite_dino.pushSprite(10, GROUND - 43);
}

void draw_dino_jump(int Y_OLD, int Y_NEW)
{
  sprite_dino.pushImage(0, 0, 40, 43, dino0);
  int index = abs(Y_OLD - Y_NEW);
  bool up = Y_OLD > Y_NEW ? true : false;
  if(up)
  {
//    Serial.println("up");
    sprite_dino.pushSprite(10, Y_NEW);
    tft.fillRect(10, Y_NEW + 43, 40, Y_OLD - Y_NEW, TFT_WHITE);
  }
  else
  {
//    Serial.println("down");
    Serial.println(Y_NEW);
//    Serial.println(Y_OLD);
    sprite_dino.pushSprite(10, Y_NEW);
    tft.fillRect(10, Y_OLD, 40, Y_NEW - Y_OLD, TFT_WHITE);
  }
}


/***LOGIC***/
void game()
{
  // Serial.println("game start\n");
  //clear and init
  bool alive = true;
  bool leftfoot = true;
  bool isjump = false;
  bool inair = false;
  //dino pos
  const int X_D = 10;
  int Y_D = GROUND - 43;
  int V_D = 0;
  //obs pos
  const int Y_O = GROUND - 33;
  int X_O1 = 240 + random(100);
  int X_O2 = 360 + random(100);
  int X_O3 = 240;
  int score = 0;

//
//
//while(1) updateobs3(score, X_O3) delay(100);
//
//

  while(alive)
  {

    updateobs1(score, X_O1, X_O2);
    updateobs2(score, X_O2, X_O1);
    updateobs3(score, X_O3);
    
    //each loop for one frame
    //choose move type
    if(digitalRead(KEY_F) == LOW){
      delay(15);if(digitalRead(KEY_F) == LOW){isjump = true;/*Serial.println("F pressed");*/}}

    if(isjump)
      jump(score,Y_D, V_D, isjump, inair);
    else
      walk(score, leftfoot);

//    updateobs1(score, X_O1);
//    updateobs2(score, X_O2);
//    updateobs3(score, X_O3);

    alive = detect1(Y_D, X_O1) && detect2(Y_D, X_O2);
    delay(100);
  }
  gameover(score);
}

void gameover(int score)
{ 
  RECORD = RECORD > score ? RECORD : score;
  tft.pushImage(0, 0, 240, 240, endmenu);//fill over menu
  tft.drawString("HI: " + String(RECORD), 120, 5);
  tft.drawString("YOU: " + String(score), 120, 20);
  while(!digitalRead(KEY_B) == LOW);
  tft.pushImage(0, 0, 240, 240, menu);//fill start menu
  tft.drawString("HI: " + String(RECORD), 120, 5);
  
//  currentTime = millis(); //获取当前的时间戳
  int i = 0;
//  ledcWrite(8, 10); //设置输出PWM占空比
  while(i < 9){
  currentTime = millis(); //获取当前的时间戳
  if (currentTime - previousTime >= interval) { //如果时间间隔达到设定的触发间隔
    previousTime = currentTime; //更新时间戳
    Serial.println("定时器触发！"); //在串口中输出一条消息
    music_set(i++ % 9);
  }
  }
  ledcWrite(8, 0); //设置输出PWM占空比
}


/***CONTRL***/
void walk(int & score, bool & leftfoot)
{
  draw_dino_run(leftfoot);
  leftfoot = !leftfoot;
  score++;
  tft.fillRect(190, 5, 50, 15, TFT_WHITE);
  tft.drawString("SCORE: " + String(score), 110, 5);
}

void jump(int & score,int & Y_D, int & V_D, bool & isjump, bool & inair)
{
  Serial.println("jump");
  static int state;
  static int Y_OLD;
  if(inair == false)
  {
    inair = true;
    Y_D = GROUND - 44;
    V_D = -35;
    state = 0;
    Y_OLD = Y_D;
  }
  
  switch (state)
  {
  case 0://quick up
//  Serial.println("case0");
    Y_OLD = Y_D;
    Y_D += V_D / 3;
    V_D += 3;
    if(Y_D < GROUND - 80)
      state++;
    break;
  case 1:// slow up
//  Serial.println("case1");
    Y_OLD = Y_D;
    Y_D += V_D / 2;
    V_D += 3;
    if(V_D > 0)
      state++;
    break;
  case 2:// down;
//  Serial.println("case2");
    Y_OLD = Y_D;
    if(digitalRead(KEY_F) == LOW)//slow down
      {Y_D++;V_D--;}
    else if(digitalRead(KEY_G) == LOW)//quick down
      Y_D += 3 * V_D;
    else
      Y_D += V_D;
    V_D++;
    break;
  }
  draw_dino_jump(Y_OLD, Y_D);
  
  if(Y_D >= GROUND -44)
  {
    isjump = false;
    inair = false;
    state = 0;
    V_D = 0;
    Y_OLD = Y_D;
    Y_D = GROUND - 43;
    draw_dino_jump(Y_OLD, Y_D);
  }      
  score++;
  tft.fillRect(190, 5, 50, 15, TFT_WHITE);
  tft.drawString("SCORE: " + String(score), 110, 5);

//  delay(100);
//  Serial.println("jump end");
}



/******obstacle********/
//IF   YOU WANT TO ADD NEW OBSTACLE, CHECK HERE


void updateobs1(int score, int & X_O, int x1)
{
  int X_OLD = X_O;
  int index = min(2 + score / 40, 10);
  X_O -= index;
  sprite1.pushSprite(X_O, GROUND - 33);
  tft.fillRect(X_O + 15, GROUND - 33, index, 33, TFT_WHITE);
  if(X_O < -15)
    X_O = x1 + random(120, 320);
}


bool detect1(int Y_D, int X_O)
{
  bool alive = true;
  if(X_O < 45 && X_O > 0)
      alive = Y_D < GROUND - 80 ? true : false;
//Serial.println(Y_D);
 if(digitalRead(KEY_D) == LOW)
 {
   delay(20);
   if(digitalRead(KEY_D) == LOW){
   alive = true;
   Serial.println("YOU HIT THE POINT");
  }
 }
  return alive;
}

void updateobs2(int score, int & X_O, int x1)
{
  int X_OLD = X_O;
  int index = min(2 + score / 40, 10);
  X_O -= index;
  sprite2.pushSprite(X_O, GROUND - 44);
  tft.fillRect(X_O + 23, GROUND - 44, index, 44, TFT_WHITE);
  if(X_O < -23)
    X_O = x1 + random(180, 320);
}

bool detect2(int Y_D, int X_O)
{
  bool alive = true;
  if(X_O < 45 && X_O > -5)
      alive = Y_D < GROUND - 90 ? true : false;
//Serial.println(Y_D);
 if(digitalRead(KEY_D) == LOW)
 {
   delay(20);
   if(digitalRead(KEY_D) == LOW){
   alive = true;
   Serial.println("YOU HIT THE POINT");
  }
 }
  return alive;
}

void updateobs3(int score, int & X_O)
{
  int index = random(6);
  static bool forward = true;
  if(X_O < 45)
    forward = false;
  else if(X_O > 250)
    forward = true;
  
  if(!forward)//backward
  {
    X_O += index;
    sprite3.pushSprite(X_O, GROUND - 140);
    tft.fillRect(X_O - index, GROUND - 140, index, 36, TFT_WHITE);
  }
  else
  {
    X_O -= index;
    sprite3.pushSprite(X_O, GROUND - 140);
    tft.fillRect(X_O + 44, GROUND - 140, index, 36, TFT_WHITE);
  }
    //clear！！！
}

bool detect3(int Y_D, int X_O)
{
  bool alive = true;
  if(X_O < 45 && X_O > -20)
      alive = Y_D > 110 ? true : false;
Serial.println(Y_D);
 if(digitalRead(KEY_D) == LOW)
 {
   delay(20);
   if(digitalRead(KEY_D) == LOW){
   alive = true;
   Serial.println("YOU HIT THE POINT");
  }
 }
  return alive;
}
