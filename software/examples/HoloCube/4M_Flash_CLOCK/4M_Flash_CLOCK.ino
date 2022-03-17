#include <ArduinoJson.h> //请使用ArduinoJson V5版本，最新版V6会导致编译失败

#include <TimeLib.h>

#include <Preferences.h>
Preferences preferences; 
String PrefSSID, PrefPassword; 

#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>

#include "font/ZdyLwFont_20.h"
#include "font/FxLED_32.h"

#include "img/main_img/main_img.h"
#include "img/temperature.h"
#include "img/humidity.h"
#include "img/watch_top.h"
#include "img/watch_bottom.h"
#include "img/start_gif.h"
#include "img/weather_code_jpg.h"

#include "weather_code_jpg/d00.h"

#include "img/taikongren/i0.h"
#include "img/taikongren/i1.h"
#include "img/taikongren/i2.h"
#include "img/taikongren/i3.h"
#include "img/taikongren/i4.h"
#include "img/taikongren/i5.h"
#include "img/taikongren/i6.h"
#include "img/taikongren/i7.h"
#include "img/taikongren/i8.h"
#include "img/taikongren/i9.h"
#include "img/setWiFi_img.h"

#include "src/SetWiFi.h"

#include <TFT_eSPI.h> 
#include <SPI.h>

#include<EEPROM.h>
bool guanggao_Flag = false;

/*功能参数配置*/
#define SerialBaud 115200   //串口波特率
#define SerialON/OFF 1      //1 - 打开串口输出 0 - 关闭串口输出
unsigned int Gif_Mode = 4;          //1 - 龙猫 4 - 太空人

TFT_eSPI tft = TFT_eSPI();  
TFT_eSprite clk = TFT_eSprite(&tft);

#include <TJpg_Decoder.h>

uint32_t targetTime = 0;   
byte omm = 99;
boolean initial = 1;
byte xcolon = 0;
unsigned int colour = 0;

uint16_t bgColor = 0xFFFF;
String cityCode = "";  //天气城市代码

//NTP服务器
static const char ntpServerName[] = "ntp6.aliyun.com";
const int timeZone = 8;     //东八区

WiFiUDP Udp;
unsigned int localPort = 8000;

time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
String num2str(int digits);
void sendNTPpacket(IPAddress &address);

byte weatherCode = 99;
bool getCityWeaterFlag = false;
bool getCityCodeFlag = false;

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  // Return 1 to decode next block
  return 1;
}

byte loadNum = 6;
void loading(byte delayTime,byte NUM){
  clk.setColorDepth(8);
  clk.createSprite(200, 50);
  clk.fillSprite(0x0000);
  clk.loadFont(ZdyLwFont_20); //加载font/ZdyLwFont_20字体
  clk.drawRoundRect(0,0,200,16,8,0xFFFF);
  clk.fillRoundRect(3,3,loadNum,10,5,0xFFFF);
  clk.setTextDatum(CC_DATUM);
  clk.setTextColor(TFT_WHITE, 0x0000); 
  clk.drawString("正在连接 "+ PrefSSID + " ...",100,40,2);
  clk.pushSprite(20,110);
  clk.deleteSprite();
  loadNum += NUM;
  if(loadNum>=194){
    loadNum = 194;
  }
  delay(delayTime);
  clk.unloadFont(); //释放加载字体资源
}

//显示wifi连接失败，并重新进入配网模式
void displayConnectWifiFalse() {
  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);
  TJpgDec.drawJpg(0,0,wififalse, sizeof(wififalse)); 
  delay(5000);
}

unsigned long oldTime_1 = 0;
int imgNum_1 = 0;
int connectTimes = 0;
int lightValue=0,backLight_hour=0;

int Filter_Value;

long __tstamp;
char m[2] = {'0', '\0'};
boolean checkMillis(int m) {
  if (millis() - __tstamp > m) {
    __tstamp = millis();
    return true;
  } else {
    return false;
  }
}

//强制门户Web配网
bool setWiFi_Flag = false;
void setWiFi() {
  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);
  TJpgDec.drawJpg(0,0,setWiFi_img, sizeof(setWiFi_img));

  initBasic();
  initSoftAP();
  initWebServer();
  initDNS();
  while(setWiFi_Flag == false) {
    server.handleClient();
    dnsServer.processNextRequest();
    if(WiFi.status() == WL_CONNECTED) {
      server.stop();
      setWiFi_Flag = true;
    }
  }
}

