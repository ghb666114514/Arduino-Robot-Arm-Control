#include <Servo.h>
void handleSerial();
void handleJoystick();
void moveToHome();

// ========== 引脚定义（按你的实际接线修改）==========
#define BASE_PIN     11   // 底座舵机
#define SHOULDER_PIN 10   // 肩部舵机
#define ELBOW_PIN    9    // 肘部舵机
#define CLAW_PIN     6    // 夹爪舵机

#define JOY_X_PIN    A0   // 摇杆X轴
#define JOY_Y_PIN    A1   // 摇杆Y轴

// ========== 舵机对象 ==========
Servo servoBase;
Servo servoShoulder;
Servo servoElbow;
Servo servoClaw;

// ========== 角度限制（根据实际机械结构调整）==========
const int BASE_MIN = 0,    BASE_MAX = 180;
const int SHOULDER_MIN = 0, SHOULDER_MAX = 180;
const int ELBOW_MIN = 0,   ELBOW_MAX = 180;
const int CLAW_OPEN = 70;   // 夹爪张开角度，需实测
const int CLAW_CLOSE = 10;  // 夹爪闭合角度，需实测

// ========== 摇杆死区（防止抖动）==========
const int JOY_DEADZONE = 50;

// ========== 速度控制 ==========
int speedDelay = 15;  // 舵机步进延时，越小越快

// ========== 当前角度 ==========
int baseAngle = 90;
int shoulderAngle = 90;
int elbowAngle = 90;

// ========== 初始/回中姿态 ==========
const int HOME_BASE = 90;
const int HOME_SHOULDER = 90;
const int HOME_ELBOW = 90;

void setup() {
  Serial.begin(9600);

  servoBase.attach(BASE_PIN);
  servoShoulder.attach(SHOULDER_PIN);
  servoElbow.attach(ELBOW_PIN);
  servoClaw.attach(CLAW_PIN);

  // 上电先回中
  moveToHome();

  Serial.println("MeArm ready. Commands: O/S/H/L, x,y,z format");
}

void loop() {
  // 1. 串口指令处理（任务一）
  handleSerial();

  // 2. 摇杆实时控制（任务一基础控制）
  handleJoystick();
}

// ========== 串口指令处理 ==========
void handleSerial() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "O") {
      servoClaw.write(CLAW_OPEN);
      Serial.println("Claw OPEN");
    }
    else if (cmd == "S") {
      servoClaw.write(CLAW_CLOSE);
      Serial.println("Claw CLOSE");
    }
    else if (cmd == "H") {
      speedDelay = max(5, speedDelay - 3);
      Serial.print("Speed UP, delay=");
      Serial.println(speedDelay);
    }
    else if (cmd == "L") {
      speedDelay = min(30, speedDelay + 3);
      Serial.print("Speed DOWN, delay=");
      Serial.println(speedDelay);
    }
    // 多舵机协同：格式 "x90,y45,z120"
    else if (cmd.startsWith("x")) {
      int x, y, z;
      if (sscanf(cmd.c_str(), "x%d,y%d,z%d", &x, &y, &z) == 3) {
        baseAngle = constrain(x, BASE_MIN, BASE_MAX);
        shoulderAngle = constrain(y, SHOULDER_MIN, SHOULDER_MAX);
        elbowAngle = constrain(z, ELBOW_MIN, ELBOW_MAX);
        servoBase.write(baseAngle);
        servoShoulder.write(shoulderAngle);
        servoElbow.write(elbowAngle);
        Serial.println("Finish!");
      }
    }
  }
}

// ========== 摇杆控制 ==========
  void handleJoystick() {
  int joyX = analogRead(JOY_X_PIN);
  int joyY = analogRead(JOY_Y_PIN);

  // 死区处理 + 映射
  if (abs(joyX - 512) > JOY_DEADZONE) {
    baseAngle += map(joyX, 0, 1023, -2, 2);
    baseAngle = constrain(baseAngle, BASE_MIN, BASE_MAX);
    servoBase.write(baseAngle);
  }

  if (abs(joyY - 512) > JOY_DEADZONE) {
    shoulderAngle += map(joyY, 0, 1023, 2, -2);  // 方向可能需要反过来
    shoulderAngle = constrain(shoulderAngle, SHOULDER_MIN, SHOULDER_MAX);
    servoShoulder.write(shoulderAngle);
  }
}

// ========== 回中 ==========
void moveToHome() {
  servoBase.write(HOME_BASE);
  servoShoulder.write(HOME_SHOULDER);
  servoElbow.write(HOME_ELBOW);
  servoClaw.write(CLAW_OPEN);
  baseAngle = HOME_BASE;
  shoulderAngle = HOME_SHOULDER;
  elbowAngle = HOME_ELBOW;
  delay(500);
}
