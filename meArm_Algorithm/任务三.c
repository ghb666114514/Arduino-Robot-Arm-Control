#include <Servo.h>

// ========== 函数声明 ==========
void gohome();
void serial_handle();
void joystick_handle();
void pick(byte place[][4]);
void moveToAngle(Servo &s, int &current, int target);
void onButton1();
void onButton2();
void onButton3();
void onButton4();

// ========== 舵机对象 ==========
Servo base, larm, rarm, claw;

// ========== DSD 与 step 与死区 ==========
int DSD = 20;
int step = 1;
int deadzone = 300;

// ========== 当前舵机角度 ==========
int base_angle = 90;
int larm_angle = 90;
int rarm_angle = 90;
int claw_angle = 90;

// ========== 极限角度区 ==========
const int basemin = 0;
const int basemax = 180;
const int larmmin = 45;
const int larmmax = 125;
const int rarmmin = 60;
const int rarmmax = 136;
const int clawopen = 20;
const int clawoff = 100;

// ========== 按键引脚 ==========
#define btn1Pin 23
#define btn2Pin 24
#define btn3Pin 25
#define btn4Pin 26

int lastBtn1 = HIGH;
int lastBtn2 = HIGH;
int lastBtn3 = HIGH;
int lastBtn4 = HIGH;

// ========== 录制相关 ==========
#define maxRecord 120
#define recordInterval 100

byte recordBuffer[maxRecord][4];
int recordIndex = 0;//坐标点索引
int recordLength = 0;//已经记录的点
bool isRecording = false;
unsigned long lastRecordTime = 0;

// ========== 按键1循环索引 ==========
int taskIndex = 0;

// ========== 任务二夹取物体区 ==========
byte Aplace[8][4] = {
  { 47, 45, 90, 22 },
  { 47, 45, 136, 22 },
  { 47, 45, 136, 100 },
  { 47, 45, 99, 100 },
  { 130, 45, 97, 100 },
  { 130, 45, 136, 100 },
  { 130, 45, 136, 20 },
  { 130, 45, 91, 20 }
};

byte Bplace[8][4] = {
  { 65, 90, 69, 27 },
  { 65, 90, 111, 27 },
  { 65, 90, 111, 100 },
  { 65, 90, 79, 100 },
  { 124, 90, 79, 100 },
  { 124, 83, 85, 100 },
  { 124, 83, 85, 20 },
  { 124, 112, 83, 20 }
};

byte Cplace[8][4] = {
  { 62, 90, 110, 20 },
  { 64, 49, 136, 20 },
  { 64, 49, 136, 100 },
  { 67, 80, 91, 100 },
  { 136, 80, 91, 100 },
  { 124, 45, 125, 100 },
  { 124, 45, 125, 20 },
  { 124, 106, 84, 20 }
};

void setup() {
  Serial.begin(9600);
  delay(1000);

  base.attach(9);
  delay(1000);
  larm.attach(8);
  delay(1000);
  rarm.attach(7);
  delay(1000);
  claw.attach(6);
  delay(1000);

  pinMode(btn1Pin, INPUT_PULLUP);
  pinMode(btn2Pin, INPUT_PULLUP);
  pinMode(btn3Pin, INPUT_PULLUP);
  pinMode(btn4Pin, INPUT_PULLUP);

  gohome();

  Serial.println(F("meArm ready! 请发送指令: O/S/H/L/I/P/A/B/C/K1/K2/K3/K4"));
  Serial.println(F("或多电机协同: x30,y45,z60"));
}