void setup()
{
  EEPROM.begin(8);
  guanggao_Flag = EEPROM.read(0);
  tft.init();
  // 设置屏幕显示的旋转角度，参数为：0, 1, 2, 3
  // 分别代表 0°、90°、180°、270°
  tft.setRotation(0); 
  
  Serial.begin(SerialBaud);
  pinMode(4,INPUT); //配网按钮接GPIO-4
  pinMode(32,INPUT); //光敏电阻
  randomSeed(analogRead(32));
  ledcSetup(0,5000,8);
  ledcAttachPin(22,0);
  ledcWrite(0,150);

  //首次使用自动进入配网模式
  preferences.begin("wifi", false);
  PrefSSID =  preferences.getString("ssid", "none");
  PrefPassword =  preferences.getString("password", "none");
  preferences.end();
  if( PrefSSID == "none" )
  {
    //smartConfigWIFI();
    setWiFi();
  }

  int buttonStateTime = 0;
  
  while(digitalRead(4) == HIGH) {
    buttonStateTime = millis();
    clk.loadFont(ZdyLwFont_20);
    clk.createSprite(240, 80); 
    clk.setTextDatum(CC_DATUM);
    clk.setTextColor(TFT_WHITE, bgColor);
    if(buttonStateTime >= 15500) {
      clk.drawString("" + String(millis()/1000) + "秒" + " ON/OFF广告",120,40);
    } 
    else if(buttonStateTime >= 12500) {
      clk.drawString("" + String(millis()/1000) + "秒" + " 配网模式",120,40);
    }
    else if(buttonStateTime >= 9500) {
      clk.drawString("" + String(millis()/1000) + "秒" + " 动画:太空人",120,40);
    }
    else if(buttonStateTime >= 6500) {
      clk.drawString("" + String(millis()/1000) + "秒" + " 动画:龙猫转圈",120,40);
    }
    else if(buttonStateTime >= 3500) {
      clk.drawString("" + String(millis()/1000) + "秒" + " 动画:龙猫跳绳 图片待缩小",120,40);
    }
    else if(buttonStateTime >= 500) {
      clk.drawString("" + String(millis()/1000) + "秒" + " 动画:打乒乓",120,40);
    }
    clk.pushSprite(0,80);
    clk.deleteSprite();
    clk.unloadFont(); //释放加载字体资源
  }
  

  if(buttonStateTime >= 15500) { //打开/关闭启动广告
    guanggao_Flag = !guanggao_Flag;
    EEPROM.write(0,guanggao_Flag);
    EEPROM.commit();
    //Serial.println(EEPROM.read(0));
    clk.loadFont(ZdyLwFont_20);
    clk.createSprite(240, 80); 
    clk.setTextDatum(CC_DATUM);
    clk.setTextColor(TFT_WHITE, bgColor); 
    if(guanggao_Flag) {
      clk.drawString("启动广告已打开",120,40);
    }
    else {
      clk.drawString("启动广告已关闭",120,40);
    }
    clk.pushSprite(0,80);
    clk.deleteSprite();
    clk.unloadFont(); //释放加载字体资源
    delay(3000);
  } 
  else if(buttonStateTime >= 12500) { //配网模式
    //smartConfigWIFI();
    setWiFi();
  }
  else if(buttonStateTime >= 9500) { //动画-太空人
    EEPROM.write(1,4);
    EEPROM.commit();
  }
  else if(buttonStateTime >= 6500) { //动画-龙猫转圈
    EEPROM.write(1,3);
    EEPROM.commit();
  }
  else if(buttonStateTime >= 3500) { //动画-龙猫跳绳
    EEPROM.write(1,2);
    EEPROM.commit();
  }
  else if(buttonStateTime >= 500) { //动画-打乒乓
    EEPROM.write(1,1);
    EEPROM.commit();
  }

  Gif_Mode = EEPROM.read(1);

  //广告页
  /*
  if(guanggao_Flag == true) {
    TJpgDec.setJpgScale(1);
    TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(tft_output);
    TJpgDec.drawJpg(0,0,guanggao, sizeof(guanggao));
    clk.loadFont(ZdyLwFont_20);
    clk.createSprite(40, 30); 
    for(int i=10;i>=1;i--) {
      clk.fillSprite(bgColor);
      clk.setTextDatum(CC_DATUM);
      clk.setTextColor(TFT_BLACK, bgColor); 
      clk.drawString(String(i) + "秒",20,15);
      clk.pushSprite(200,210);
      delay(1000);
    }
    clk.deleteSprite();
    clk.unloadFont(); //释放加载字体资源
  }*/

  tft.fillScreen(0x0000);
  delay(100);
  tft.setTextColor(TFT_BLACK, bgColor);

  targetTime = millis() + 1000; 

  Serial.println("正在连接"+ PrefSSID + " ...");
  WiFi.begin(PrefSSID.c_str(), PrefPassword.c_str());
  //WiFi.begin("cktn", "18900744765");
  while (WiFi.status() != WL_CONNECTED) {
    for(byte n=0;n<10;n++){ 
      loading(100,1);
      connectTimes++;
      if(connectTimes >= 190) { //进度条即将结束时还未连接成功，则提示wifi连接失败，自动进入配网模式
        connectTimes = 0;
        displayConnectWifiFalse();
        //smartConfigWIFI();
        setWiFi();
      }
    }
  }
  while(loadNum < 194 & connectTimes <= 189){ //让动画走完
    loading(0,5);
    connectTimes = 0;
  }

  Serial.print("本地IP： ");
  Serial.println(WiFi.localIP());
  //Serial.println("启动UDP");
  Udp.begin(localPort);
  //Serial.print("端口号: ");
  //Serial.println(Udp.localPort());
  //Serial.println("等待同步...");
  setSyncProvider(getNtpTime);
  setSyncInterval(300);
 
  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);

  int x=0,y=0,dt=100,xyz=0;
  while(imgNum_1 <= 6 & xyz <= 0) {
    if(millis() - oldTime_1 >= dt) {
      imgNum_1 = imgNum_1 + 1;
      oldTime_1 = millis();
    }
    switch(imgNum_1) {
      case 1: TJpgDec.drawJpg(x,y,start_0, sizeof(start_0));break;
      case 2: TJpgDec.drawJpg(x,y,start_1, sizeof(start_1));break;
      case 3: TJpgDec.drawJpg(x,y,start_2, sizeof(start_2));break;
      case 4: TJpgDec.drawJpg(x,y,start_3, sizeof(start_3));break;
      case 5: TJpgDec.drawJpg(x,y,start_4, sizeof(start_4));break;
      case 6: TJpgDec.drawJpg(x,y,start_5, sizeof(start_5));imgNum_1 = 1;xyz++;break;
    } 
  }
  
  delay(1000);

  //TJpgDec.drawJpg(0,0,watchtop, sizeof(watchtop)); //顶部图片显示 240*20
  //TJpgDec.drawJpg(0,220,watchbottom, sizeof(watchbottom)); //底部图片显示 240*20
  
  //绘制一个视口
  //tft.setViewport(0, 20, 240, 240);
  tft.fillScreen(0x0000);
  tft.fillRoundRect(0,0,240,240,0,bgColor);//实心矩形
  //tft.resetViewport();

  //绘制线框
  tft.drawFastHLine(0,0,240,TFT_BLACK);
  //tft.drawFastHLine(0,220,240,TFT_BLACK);

  tft.drawFastHLine(0,34,240,TFT_BLACK);
  tft.drawFastHLine(0,200,240,TFT_BLACK);
  
  tft.drawFastVLine(150,0,34,TFT_BLACK);
  
  tft.drawFastHLine(0,166,240,TFT_BLACK);
  
  tft.drawFastVLine(60,166,34,TFT_BLACK);
  tft.drawFastVLine(160,166,34,TFT_BLACK);

  getCityCode();  //获取城市代码
  
  TJpgDec.drawJpg(165,171,temperature, sizeof(temperature));  //温度图标
  TJpgDec.drawJpg(163,140,humidity, sizeof(humidity));  //湿度图标
}

time_t prevDisplay = 0; // 显示时间
unsigned long weaterTime = 0;

float v1 = 2.0;
int time123 = 0;

