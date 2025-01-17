#include "cy8cmbr3116.h"
#include "Wire.h"
#include "wiring_private.h"
#include "Keyboard.h"


#define BTN1 A0
#define BTN2 A1
#define BTN3 A2
#define BTN4 A3
#define BTN5 A4
#define BTN6 A5 // D8
#define BTN7 9  // D9
#define BTN8 1  // D1

#define Serial SerialUSB
class touchblock {
  public:
    uint8_t sensid;
    uint8_t portid;
    uint16_t thres;
};
touchblock touchmap[34] = {
  // Group A
  {0, 6, 150}, //1
  {0, 10, 150}, //2
  {0, 15, 150}, //3
  {1, 7, 150}, //4
  {1, 11, 150}, //5
  {1, 15, 150}, //6
  {2, 8, 150}, //7
  {2, 12, 150}, //8
  //Group B
  {0, 7, 150}, //1
  {0, 11, 150}, //2
  {0, 3, 150}, //3
  {1, 8, 150}, //4
  {1, 12, 150}, //5
  {1, 3, 150}, //6
  {2, 9, 150}, //7
  {2, 13,150}, //8
  //Group C
  {0, 14, 40}, //1
  {2, 7, 40}, //2
  //Group D
  {2, 14, 150}, //1
  {0, 8, 150}, //2
  {0, 12, 150}, //3
  {0, 2, 150}, //4
  {1, 9, 150}, //5
  {1, 13, 150}, //6
  {1, 2, 150}, //7
  {2, 10, 150}, //8
  //Group E
  {2, 15, 150}, //1
  {0, 9, 150}, //2
  {0, 13, 150}, //3
  {1, 6, 150}, //4
  {1, 10, 150}, //5
  {1, 14, 150}, //6
  {2, 6, 150}, //7
  {2, 11, 150}, //8
};

//#define RUNLED 7

CY8CMBR3116 Touch[3];
TwoWire WireTouchA = Wire; // D21-SCL | D20-SDA

enum {
  commandRSET  = 0x45,//E
  commandHALT  = 0x4C,//L
  commandSTAT  = 0x41,//A
  commandRatio = 0x72,//r
  commandSens  = 0x6B,//k
};
uint8_t packet[6];
bool Conditioning = true;

void setup() {
  Serial.begin(9600);
  //pinMode(RUNLED, OUTPUT);
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);
  pinMode(BTN5, INPUT_PULLUP);
  pinMode(BTN6, INPUT_PULLUP);
  pinMode(BTN7, INPUT_PULLUP);
  pinMode(BTN8, INPUT_PULLUP);
  if (digitalRead(BTN1) == HIGH &&
      digitalRead(BTN2) == HIGH &&
      digitalRead(BTN3) == HIGH &&
      digitalRead(BTN4) == HIGH &&
      digitalRead(BTN5) == HIGH &&
      digitalRead(BTN6) == HIGH &&
      digitalRead(BTN7) == HIGH &&
      digitalRead(BTN8) == HIGH) 
  {
        delay(3000);
        Serial.println("未插入");
        while(true){}
  }
  Serial.setTimeout(0);
  Wire.begin();
  Wire.setClock(800000);
  delay(2000);
  
  int retry=0;
  for(retry=0;retry<10;retry++)
  {
      int error;
      Wire.beginTransmission(0x37);
      error = Wire.endTransmission();
      Serial.println(error);
      if(error == 0) {
          
          if (!Touch[0].begin(0x37)) {
            Serial.println("Failed to initialize touch sensor!");
            while (1) {
              delay(10);  // 停止程序运行
            }
          }
          Serial.println("Touch sensor initialized.");
          break;
      }
      else{
        Serial.println("I2C_0 连接失败,正在重试");
      }
      delay(200);
      Serial.print("重试次数:");
      Serial.println(retry);
  }
  Serial.print("重试次数:");
  Serial.println(retry);
  if(retry == 10){
      Serial.println("连接失败！");
      while (1) {
            delay(10);  // 停止程序运行
      }
  }
  Serial.println("I2C_0 连接成功！");

  for(retry=0;retry<10;retry++)
  {
      int error;
      Wire.beginTransmission(0x38);
      error = Wire.endTransmission();
      Serial.println(error);
      if(error == 0) {
          
          if (!Touch[1].begin(0x38)) {
            Serial.println("Failed to initialize touch sensor!");
            while (1) {
              delay(10);  // 停止程序运行
            }
          }
          Serial.println("Touch sensor initialized.");
          break;
      }
      else{
        Serial.println("I2C_1 连接失败,正在重试");
      }
      delay(200);
      Serial.print("重试次数:");
      Serial.println(retry);
  }
  Serial.print("重试次数:");
  Serial.println(retry);
  if(retry == 10){
      Serial.println("连接失败！");
      while (1) {
            delay(10);  // 停止程序运行
      }
  }
  Serial.println("I2C_1 连接成功！");
  
  for(retry=0;retry<10;retry++)
  {
      int error;
      Wire.beginTransmission(0x39);
      error = Wire.endTransmission();
      Serial.println(error);
      if(error == 0) {
          
          if (!Touch[2].begin(0x39)) {
            Serial.println("Failed to initialize touch sensor!");
            while (1) {
              delay(10);  // 停止程序运行
            }
          }
          Serial.println("Touch sensor initialized.");
          break;
      }
      else{
        Serial.println("I2C_2 连接失败,正在重试");
      }
      delay(200);
      Serial.print("重试次数:");
      Serial.println(retry);
  }
  Serial.print("重试次数:");
  Serial.println(retry);
  if(retry == 10){
      Serial.println("连接失败！");
      while (1) {
            delay(10);  // 停止程序运行
      }
  }
  Serial.println("I2C_2 连接成功！");
  //digitalWrite(RUNLED, HIGH);
}


