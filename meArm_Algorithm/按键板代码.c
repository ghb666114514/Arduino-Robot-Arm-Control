// ========== 按键板代码 ==========
// 功能：检测4个按键，通过串口发送 K1~K4

#define btn1Pin 23
#define btn2Pin 24
#define btn3Pin 25
#define btn4Pin 26

int lastBtn1 = HIGH;
int lastBtn2 = HIGH;
int lastBtn3 = HIGH;
int lastBtn4 = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(btn1Pin, INPUT_PULLUP);
  pinMode(btn2Pin, INPUT_PULLUP);
  pinMode(btn3Pin, INPUT_PULLUP);
  pinMode(btn4Pin, INPUT_PULLUP);
}

void loop() {
  // 按键1
  int btn1 = digitalRead(btn1Pin);
  if (lastBtn1 == HIGH && btn1 == LOW) {
    delay(50);
    if (digitalRead(btn1Pin) == LOW) {
      Serial.println(F("K1"));
      delay(300);
    }
  }
  lastBtn1 = btn1;

  // 按键2
  int btn2 = digitalRead(btn2Pin);
  if (lastBtn2 == HIGH && btn2 == LOW) {
    delay(50);
    if (digitalRead(btn2Pin) == LOW) {
      Serial.println(F("K2"));
      delay(300);
    }
  }
  lastBtn2 = btn2;

  // 按键3
  int btn3 = digitalRead(btn3Pin);
  if (lastBtn3 == HIGH && btn3 == LOW) {
    delay(50);
    if (digitalRead(btn3Pin) == LOW) {
      Serial.println(F("K3"));
      delay(300);
    }
  }
  lastBtn3 = btn3;

  // 按键4
  int btn4 = digitalRead(btn4Pin);
  if (lastBtn4 == HIGH && btn4 == LOW) {
    delay(50);
    if (digitalRead(btn4Pin) == LOW) {
      Serial.println(F("K4"));
      delay(300);
    }
  }
  lastBtn4 = btn4;
}