#define FILTER_N 20
int Filter() {
  int i;
  int filter_sum = 0;
  int filter_max, filter_min;
  int filter_buf[FILTER_N];
  for(i = 0; i < FILTER_N; i++) {
    filter_buf[i] = analogRead(32);
    delay(1);
  }
  filter_max = filter_buf[0];
  filter_min = filter_buf[0];
  filter_sum = filter_buf[0];
  for(i = FILTER_N - 1; i > 0; i--) {
    if(filter_buf[i] > filter_max)
      filter_max=filter_buf[i];
    else if(filter_buf[i] < filter_min)
      filter_min=filter_buf[i];
    filter_sum = filter_sum + filter_buf[i];
    filter_buf[i] = filter_buf[i - 1];
  }
  i = FILTER_N - 2;
  filter_sum = filter_sum - filter_max - filter_min + i / 2; // +i/2 的目的是为了四舍五入
  filter_sum = filter_sum / i;
  return filter_sum;
}

void loop(){
  //屏幕背光控制，随光线强度变化 光线强-亮度高、光线弱-亮暗暗
  //输入的数据范围很大0-4095，输出的范围很小0-255 = 自带滤波，不会造成屏幕忽明忽暗
  //ledcWrite(0,map(analogRead(32),0,4095,0,255)); 
  //Serial.println(analogRead(32));

  Filter_Value = Filter();
  //ledcWrite(0,map(Filter_Value,0,4095,0,255));
  ledcWrite(0,3250);
  
  if(digitalRead(4) == HIGH) { 
    delay(500);
    ESP.restart(); //重启ESP32 
  }
  
  if (now() != prevDisplay) {
    prevDisplay = now();
    digitalClockDisplay();
  }

  //更新时，网络环境差的情况下，屏幕会有短暂停止刷新过程，网络环境好，该过程不明显，很难看出差别
  if(millis() - weaterTime > 600000){ //600000 1分钟更新一次天气
    getCityWeaterFlag = false;
    getCityCodeFlag = false;
    weaterTime = millis();
    getCityWeater();
  }
  scrollBanner();
  ButtonscrollBanner();
  imgDisplay();
}

void smartConfigWIFI()
{
  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);
  TJpgDec.drawJpg(0,0,wifi_config, sizeof(wifi_config)); //显示微信配网图片 
  WiFi.mode(WIFI_AP_STA);
  delay(100);
  WiFi.beginSmartConfig();
  Serial.println("配网中.");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  preferences.begin("wifi", false);
  preferences.putString( "ssid" , WiFi.SSID());
  preferences.putString( "password", WiFi.psk());
  preferences.end();

  Serial.println("配网完成，正在重启...");
  delay(2000);
  ESP.restart(); //重启ESP32
}

// 发送HTTP请求并且将服务器响应通过串口输出
void getCityCode(){
  //创建 HTTPClient 对象
  HTTPClient httpClient;
  while(getCityCodeFlag == false) {
    String URL = "http://wgeo.weather.com.cn/ip/?_="+String(now());
    
    //配置请求地址。此处也可以不使用端口号和PATH而单纯的
    httpClient.begin(URL); 
    
    //设置请求头中的User-Agent
    httpClient.setUserAgent("Mozilla/5.0 (iPhone; CPU iPhone OS 11_0 like Mac OS X) AppleWebKit/604.1.38 (KHTML, like Gecko) Version/11.0 Mobile/15A372 Safari/604.1");
    httpClient.addHeader("Referer", "http://www.weather.com.cn/");
  
    //启动连接并发送HTTP请求
    int httpCode = httpClient.GET();
    //Serial.print("Send GET request to URL: ");
    //Serial.println(URL);
    Serial.println("数据请求中...");
    
    //如果服务器响应OK则从服务器获取响应体信息并通过串口输出
    if (httpCode == HTTP_CODE_OK) {
      String str = httpClient.getString();
      //Serial.println(str); 
      int aa = str.indexOf("id=");
      if(aa>-1){
        cityCode = str.substring(aa+4,aa+4+9);
        //Serial.println(cityCode); 
        Serial.println("获取城市代码成功");
        getCityCodeFlag = true;  
        getCityWeater();
      }else{
        Serial.println("获取城市代码失败，正在重新获取...");  
      }
    } else {
      Serial.print("请求城市代码错误：");
      Serial.println(String(httpCode) + "正在重新获取...");
    }
  }
  //关闭ESP8266与服务器连接
  httpClient.end();
}

// 获取城市天气
/*
* 101250111 - 雨花区
* 101250106 - 长沙县
* 101250101 - 长沙
*/

void getCityWeater(){
  //cityCode = "101070201";
  HTTPClient httpClient;
  while(getCityWeaterFlag == false) {
    String URL = "http://d1.weather.com.cn/weather_index/" + cityCode + ".html?_="+String(now());
    //创建 HTTPClient 对象
    
    httpClient.begin(URL); 
    
    //设置请求头中的User-Agent
    httpClient.setUserAgent("Mozilla/5.0 (iPhone; CPU iPhone OS 11_0 like Mac OS X) AppleWebKit/604.1.38 (KHTML, like Gecko) Version/11.0 Mobile/15A372 Safari/604.1");
    httpClient.addHeader("Referer", "http://www.weather.com.cn/");
  
    //启动连接并发送HTTP请求
    int httpCode = httpClient.GET();
    Serial.println("正在获取天气数据");
    //Serial.println(URL);

    //如果服务器响应OK则从服务器获取响应体信息并通过串口输出
    if (httpCode == HTTP_CODE_OK) {

      String str = httpClient.getString();
      //Serial.println(str);

      int indexStart = str.indexOf("weatherinfo\":");
      int indexEnd = str.indexOf("};var alarmDZ");

      String jsonCityDZ = str.substring(indexStart+13,indexEnd);
      //Serial.println(jsonCityDZ);

      indexStart = str.indexOf("dataSK =");
      indexEnd = str.indexOf(";var dataZS");
      String jsonDataSK = str.substring(indexStart+8,indexEnd);
      //Serial.println(jsonDataSK);

      indexStart = str.indexOf("\"f\":[");
      indexEnd = str.indexOf(",{\"fa");
      String jsonFC = str.substring(indexStart+5,indexEnd);
      //Serial.println(jsonFC);

      indexStart = str.indexOf(";var dataZS ={\"zs\":");
      indexEnd = str.indexOf(",\"cn\":\"长沙\"};var fc =");
      String jsonSuggest = str.substring(indexStart+19,indexEnd);
      //Serial.println(jsonSuggest);
      
      weaterData(&jsonCityDZ,&jsonDataSK,&jsonFC,&jsonSuggest);
      Serial.println("天气数据获取成功");
      getCityWeaterFlag = true;
    } else {
      Serial.print("请求城市天气错误：");
      Serial.println(String(httpCode) + "正在重新获取...");
    }
  }
  //关闭ESP8266与服务器连接
  httpClient.end();
}