void loop() {
  Recv();
  KeyboardDetect();
  Conditioning ? void() : TouchSend();//只有不处于设定模式时才发送触摸数据
  
}


void CY8C_RSET(){
  Touch[0].writeRegister(CTRL_CMD,SW_RESET);
  Touch[1].writeRegister(CTRL_CMD,SW_RESET);
  Touch[2].writeRegister(CTRL_CMD,SW_RESET);
}


void CY8C_HALT() {
  Conditioning = true;
}


void CY8C_Ratio() { // Set Touch Panel Ratio
  Serial.write('(');
  Serial.write(packet[1]); //L,R
  Serial.write(packet[2]); //sensor
  Serial.write('r');
  Serial.write(packet[4]); // Ratio
  Serial.write(')');
}

void CY8C_Sens() { // Set Touch Panel Sensitivity
  Serial.write('(');
  Serial.write(packet[1]); // L,R
  Serial.write(packet[2]); // sensor
  Serial.write('k');
  Serial.write(packet[4]); // Sensitivity
  Serial.write(')');
}

void CY8C_STAT() {
  Conditioning = false;
}

uint8_t len = 0; // 当前接收的包长度
void Recv() {
  while (Serial.available()) {
    uint8_t r = Serial.read();
    if (r == '{') {
      len = 0;
    }
    if (r == '}') {
      break;
    }
    packet[len++] = r;
  }
  if (len == 5) {
    switch (packet[3]) {
      case commandRSET:
        CY8C_RSET();
        break;
      case commandHALT:
        CY8C_HALT();
        break;
      case commandRatio:
        CY8C_Ratio();
        break;
      case commandSens:
        CY8C_Sens();
        break;
      case commandSTAT:
        CY8C_STAT();
        break;
    }
    len = 0;
    memset(packet, 0, 6);
  }
}



void TouchSend() {
  uint64_t TouchData = 0; //触摸数据包
  // 简单方法，从 mpr.touched() 一次读取 12个触摸点的按下状态，需要正确配置 mpr121 的各种参数值才能获取准确的状态
  //TouchData = (TouchData | Touch[0].touched()) << 16;// D7 ~ E8
  //TouchData = (TouchData | Touch[1].touched()) << 16;// B5 ~ D6
  //TouchData = (TouchData | Touch[2].touched());// A1 ~ B4
  //  for (uint8_t i = 0; i < 12; i++) {// B4 ~ A1
  //    TouchData = (TouchData | (int)(mprA.baselineData(i) - mprA.filteredData(i)) > Threshold) << 1;
  //  }
  //  TouchData >>= 1;
  for (int i = 33; i >= 0; i--) {
    TouchData <<= 1;
    TouchData = (TouchData | (int)(Touch[touchmap[i].sensid].read_touched_data(touchmap[i].portid)) > touchmap[i].thres);
  }

  Serial.write('(');
  for (uint8_t r = 0; r < 7; r++) {
    Serial.write((uint8_t)(TouchData & B11111));
    TouchData >>= 5;
  }
  Serial.write(')');
}

void KeyboardDetect(){
  const char keys[] = {'W', 'E', 'D', 'C', 'X', 'Z', 'A', 'Q'};
  static uint8_t lastState = 0;

  uint8_t buttonState = 0;

  // 读取按钮状态并生成二进制数
  buttonState |= (digitalRead(BTN1) == HIGH) << 7; // HIGH 表示按下
  buttonState |= (digitalRead(BTN2) == HIGH) << 6;
  buttonState |= (digitalRead(BTN3) == HIGH) << 5;
  buttonState |= (digitalRead(BTN4) == HIGH) << 4;
  buttonState |= (digitalRead(BTN5) == HIGH) << 3;
  buttonState |= (digitalRead(BTN6) == HIGH) << 2;
  buttonState |= (digitalRead(BTN7) == HIGH) << 1;
  buttonState |= (digitalRead(BTN8) == HIGH) << 0;

  // 检测按键变化
  for (int i = 0; i < 8; i++) {
    bool currentState = (buttonState >> (7 - i)) & 1;
    bool previousState = (lastState >> (7 - i)) & 1;

    if (currentState && !previousState) {
      // 按键按下
      Keyboard.press(keys[i]);
    } else if (!currentState && previousState) {
      // 按键释放
      Keyboard.release(keys[i]);
    }
  }

  lastState = buttonState;
}