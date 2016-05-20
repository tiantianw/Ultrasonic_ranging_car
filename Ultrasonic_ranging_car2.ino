#include <Servo.h>
#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(0, 1, 2, 3, 7);

int IN4 = 8;   // IN4
int IN3 = 9;   //IN3

int IN2 = 10;  // IN2
int IN1 = 11;  // IN1

int inputPin = 13;  // 超音波
int outputPin = 12;

float Fdistance = 0;      // 前方的障碍物距离
float Rdistance = 0;
float Ldistance = 0;

int directionn = 0;

Servo myservo;

int delay_time = 250;

int Fgo = 8;         // 定义前进的数值
int Rgo = 6;         // 定义右转的数值
int Lgo = 4;         // 定义左转的数值
int Bgo = 2;         // 定义倒车的数值

float distance;

void setup()
{
  // Serial.begin(9600);
  pinMode(IN4, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN1, OUTPUT);

  pinMode(MotorA, OUTPUT);
  pinMode(MotorB, OUTPUT);

  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);

  myservo.attach(4);
}

void advance(int a)
{
  digitalWrite(IN2, LOW);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN4, LOW);
  digitalWrite(IN3, HIGH);
  delay(a * 100);
}

void turnR(int d)
{
  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN4, LOW);
  digitalWrite(IN3, HIGH);
  delay(d * 50);
}

void turnL(int e)
{
  digitalWrite(IN2, LOW);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  delay(e * 50);
}

void stopp(int f)
{
  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, HIGH);
  delay(f * 100);
}

void back(int g)
{
  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  delay(g * 100);
}

void detection()        //测量3個角度(5.90.177)
{
  delay_time = 250;   // 舵机转向后的稳定时间
  ask_pin_F();            // 读取前方的距离

  if (Fdistance < 20)        // 假如前方距离小于10cm
  {
    stopp(1);               // 停止0.1秒
    back(2);                // 后退 0.2秒
  }
  if (Fdistance < 35)        // 假如前方距离小于25cm
  {
    stopp(1);               // 停止0.1秒
    ask_pin_L();            // 读取左边的距离
    delay(delay_time);      // 等待舵机稳定
    ask_pin_R();            // 读取右边的距离
    delay(delay_time);      // 等待舵机稳定

    if (Ldistance > Rdistance)  //假如左边的距离大于右边的距离
    { directionn = Lgo;      //向左边转
    }

    if (Ldistance <= Rdistance)  //假如右边距离大于等于左边的距离
    {
      directionn = Rgo;      //向右边走
    }

    if (Ldistance < 20 && Rdistance < 20)   //假如左边距离和右边距离都小于10CM
    {
      directionn = Bgo;      //向后退
    }
  }
  else                      //假如前方大于25CM
  {
    directionn = Fgo;        //向前走
  }

}

void ask_pin_F()   // 量出前方距离
{
  myservo.write(90);  //舵机指向中间
  delay(delay_time);  //舵机稳定时间
  Fdistance = Sonar();  // 读取距离值
  //  Serial.print("F distance:");      //用串口输出距离值
  // Serial.println(Fdistance);         //显示距离
}

void ask_pin_L()   // 量出左边的距离
{
  myservo.write(177); //舵机转向177度，左边
  delay(delay_time);
  Ldistance = Sonar();  // 读出距离值
  //  Serial.print("L distance:");       //输出距离
  //  Serial.println(Ldistance);
}

void ask_pin_R()   // 量出右边距离
{
  myservo.write(5); //舵机转向右边，5度
  delay(delay_time);
  Rdistance = Sonar(); // 讀差相差時間
  // Serial.print("R distance:");       //输出距离
  // Serial.println(Rdistance);
}

float Sonar()
{
  digitalWrite(outputPin, LOW);
  delayMicroseconds(2);
  digitalWrite(outputPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(outputPin, LOW);
  distance = pulseIn(inputPin, HIGH);
  distance = distance / 58; // 将时间转为距离值（单位：厘米）
  return distance;
}

void draw(int x, int y) { //距离值显示到OLED
  u8g.setFont(u8g_font_helvB08);//字体设置
  u8g.setPrintPos(x, y);
  u8g.print(distance);
}

void loop()
{
  myservo.write(90);  //每次主函数循环，先让舵机回中。
  detection();        //测量3个角度的距离值，判断往哪个方向走

  if (directionn == Bgo)    //假如方向为2，倒车
  {
    back(8);                    //  倒车
    turnL(1);                   //微向左转，防止卡死
    //   Serial.print(" Reverse");   //显示后退
  }
  if (directionn == Rgo)          //假如方向为 6(右转)
  {
    back(1);
    turnR(1);                   // 右转，调整该时间可以获得不同的转弯效果
    //  Serial.print(" Right");    //显示左转
  }
  if (directionn == Lgo)         //假如方向为4，左转
  {
    back(1);
    turnL(1);                  // 左转，调整该时间可以得到不同的转弯效果
    //  Serial.print(" Left ");     //显示右转
  }
  if (directionn == Fgo)         //假如方向为9，前进
  {
    advance(1);                 // 正常前进
    //   Serial.print("Advance");   //显示方向前进
    //  Serial.print("   ");
  }

  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_helvB08);
    u8g.setPrintPos(9, 15);
    u8g.print("Frontdistance:");
    u8g.setPrintPos(110, 15);
    u8g.print("cm");
    u8g.setPrintPos(9, 35);
    u8g.print("Leftdistance: ");
    u8g.setPrintPos(110, 35);
    u8g.print("cm");
    u8g.setPrintPos(9, 55);
    u8g.print("Rightdistance:");
    u8g.setPrintPos(110, 55);
    u8g.print("cm");
    if (directionn == Fgo)
      draw(85, 15);
    if (directionn == Lgo)
      draw(85, 35);
    if (directionn == Rgo)
      draw(85, 55);
  } while (u8g.nextPage());
  delay(100);
}