String scrollText[6];
String ButtonScrollText[8];
//int scrollTextWidth = 0;
//天气信息写到屏幕上
void weaterData(String *cityDZ,String *dataSK,String *dataFC,String *dataSuggest){
  
  DynamicJsonDocument doc(4096);
  deserializeJson(doc, *dataSK);
  JsonObject sk = doc.as<JsonObject>();

  //TFT_eSprite clkb = TFT_eSprite(&tft);
  
  /***绘制相关文字***/
  clk.setColorDepth(8);
  clk.loadFont(ZdyLwFont_20); //加载font/ZdyLwFont_20字体
  
  //温度
  clk.createSprite(54, 32); 
  clk.fillSprite(bgColor);
  clk.setTextDatum(CC_DATUM);
  clk.setTextColor(TFT_BLACK, bgColor); 
  clk.drawString(sk["temp"].as<String>()+"℃",27,16);
  clk.pushSprite(185,168);
  clk.deleteSprite();
 
  //城市名称
  clk.createSprite(88, 32);  //88,32
  clk.fillSprite(bgColor);
  clk.setTextDatum(CC_DATUM);
  clk.setTextColor(TFT_BLACK, bgColor); 
  //clk.drawString(sk["cityname"].as<String>()+"区",44,18);
  clk.drawString(sk["cityname"].as<String>(),44,18);
  clk.pushSprite(151,1);
  clk.deleteSprite();
  
  //PM2.5空气指数
  uint16_t pm25BgColor;//优
  String aqiTxt;
  int pm25V = sk["aqi"];
  //Serial.println("pm25V:" + String(pm25V));
  if(pm25V >= 301) {
    pm25BgColor = tft.color565(255,36,0);//重度
    aqiTxt = "严重";
  }else if(pm25V >= 201 & pm25V <= 300){
    pm25BgColor = tft.color565(136,11,32);//重度
    aqiTxt = "重度";
  }else if(pm25V >= 151 & pm25V <= 200){
    pm25BgColor = tft.color565(186,55,121);//中度
    aqiTxt = "中度";
  }else if(pm25V >= 101 & pm25V <= 160){
    pm25BgColor = tft.color565(242,159,57);//轻
    aqiTxt = "轻度";
  }else if(pm25V >= 51 & pm25V <= 100){
    pm25BgColor = tft.color565(247,219,100);//良
    aqiTxt = "良";
  }else if(pm25V >= 0 & pm25V <= 50) {
    pm25BgColor = tft.color565(156,202,127);//优
    aqiTxt = "优";
  }
  clk.createSprite(50, 24); 
  clk.fillSprite(bgColor);
  clk.fillRoundRect(0,0,50,24,4,pm25BgColor);
  clk.setTextDatum(CC_DATUM);
  clk.setTextColor(0xFFFF); 
  clk.drawString(aqiTxt,25,13);
  clk.pushSprite(5,140);
  clk.deleteSprite();

  //湿度
  clk.createSprite(56, 24); 
  clk.fillSprite(bgColor);
  clk.setTextDatum(CC_DATUM);
  clk.setTextColor(TFT_BLACK, bgColor); 
  clk.drawString(sk["SD"].as<String>(),28,13);
  //clk.drawString("100%",28,13);
  clk.pushSprite(185,140);
  clk.deleteSprite();

  //左上角滚动字幕
  //解析第二段JSON
  scrollText[0] = "实时天气 "+sk["weather"].as<String>();
  scrollText[1] = "空气质量 "+aqiTxt;
  
  scrollText[2] = "风向 "+sk["WD"].as<String>()+sk["WS"].as<String>();
  
  deserializeJson(doc, *cityDZ);
  JsonObject dz = doc.as<JsonObject>();
  scrollText[3] = "今日 "+dz["weather"].as<String>();

  //显示天气图标
  String weatherCodeText = dz["weathercode"].as<String>();
  weatherCode = weatherCodeText.substring(1,weatherCodeText.length()+1).toInt();
  //Serial.println(weatherCode);
  switch(weatherCode) {
    case 0:TJpgDec.drawJpg(10,105,d00_40X30, sizeof(d00_40X30));break;
    case 1:TJpgDec.drawJpg(10,105,d01_40X30, sizeof(d01_40X30));break;
    case 2:TJpgDec.drawJpg(10,105,d02_40X30, sizeof(d02_40X30));break;
    case 3:TJpgDec.drawJpg(10,105,d03_40X30, sizeof(d03_40X30));break;
    case 4:TJpgDec.drawJpg(10,105,d04_40X30, sizeof(d04_40X30));break;
    case 5:TJpgDec.drawJpg(10,105,d05_40X30, sizeof(d05_40X30));break;
    case 6:TJpgDec.drawJpg(10,105,d06_40X30, sizeof(d06_40X30));break;
    case 7:TJpgDec.drawJpg(10,105,d07_40X30, sizeof(d07_40X30));break;
    case 8:TJpgDec.drawJpg(10,105,d08_40X30, sizeof(d08_40X30));break;
    case 9:TJpgDec.drawJpg(10,105,d09_40X30, sizeof(d09_40X30));break;
    case 10:TJpgDec.drawJpg(10,105,d10_40X30, sizeof(d10_40X30));break;
    case 11:TJpgDec.drawJpg(10,105,d11_40X30, sizeof(d11_40X30));break;
    case 12:TJpgDec.drawJpg(10,105,d12_40X30, sizeof(d12_40X30));break;
    case 13:TJpgDec.drawJpg(10,105,d13_40X30, sizeof(d13_40X30));break;
    case 14:TJpgDec.drawJpg(10,105,d14_40X30, sizeof(d14_40X30));break;
    case 15:TJpgDec.drawJpg(10,105,d15_40X30, sizeof(d15_40X30));break;
    case 16:TJpgDec.drawJpg(10,105,d16_40X30, sizeof(d16_40X30));break;
    case 17:TJpgDec.drawJpg(10,105,d17_40X30, sizeof(d17_40X30));break;
    case 18:TJpgDec.drawJpg(10,105,d18_40X30, sizeof(d18_40X30));break;
    case 19:TJpgDec.drawJpg(10,105,d19_40X30, sizeof(d19_40X30));break;
    case 20:TJpgDec.drawJpg(10,105,d20_40X30, sizeof(d20_40X30));break;
    case 21:TJpgDec.drawJpg(10,105,d21_40X30, sizeof(d21_40X30));break;
    case 22:TJpgDec.drawJpg(10,105,d22_40X30, sizeof(d22_40X30));break;
    case 23:TJpgDec.drawJpg(10,105,d23_40X30, sizeof(d23_40X30));break;
    case 24:TJpgDec.drawJpg(10,105,d24_40X30, sizeof(d24_40X30));break;
    case 25:TJpgDec.drawJpg(10,105,d25_40X30, sizeof(d25_40X30));break;
    case 26:TJpgDec.drawJpg(10,105,d26_40X30, sizeof(d26_40X30));break;
    case 27:TJpgDec.drawJpg(10,105,d27_40X30, sizeof(d27_40X30));break;
    case 28:TJpgDec.drawJpg(10,105,d28_40X30, sizeof(d28_40X30));break;
    case 29:TJpgDec.drawJpg(10,105,d29_40X30, sizeof(d29_40X30));break;
    case 30:TJpgDec.drawJpg(10,105,d30_40X30, sizeof(d30_40X30));break;
    case 31:TJpgDec.drawJpg(10,105,d31_40X30, sizeof(d31_40X30));break;
    case 32:TJpgDec.drawJpg(10,105,d32_40X30, sizeof(d32_40X30));break;
    case 33:TJpgDec.drawJpg(10,105,d33_40X30, sizeof(d33_40X30));break;
    case 49:TJpgDec.drawJpg(10,105,d49_40X30, sizeof(d49_40X30));break;
    case 53:TJpgDec.drawJpg(10,105,d53_40X30, sizeof(d53_40X30));break;
    case 54:TJpgDec.drawJpg(10,105,d54_40X30, sizeof(d54_40X30));break;
    case 55:TJpgDec.drawJpg(10,105,d55_40X30, sizeof(d55_40X30));break;
    case 56:TJpgDec.drawJpg(10,105,d56_40X30, sizeof(d56_40X30));break;
    case 57:TJpgDec.drawJpg(10,105,d57_40X30, sizeof(d57_40X30));break;
    case 58:TJpgDec.drawJpg(10,105,d58_40X30, sizeof(d58_40X30));break;
    case 301:TJpgDec.drawJpg(10,105,d301_40X30, sizeof(d301_40X30));break;
    case 302:TJpgDec.drawJpg(10,105,d302_40X30, sizeof(d302_40X30));break;
    default:break;
  }
  
  deserializeJson(doc, *dataFC);
  JsonObject fc = doc.as<JsonObject>();
  
  scrollText[4] = "最低温度 "+fc["fd"].as<String>()+"℃";
  scrollText[5] = "最高温度 "+fc["fc"].as<String>()+"℃";

  //scrollText[6] = "PM2.5 "+sk["aqi"].as<String>();
  
  //Serial.println(scrollText[0]);
  clk.unloadFont(); //释放加载字体资源

  deserializeJson(doc, *dataSuggest);
  JsonObject dataSuggestJson = doc.as<JsonObject>();
  ButtonScrollText[0] = dataSuggestJson["lk_name"].as<String>() + " " + dataSuggestJson["lk_hint"].as<String>();
  ButtonScrollText[1] = dataSuggestJson["cl_name"].as<String>() + " " + dataSuggestJson["cl_hint"].as<String>();
  ButtonScrollText[2] = dataSuggestJson["uv_name"].as<String>() + " " + dataSuggestJson["uv_hint"].as<String>();
  ButtonScrollText[3] = dataSuggestJson["ct_name"].as<String>() + " " + dataSuggestJson["ct_hint"].as<String>();
  ButtonScrollText[4] = dataSuggestJson["gm_name"].as<String>() + " " + dataSuggestJson["gm_hint"].as<String>();
  ButtonScrollText[5] = dataSuggestJson["ys_name"].as<String>() + " " + dataSuggestJson["ys_hint"].as<String>();
  //ButtonScrollText[6] = dataSuggestJson["gz_name"].as<String>() + " " + dataSuggestJson["gz_hint"].as<String>();
  //ButtonScrollText[6] = dataSuggestJson["cl_name"].as<String>() + " " + dataSuggestJson["cl_hint"].as<String>();
  ButtonScrollText[6] = dataSuggestJson["pl_name"].as<String>() + " " + dataSuggestJson["pl_hint"].as<String>();
  ButtonScrollText[7] = dataSuggestJson["co_name"].as<String>() + " " + dataSuggestJson["co_hint"].as<String>();
}

