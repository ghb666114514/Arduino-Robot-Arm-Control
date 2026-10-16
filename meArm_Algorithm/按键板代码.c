// ========== 自制板（按键板）代码 ==========
// 功能：检测4个按键，短按发 K1~K4，长按发 L1~L4

#define btn1Pin 23
#define btn2Pin 24
#define btn3Pin 25
#define btn4Pin 26

#define LONG_PRESS_TIME 1000  // 长按阈值 1 秒

int lastBtn1 = HIGH;
int lastBtn2 = HIGH;
int lastBtn3 = HIGH;
int lastBtn4 = HIGH;

unsigned long pressStart1 = 0;
unsigned long pressStart2 = 0;
unsigned long pressStart3 = 0;
unsigned long pressStart4 = 0;

int longFlag1 = 0;
int longFlag2 = 0;
int longFlag3 = 0;
int longFlag4 = 0;

void setup() {
  Serial.begin(9600);
  pinMode(btn1Pin, INPUT_PULLUP);
  pinMode(btn2Pin, INPUT_PULLUP);
  pinMode(btn3Pin, INPUT_PULLUP);
  pinMode(btn4Pin, INPUT_PULLUP);
}

void loop() {
  // ========== 按键1 ==========
  int btn1 = digitalRead(btn1Pin);
  if (lastBtn1 == HIGH && btn1 == LOW) {
    pressStart1 = millis();
    longFlag1 = 0;
  }
  if (btn1 == LOW && longFlag1 == 0) {
    if (millis() - pressStart1 > LONG_PRESS_TIME) {
      longFlag1 = 1;
      Serial.println(F("L1"));  // 长按：继续
      delay(300);
    }
  }
  if (lastBtn1 == LOW && btn1 == HIGH) {
    if (longFlag1 == 0) {
      Serial.println(F("K1"));  // 短按：循环夹取
      delay(300);
    }
    longFlag1 = 0;
  }
  lastBtn1 = btn1;

  // ========== 按键2 ==========
  int btn2 = digitalRead(btn2Pin);
  if (lastBtn2 == HIGH && btn2 == LOW) {
    pressStart2 = millis();
    longFlag2 = 0;
  }
  if (btn2 == LOW && longFlag2 == 0) {
    if (millis() - pressStart2 > LONG_PRESS_TIME) {
      longFlag2 = 1;
      Serial.println(F("L2"));  // 长按：取消
      delay(300);
    }
  }
  if (lastBtn2 == LOW && btn2 == HIGH) {
    if (longFlag2 == 0) {
      Serial.println(F("K2"));  // 短按：录制
      delay(300);
    }
    longFlag2 = 0;
  }
  lastBtn2 = btn2;

  // ========== 按键3 ==========
  int btn3 = digitalRead(btn3Pin);
  if (lastBtn3 == HIGH && btn3 == LOW) {
    pressStart3 = millis();
    longFlag3 = 0;
  }
  if (btn3 == LOW && longFlag3 == 0) {
    if (millis() - pressStart3 > LONG_PRESS_TIME) {
      longFlag3 = 1;
      delay(300);
    }
  }
  if (lastBtn3 == LOW && btn3 == HIGH) {
    if (longFlag3 == 0) {
      Serial.println(F("K3"));  // 短按：播放
      delay(300);
    }
    longFlag3 = 0;
  }
  lastBtn3 = btn3;

  // ========== 按键4 ==========
  int btn4 = digitalRead(btn4Pin);
  if (lastBtn4 == HIGH && btn4 == LOW) {
    pressStart4 = millis();
    longFlag4 = 0;
  }
  if (btn4 == LOW && longFlag4 == 0) {
    if (millis() - pressStart4 > LONG_PRESS_TIME) {
      longFlag4 = 1;
      Serial.println(F("L4"));  // 长按：暂停
      delay(300);
    }
  }
  if (lastBtn4 == LOW && btn4 == HIGH) {
    if (longFlag4 == 0) {
      Serial.println(F("K4"));  // 短按：回中
      delay(300);
    }
    longFlag4 = 0;
  }
  lastBtn4 = btn4;
}