void loop() {
  serial_handle();
  joystick_handle();

  // ========== 按键扫描（已经移植到了按键板）==========
  /*
  int btn1 = digitalRead(btn1Pin);
  if (lastBtn1 == HIGH && btn1 == LOW) {
    delay(50);
    if (digitalRead(btn1Pin) == LOW) { onButton1(); delay(300); }
  }
  lastBtn1 = btn1;

  int btn2 = digitalRead(btn2Pin);
  if (lastBtn2 == HIGH && btn2 == LOW) {
    delay(50);
    if (digitalRead(btn2Pin) == LOW) { onButton2(); delay(300); }
  }
  lastBtn2 = btn2;

  int btn3 = digitalRead(btn3Pin);
  if (lastBtn3 == HIGH && btn3 == LOW) {
    delay(50);
    if (digitalRead(btn3Pin) == LOW) { onButton3(); delay(300); }
  }
  lastBtn3 = btn3;

  int btn4 = digitalRead(btn4Pin);
  if (lastBtn4 == HIGH && btn4 == LOW) {
    delay(50);
    if (digitalRead(btn4Pin) == LOW) { onButton4(); delay(300); }
  }
  lastBtn4 = btn4;
  */
  // ========== 按键扫描结束 ==========

  // 录制中（用 millis 不阻塞）
  if (isRecording) {
    unsigned long now = millis();
    if (now - lastRecordTime >= recordInterval) {//如果当前时间减记录时间大于等于间隔
      lastRecordTime = now;//把记录时间重置
      if (recordIndex < maxRecord) {
        recordBuffer[recordIndex][0] = base_angle;
        recordBuffer[recordIndex][1] = larm_angle;
        recordBuffer[recordIndex][2] = rarm_angle;
        recordBuffer[recordIndex][3] = claw_angle;
        recordIndex++;
      } else {
        isRecording = false;
        recordLength = recordIndex;
        Serial.println(F("Record buffer full! Recording stopped."));
      }
    }
  }
}

// ========== 平滑移动 ==========
void moveToAngle(Servo &s, int &current, int target) {
  target = constrain(target, 0, 180);
  while (current != target) {
    if (current < target) {
      current += step;
      if (current > target) current = target;
    } else {
      current -= step;
      if (current < target) current = target;
    }
    s.write(current);
    delay(DSD);
  }
}

// ========== 回中 ==========
void gohome() {
  moveToAngle(base, base_angle, 90);
  moveToAngle(larm, larm_angle, 90);
  moveToAngle(rarm, rarm_angle, 90);
  moveToAngle(claw, claw_angle, 90);
}

// ========== 执行动作序列 ==========
void pick(byte place[][4]) {
  for (int i = 0; i < 8; i++) {
    moveToAngle(base, base_angle, place[i][0]);delay(500);
    moveToAngle(larm, larm_angle, place[i][1]);delay(500);
    moveToAngle(rarm, rarm_angle, place[i][2]);delay(500);
    moveToAngle(claw, claw_angle, place[i][3]);delay(500);
  }
}

// ========== 按键1：循环夹取 ==========
void onButton1() {
  switch (taskIndex) {
    case 0: pick(Aplace); Serial.println(F("Pick A")); break;
    case 1: pick(Bplace); Serial.println(F("Pick B")); break;
    case 2: pick(Cplace); Serial.println(F("Pick C")); break;
  }
  taskIndex = (taskIndex + 1) % 3;
}

// ========== 按键2：录制 ==========
void onButton2() {
  if (!isRecording) {
    isRecording = true;
    recordIndex = 0;
    lastRecordTime = millis();
    Serial.println(F("Recording start"));
  } else {
    isRecording = false;
    recordLength = recordIndex;
    Serial.print(F("Recording end, points="));
    Serial.println(recordLength);
  }
}

// ========== 按键3：播放 ==========
void onButton3() {
  if (recordLength == 0) {
    Serial.println(F("No recording!"));
    return;
  }
  for (int i = 0; i < recordLength; i++) {
    base.write(recordBuffer[i][0]);
    larm.write(recordBuffer[i][1]);
    rarm.write(recordBuffer[i][2]);
    claw.write(recordBuffer[i][3]);
    delay(recordInterval);
  }
  Serial.println(F("Playback done"));
}

// ========== 按键4：回中 ==========
void onButton4() {
  gohome();
  Serial.println(F("Go home!"));
}