int currentIndex = 0;
int prevTime = 0;
TFT_eSprite clkb = TFT_eSprite(&tft);

void scrollBanner(){
  if(millis() - prevTime > 3500){ //3.5秒切换一次

    if(scrollText[currentIndex]){
  
      clkb.setColorDepth(8);
      clkb.loadFont(ZdyLwFont_20);
      
      for(int pos = 20; pos>0 ; pos--){
        scrollTxt(pos);
      }
      
      clkb.deleteSprite();
      clkb.unloadFont();
  
      if(currentIndex>=5){
        currentIndex = 0;  //回第一个
      }else{
        currentIndex += 1;  //准备切换到下一个  
      }
      
      //Serial.println(currentIndex);
      
    }
    prevTime = millis();
    
  }
}

void scrollTxt(int pos){
    clkb.createSprite(148, 24); 
    clkb.fillSprite(bgColor);
    clkb.setTextWrap(false);
    clkb.setTextDatum(CC_DATUM);
    clkb.setTextColor(TFT_BLACK, bgColor); 
    clkb.drawString(scrollText[currentIndex],74,pos+14);
    clkb.pushSprite(2,4);
}

/**
*底部生活信息滚动显示 
*/

byte ButtoncurrentIndex = 0;
unsigned long ButtonprevTime = 0;
TFT_eSprite clkbb = TFT_eSprite(&tft);

void ButtonscrollBanner(){
  if(millis() - ButtonprevTime > 5000){ //5秒切换一次

    if(ButtonScrollText[ButtoncurrentIndex]){
      clkbb.loadFont(ZdyLwFont_20);
      
      for(int pos = 20; pos>0 ; pos--){
        ButtonScrollTxt(pos);
      }
      
      clkbb.deleteSprite();
      clkbb.unloadFont();
  
      if(ButtoncurrentIndex>=7){
        ButtoncurrentIndex = 0;  //回第一个
      }else{
        ButtoncurrentIndex += 1;  //准备切换到下一个  
      }
      
      //Serial.println(ButtoncurrentIndex);
      
    }
    ButtonprevTime = millis();
    
  }
}

