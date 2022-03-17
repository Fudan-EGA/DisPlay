void setup() {
  // put your setup code here, to run once:
  ledcSetup(8, 1000, 10);  //设置LEDC通道8频率为1000，分辨率为10位，即占空比可选0~1023
  ledcAttachPin(15, 8); //设置LEDC通道8在IO上输出
  //ledcWrite(8, 512); //设置输出PWM占空比

  pinMode(14, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(14)==LOW){
    ledcWrite(8, 128); 
  }
  else{
    ledcWrite(8, 0); 
  }
  
}