// ========== 串口处理 ==========
void serial_handle() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "O") {
      moveToAngle(claw, claw_angle, clawopen);
      Serial.println(F("Claw open!"));
    }
    else if (cmd == "S") {
      moveToAngle(claw, claw_angle, clawoff);
      Serial.println(F("Claw close!"));
    }
    else if (cmd == "H") {
      DSD = max(5, DSD - 3);
      step = min(15, step + 1);
      Serial.print(F("Speed up! DSD="));
      Serial.print(DSD);
      Serial.print(F(", step="));
      Serial.println(step);
    }
    else if (cmd == "L") {
      DSD = min(30, DSD + 3);
      step = max(1, step - 1);
      Serial.print(F("Speed low! DSD="));
      Serial.print(DSD);
      Serial.print(F(", step="));
      Serial.println(step);
    }
    else if (cmd == "I") {
      gohome();
      Serial.println(F("Go home!"));
    }
    else if (cmd == "P") {
      Serial.println(F("---- 当前角度 ----"));
      Serial.print(F("base = "));
      Serial.println(base_angle);
      Serial.print(F("larm = "));
      Serial.println(larm_angle);
      Serial.print(F("rarm = "));
      Serial.println(rarm_angle);
      Serial.print(F("claw = "));
      Serial.println(claw_angle);
      Serial.println(F("------------------"));
    }
    else if (cmd.startsWith("x")) {
      int x, y, z;
      if (sscanf(cmd.c_str(), "x%d,y%d,z%d", &x, &y, &z) == 3) {
        x = constrain(x, basemin, basemax);
        y = constrain(y, larmmin, larmmax);
        z = constrain(z, rarmmin, rarmmax);

        moveToAngle(base, base_angle, x);
        moveToAngle(larm, larm_angle, y);
        moveToAngle(rarm, rarm_angle, z);

        Serial.print(F("base: "));
        Serial.print(base_angle);
        Serial.print(F("°  duty="));
        Serial.print(base.readMicroseconds() / 20000.0 * 100.0, 2);
        Serial.println(F("%"));

        Serial.print(F("larm: "));
        Serial.print(larm_angle);
        Serial.print(F("°  duty="));
        Serial.print(larm.readMicroseconds() / 20000.0 * 100.0, 2);
        Serial.println(F("%"));

        Serial.print(F("rarm: "));
        Serial.print(rarm_angle);
        Serial.print(F("°  duty="));
        Serial.print(rarm.readMicroseconds() / 20000.0 * 100.0, 2);
        Serial.println(F("%"));

        Serial.println(F("多舵机协同转动完毕！"));
      }
    }
    else if (cmd == "A") {
      pick(Aplace);
      Serial.println(F("A done"));
    }
    else if (cmd == "B") {
      pick(Bplace);
      Serial.println(F("B done"));
    }
    else if (cmd == "C") {
      pick(Cplace);
      Serial.println(F("C done"));
    }
    else if (cmd == "K1") onButton1();
    else if (cmd == "K2") onButton2();
    else if (cmd == "K3") onButton3();
    else if (cmd == "K4") onButton4();
    else {
      Serial.println(F("指令错误！"));
    }
  }
}

// ========== 摇杆处理 ==========
void joystick_handle() {
  int joy1_x = analogRead(A0);
  int joy1_y = analogRead(A1);
  int joy2_x = analogRead(A2);
  int joy2_y = analogRead(A3);

  if (abs(joy1_x - 512) > deadzone) {
    base_angle += (joy1_x > 512 ? -step : step);
    base_angle = constrain(base_angle, basemin, basemax);
    base.write(base_angle);
    delay(DSD);
  }
  if (abs(joy1_y - 512) > deadzone) {
    larm_angle += (joy1_y > 512 ? -step : step);
    larm_angle = constrain(larm_angle, larmmin, larmmax);
    larm.write(larm_angle);
    delay(DSD);
  }
  if (abs(joy2_x - 512) > deadzone) {
    claw_angle += (joy2_x > 512 ? step : -step);
    claw_angle = constrain(claw_angle, clawopen, clawoff);
    claw.write(claw_angle);
    delay(DSD);
  }
  if (abs(joy2_y - 512) > deadzone) {
    rarm_angle += (joy2_y > 512 ? -step : step);
    rarm_angle = constrain(rarm_angle, rarmmin, rarmmax);
    rarm.write(rarm_angle);
    delay(DSD);
  }
}