void ButtonScrollTxt(int pos){
  //clkbb.loadFont(ZdyLwFont_20);
  clkbb.createSprite(240, 40); 
  clkbb.fillSprite(bgColor);
  clkbb.setTextDatum(CC_DATUM);
  clkbb.setTextColor(TFT_BLACK, bgColor); 
  clkbb.drawString(ButtonScrollText[ButtoncurrentIndex],120,pos+20);
  clkbb.pushSprite(0,201);
  //clkbb.deleteSprite();
  //clkbb.unloadFont(); //释放加载字体资源
}

unsigned long oldTime = 0,imgNum = 1;
void imgDisplay(){
  int x=75,y=94,dt;
  switch(Gif_Mode) { //修改动画的播放速度
    case 1:dt = 100;break;
    case 2:dt = 50;break;
    case 3:dt = 100;break;
    case 4:dt = 100;break;
  }
  if(millis() - oldTime >= dt) {
    imgNum = imgNum + 1;
    oldTime = millis();
  }
  if(Gif_Mode == 3) { //动画-龙猫转圈
    switch(imgNum) {
      case 1: TJpgDec.drawJpg(x,y,img_0, sizeof(img_0));break;
      case 2: TJpgDec.drawJpg(x,y,img_1, sizeof(img_1));break;
      case 3: TJpgDec.drawJpg(x,y,img_2, sizeof(img_2));break;
      case 4: TJpgDec.drawJpg(x,y,img_3, sizeof(img_3));break;
      case 5: TJpgDec.drawJpg(x,y,img_4, sizeof(img_4));break;
      case 6: TJpgDec.drawJpg(x,y,img_5, sizeof(img_5));break;
      case 7: TJpgDec.drawJpg(x,y,img_6, sizeof(img_6));break;
      case 8: TJpgDec.drawJpg(x,y,img_7, sizeof(img_7));break;
      case 9: TJpgDec.drawJpg(x,y,img_8, sizeof(img_8));break;
      case 10: TJpgDec.drawJpg(x,y,img_9, sizeof(img_9));break;
      case 11: TJpgDec.drawJpg(x,y,img_10, sizeof(img_10));break;
      case 12: TJpgDec.drawJpg(x,y,img_11, sizeof(img_11));break;
      case 13: TJpgDec.drawJpg(x,y,img_12, sizeof(img_12));break;
      case 14: TJpgDec.drawJpg(x,y,img_13, sizeof(img_13));break;
      case 15: TJpgDec.drawJpg(x,y,img_14, sizeof(img_14));break;
      case 16: TJpgDec.drawJpg(x,y,img_15, sizeof(img_15));break;
      case 17: TJpgDec.drawJpg(x,y,img_16, sizeof(img_16));break;
      case 18: TJpgDec.drawJpg(x,y,img_17, sizeof(img_17));break;
      case 19: TJpgDec.drawJpg(x,y,img_18, sizeof(img_18));break;
      case 20: TJpgDec.drawJpg(x,y,img_19, sizeof(img_19));break;
      case 21: TJpgDec.drawJpg(x,y,img_20, sizeof(img_20));break;
      case 22: TJpgDec.drawJpg(x,y,img_21, sizeof(img_21));break;
      case 23: TJpgDec.drawJpg(x,y,img_22, sizeof(img_22));break;
      case 24: TJpgDec.drawJpg(x,y,img_23, sizeof(img_23));break;
      case 25: TJpgDec.drawJpg(x,y,img_24, sizeof(img_24));break;
      case 26: TJpgDec.drawJpg(x,y,img_25, sizeof(img_25));break;
      case 27: TJpgDec.drawJpg(x,y,img_26, sizeof(img_26));break;
      case 28: TJpgDec.drawJpg(x,y,img_27, sizeof(img_27));break;
      case 29: TJpgDec.drawJpg(x,y,img_28, sizeof(img_28));break;
      case 30: TJpgDec.drawJpg(x,y,img_29, sizeof(img_29));break;
      case 31: TJpgDec.drawJpg(x,y,img_30, sizeof(img_30));break;
      case 32: TJpgDec.drawJpg(x,y,img_31, sizeof(img_31));break;
      case 33: TJpgDec.drawJpg(x,y,img_32, sizeof(img_32));break;
      case 34: TJpgDec.drawJpg(x,y,img_33, sizeof(img_33));break;
      case 35: TJpgDec.drawJpg(x,y,img_34, sizeof(img_34));break;
      case 36: TJpgDec.drawJpg(x,y,img_35, sizeof(img_35));break;
      case 37: TJpgDec.drawJpg(x,y,img_36, sizeof(img_36));break;
      case 38: TJpgDec.drawJpg(x,y,img_37, sizeof(img_37));break;
      case 39: TJpgDec.drawJpg(x,y,img_38, sizeof(img_38));break;
      case 40: TJpgDec.drawJpg(x,y,img_39, sizeof(img_39));break;
      case 41: TJpgDec.drawJpg(x,y,img_40, sizeof(img_40));break;
      case 42: TJpgDec.drawJpg(x,y,img_41, sizeof(img_41));break;
      case 43: TJpgDec.drawJpg(x,y,img_42, sizeof(img_42));break;
      case 44: TJpgDec.drawJpg(x,y,img_43, sizeof(img_43));break;
      case 45: TJpgDec.drawJpg(x,y,img_44, sizeof(img_44));break;
      case 46: TJpgDec.drawJpg(x,y,img_45, sizeof(img_45));break;
      case 47: TJpgDec.drawJpg(x,y,img_46, sizeof(img_46));break;
      case 48: TJpgDec.drawJpg(x,y,img_47, sizeof(img_47));break;
      case 49: TJpgDec.drawJpg(x,y,img_48, sizeof(img_48));break;
      case 50: TJpgDec.drawJpg(x,y,img_49, sizeof(img_49));break;
      case 51: TJpgDec.drawJpg(x,y,img_50, sizeof(img_50));break;
      case 52: TJpgDec.drawJpg(x,y,img_51, sizeof(img_51));break;
      case 53: TJpgDec.drawJpg(x,y,img_52, sizeof(img_52));break;
      case 54: TJpgDec.drawJpg(x,y,img_53, sizeof(img_53));break;
      case 55: TJpgDec.drawJpg(x,y,img_54, sizeof(img_54));break;
      case 56: TJpgDec.drawJpg(x,y,img_55, sizeof(img_55));break;
      case 57: TJpgDec.drawJpg(x,y,img_56, sizeof(img_56));break;
      case 58: TJpgDec.drawJpg(x,y,img_57, sizeof(img_57));break;
      case 59: TJpgDec.drawJpg(x,y,img_58, sizeof(img_58));break;
      case 60: TJpgDec.drawJpg(x,y,img_59, sizeof(img_59));break;
      case 61: TJpgDec.drawJpg(x,y,img_60, sizeof(img_60));break;
      case 62: TJpgDec.drawJpg(x,y,img_61, sizeof(img_61));break;
      case 63: TJpgDec.drawJpg(x,y,img_62, sizeof(img_62));break;
      case 64: TJpgDec.drawJpg(x,y,img_63, sizeof(img_63));break;
      case 65: TJpgDec.drawJpg(x,y,img_64, sizeof(img_64));break;
      case 66: TJpgDec.drawJpg(x,y,img_65, sizeof(img_65));break;
      case 67: TJpgDec.drawJpg(x,y,img_66, sizeof(img_66));break;
      case 68: TJpgDec.drawJpg(x,y,img_67, sizeof(img_67));break;
      case 69: TJpgDec.drawJpg(x,y,img_68, sizeof(img_68));break;
      case 70: TJpgDec.drawJpg(x,y,img_69, sizeof(img_69));break;
      case 71: TJpgDec.drawJpg(x,y,img_70, sizeof(img_70));break;
      case 72: TJpgDec.drawJpg(x,y,img_71, sizeof(img_71));break;
      case 73: TJpgDec.drawJpg(x,y,img_72, sizeof(img_72));break;
      case 74: TJpgDec.drawJpg(x,y,img_73, sizeof(img_73));break;
      case 75: TJpgDec.drawJpg(x,y,img_74, sizeof(img_74));break;
      case 76: TJpgDec.drawJpg(x,y,img_75, sizeof(img_75));break;
      case 77: TJpgDec.drawJpg(x,y,img_76, sizeof(img_76));break;
      case 78: TJpgDec.drawJpg(x,y,img_77, sizeof(img_77));break;
      case 79: TJpgDec.drawJpg(x,y,img_78, sizeof(img_78));break;
      case 80: TJpgDec.drawJpg(x,y,img_79, sizeof(img_79));imgNum=1;break;
    }
  }
  else if(Gif_Mode == 4) { //动画-太空人
    switch(imgNum) {
        case 1: TJpgDec.drawJpg(x,y,i0, sizeof(i0));break;
        case 2: TJpgDec.drawJpg(x,y,i1, sizeof(i1));break;
        case 3: TJpgDec.drawJpg(x,y,i2, sizeof(i2));break;
        case 4: TJpgDec.drawJpg(x,y,i3, sizeof(i3));break;
        case 5: TJpgDec.drawJpg(x,y,i4, sizeof(i4));break;
        case 6: TJpgDec.drawJpg(x,y,i5, sizeof(i5));break;
        case 7: TJpgDec.drawJpg(x,y,i6, sizeof(i6));break;
        case 8: TJpgDec.drawJpg(x,y,i7, sizeof(i7));break;
        case 9: TJpgDec.drawJpg(x,y,i8, sizeof(i8));break;
        case 10: TJpgDec.drawJpg(x,y,i9, sizeof(i9));imgNum=1;break;
      }
  }
  else if(Gif_Mode == 1) { //动画-打乒乓
    switch(imgNum) {
      case 1: TJpgDec.drawJpg(x,y,pingpang_0, sizeof(pingpang_0));break;
      case 2: TJpgDec.drawJpg(x,y,pingpang_1, sizeof(pingpang_1));break;
      case 3: TJpgDec.drawJpg(x,y,pingpang_2, sizeof(pingpang_2));break;
      case 4: TJpgDec.drawJpg(x,y,pingpang_3, sizeof(pingpang_3));break;
      case 5: TJpgDec.drawJpg(x,y,pingpang_4, sizeof(pingpang_4));break;
      case 6: TJpgDec.drawJpg(x,y,pingpang_5, sizeof(pingpang_5));break;
      case 7: TJpgDec.drawJpg(x,y,pingpang_6, sizeof(pingpang_6));break;
      case 8: TJpgDec.drawJpg(x,y,pingpang_7, sizeof(pingpang_7));break;
      case 9: TJpgDec.drawJpg(x,y,pingpang_8, sizeof(pingpang_8));break;
      case 10: TJpgDec.drawJpg(x,y,pingpang_9, sizeof(pingpang_9));break;
      case 11: TJpgDec.drawJpg(x,y,pingpang_10, sizeof(pingpang_10));break;
      case 12: TJpgDec.drawJpg(x,y,pingpang_12, sizeof(pingpang_12));break;
      case 13: TJpgDec.drawJpg(x,y,pingpang_13, sizeof(pingpang_13));break;
      case 14: TJpgDec.drawJpg(x,y,pingpang_14, sizeof(pingpang_14));break;
      case 15: TJpgDec.drawJpg(x,y,pingpang_15, sizeof(pingpang_15));break;
      case 16: TJpgDec.drawJpg(x,y,pingpang_16, sizeof(pingpang_16));break;
      case 17: TJpgDec.drawJpg(x,y,pingpang_17, sizeof(pingpang_17));break;
      case 18: TJpgDec.drawJpg(x,y,pingpang_18, sizeof(pingpang_18));break;
      case 19: TJpgDec.drawJpg(x,y,pingpang_19, sizeof(pingpang_19));break;
      case 20: TJpgDec.drawJpg(x,y,pingpang_20, sizeof(pingpang_20));break;
      case 21: TJpgDec.drawJpg(x,y,pingpang_21, sizeof(pingpang_21));break;
      case 22: TJpgDec.drawJpg(x,y,pingpang_22, sizeof(pingpang_22));break;
      case 23: TJpgDec.drawJpg(x,y,pingpang_23, sizeof(pingpang_23));break;
      case 24: TJpgDec.drawJpg(x,y,pingpang_24, sizeof(pingpang_24));break;
      case 25: TJpgDec.drawJpg(x,y,pingpang_25, sizeof(pingpang_25));break;
      case 26: TJpgDec.drawJpg(x,y,pingpang_26, sizeof(pingpang_26));break;
      case 27: TJpgDec.drawJpg(x,y,pingpang_27, sizeof(pingpang_27));imgNum=1;break;
    }
  }
  else if(Gif_Mode == 2) { //动画-龙猫跳绳
    switch(imgNum) {
      case 1: TJpgDec.drawJpg(x,84,quan_0, sizeof(quan_0));break;
      case 2: TJpgDec.drawJpg(x,84,quan_1, sizeof(quan_1));break;
      case 3: TJpgDec.drawJpg(x,84,quan_2, sizeof(quan_2));break;
      case 4: TJpgDec.drawJpg(x,84,quan_3, sizeof(quan_3));break;
      case 5: TJpgDec.drawJpg(x,84,quan_4, sizeof(quan_4));break;
      case 6: TJpgDec.drawJpg(x,84,quan_5, sizeof(quan_5));break;
      case 7: TJpgDec.drawJpg(x,84,quan_6, sizeof(quan_6));break;
      case 8: TJpgDec.drawJpg(x,84,quan_7, sizeof(quan_7));break;
      case 9: TJpgDec.drawJpg(x,84,quan_8, sizeof(quan_8));break;
      case 10: TJpgDec.drawJpg(x,84,quan_9, sizeof(quan_9));break;
      case 11: TJpgDec.drawJpg(x,84,quan_10, sizeof(quan_10));break;
      case 12: TJpgDec.drawJpg(x,84,quan_11, sizeof(quan_11));break;
      case 13: TJpgDec.drawJpg(x,84,quan_12, sizeof(quan_12));break;
      case 14: TJpgDec.drawJpg(x,84,quan_13, sizeof(quan_13));break;
      case 15: TJpgDec.drawJpg(x,84,quan_14, sizeof(quan_14));break;
      case 16: TJpgDec.drawJpg(x,84,quan_15, sizeof(quan_15));break;
      case 17: TJpgDec.drawJpg(x,84,quan_16, sizeof(quan_16));break;
      case 18: TJpgDec.drawJpg(x,84,quan_17, sizeof(quan_17));break;
      case 19: TJpgDec.drawJpg(x,84,quan_18, sizeof(quan_18));break;
      case 20: TJpgDec.drawJpg(x,84,quan_19, sizeof(quan_19));break;
      case 21: TJpgDec.drawJpg(x,84,quan_20, sizeof(quan_20));break;
      case 22: TJpgDec.drawJpg(x,84,quan_21, sizeof(quan_21));break;
      case 23: TJpgDec.drawJpg(x,84,quan_22, sizeof(quan_22));break;
      case 24: TJpgDec.drawJpg(x,84,quan_23, sizeof(quan_23));break;
      case 25: TJpgDec.drawJpg(x,84,quan_24, sizeof(quan_24));break;
      case 26: TJpgDec.drawJpg(x,84,quan_25, sizeof(quan_25));break;
      case 27: TJpgDec.drawJpg(x,84,quan_26, sizeof(quan_26));break;
      case 28: TJpgDec.drawJpg(x,84,quan_27, sizeof(quan_27));break;
      case 29: TJpgDec.drawJpg(x,84,quan_28, sizeof(quan_28));break;
      case 30: TJpgDec.drawJpg(x,84,quan_29, sizeof(quan_29));break;
      case 31: TJpgDec.drawJpg(x,84,quan_30, sizeof(quan_30));break;
      case 32: TJpgDec.drawJpg(x,84,quan_31, sizeof(quan_31));break;
      case 33: TJpgDec.drawJpg(x,84,quan_32, sizeof(quan_32));break;
      case 34: TJpgDec.drawJpg(x,84,quan_33, sizeof(quan_33));break;
      case 35: TJpgDec.drawJpg(x,84,quan_34, sizeof(quan_34));break;
      case 36: TJpgDec.drawJpg(x,84,quan_35, sizeof(quan_35));break;
      case 37: TJpgDec.drawJpg(x,84,quan_36, sizeof(quan_36));break;
      case 38: TJpgDec.drawJpg(x,84,quan_37, sizeof(quan_37));break;
      case 39: TJpgDec.drawJpg(x,84,quan_38, sizeof(quan_38));break;
      case 40: TJpgDec.drawJpg(x,84,quan_39, sizeof(quan_39));imgNum=1;break;
    }
  }
}

void digitalClockDisplay()
{
  
  clk.setColorDepth(8);

  /***中间时间区***/
  //时分
  clk.createSprite(140, 48);
  clk.fillSprite(bgColor);
  //clk.loadFont(FxLED_48);
  clk.setTextDatum(CC_DATUM);
  clk.setTextColor(TFT_BLACK, bgColor);
  clk.drawString(hourMinute(),70,24,7); //绘制时和分
  //clk.unloadFont();
  clk.pushSprite(28,40);
  clk.deleteSprite();
  
  //秒
  clk.createSprite(40, 32);
  clk.fillSprite(bgColor);
  
  clk.loadFont(FxLED_32);
  clk.setTextDatum(CC_DATUM);
  clk.setTextColor(TFT_BLACK, bgColor); 
  clk.drawString(num2str(second()),20,16);
  
  clk.unloadFont();
  clk.pushSprite(170,60);
  clk.deleteSprite();
  /***中间时间区***/

  /***底部***/
  clk.loadFont(ZdyLwFont_20);
  clk.createSprite(58, 32);
  clk.fillSprite(bgColor);

  //星期
  clk.setTextDatum(CC_DATUM);
  clk.setTextColor(TFT_BLACK, bgColor);
  clk.drawString(week(),29,16);
  clk.pushSprite(1,168);
  clk.deleteSprite();
  
  //月日
  clk.createSprite(98, 32);
  clk.fillSprite(bgColor);
  clk.setTextDatum(CC_DATUM);
  clk.setTextColor(TFT_BLACK, bgColor);  
  clk.drawString(monthDay(),49,16);
  clk.pushSprite(61,168);
  clk.deleteSprite();
  
  clk.unloadFont();
  /***底部***/

  
}

//星期
String week(){
  String wk[7] = {"日","一","二","三","四","五","六"};
  String s = "周" + wk[weekday()-1];
  return s;
}

//月日
String monthDay(){
  String s = String(month());
  s = s + "月" + day() + "日";
  return s;
}
//时分
String hourMinute(){
  String s = num2str(hour());
  backLight_hour = s.toInt();
  s = s + ":" + num2str(minute());
  return s;
}

String num2str(int digits)
{
  String s = "";
  if (digits < 10)
    s = s + "0";
  s = s + digits;
  return s;
}

void printDigits(int digits)
{
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP时间在消息的前48字节中
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  //Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  //Serial.print(ntpServerName);
  //Serial.print(": ");
  //Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      //Serial.println(secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR);
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // 无法获取时间时返回0
}

// 向NTP服务器发送请求
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
