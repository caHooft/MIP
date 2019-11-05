
//All methods
/*
 * ReceiveEvent
 * LEDM
 * SaveUS
 * Neck
 * Distance
 * RandomMove
 * ControlMove
 * ToggleDir
 * Movement
 * Tail
 * ReceiveMood
 * ChangeMood
 * HappyBrows
 * SadBrows
 * AngryBrows
 * DisgustedBrows
 * ScaredBrows
 * SleepBrows
 */

//Including libraries
#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal.h>

//Declaring pins
#define LCDrs 2
#define LCDen 3
#define LCDd4 4
#define LCDd5 5
#define LCDd6 6
#define LCDd7 7
#define RightWheels 8
#define LeftWheels 9
#define neckpin 10
#define tailpin 11
#define LED 13
#define trig 14
#define echo 15

//Declaring some variables
int x;
int DistanceLeft = 0;
int DistanceFront = 0;
int DistanceRight = 0;
int mipspeed;
bool AION = true;
bool sleep = false;

//Declaring some hardware
Servo neck;
Servo tail;
LiquidCrystal lcd(LCDrs, LCDen, LCDd4, LCDd5, LCDd6, LCDd7);

void setup()  
{
  Wire.begin(9);
  Wire.onReceive(ReceiveEvent);
  Serial.begin(9600);
  Serial.println("Arduino UNO 2 start");
  
  //Seting up some hardware
  neck.attach(neckpin);
  neck.write(90);
  
  tail.attach(tailpin);
  tail.write(90);
  
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("     __  __     ");

  //Initializing pins
  pinMode(LED, OUTPUT);
  pinMode(LeftWheels, OUTPUT);
  pinMode(RightWheels, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() 
{
  x = 0;
  
  Serial.println();
  Serial.println("-------------------------------------------------");
  Serial.println();
  Serial.println("Restart loop");
  Serial.println();
  
  Tail();
  Serial.println();

  if(x == 0) 
  {
    LEDM(LOW);
  }

  if(x == 1) 
  {
    LEDM(HIGH);
  }

  if(x == 2) 
  {
    AION = true;
  }
  
  if(x == 13)
  {
    AION = false;
  }
    
  if(x >= 3 && x < 8)
  {
    ChangeMood(x);
  }

  if(x == 8)
  {
    ChangeMood(8);
    sleep = true;
  }
  
  if(x == 14)
  {
    ChangeMood(3);
    sleep = false;
  }

  if(x > 15)
  {
    mipspeed = x;
  }
  
  if(AION)
  {
    Serial.println();
    Neck();
    Serial.println();
    RandomMove();
  }
  
  else if(!AION){
    if(x >= 9 && x <= 12)
    {
      switch(x)
      {
                
      }
    }
  }

  Serial.println();
  Tail();
}

//Method for receiving commands
void ReceiveEvent(int bytes)
{
  x = Wire.read();

  Serial.print("Read wire at: ");
  Serial.println(x);
}

//Method for blinking the verification LED
void LEDM(byte b)
{
  digitalWrite(LED, b);
  
  Serial.print("LED at: ");
  Serial.println(b);
}

//Method for saving the US data
void SaveUS(int xsurrogate)
{
  if(DistanceLeft == 0)
  {
    DistanceLeft = xsurrogate;
  }

  else if(DistanceFront == 0)
  {
    DistanceFront = xsurrogate;
  }

  else if(DistanceRight == 0)
  {
    DistanceRight = xsurrogate;
  }

  else
  {
    DistanceLeft = xsurrogate;
    DistanceFront = 0;
    DistanceRight = 0;

  }
}

//Method for sweeping the neck
void Neck()
{
  neck.write(180);
  delay(1000);
  DistanceLeft = Distance();
  Serial.print(DistanceLeft);
  Serial.print("cm links, ");
  
  neck.write(90);
  delay(1000);
  DistanceFront = Distance();
  Serial.print(DistanceFront);
  Serial.print("cm voor en ");
  
  neck.write(0);
  delay(1000);
  DistanceRight = Distance();
  Serial.print(DistanceRight);
  Serial.println("cm rechts.");
  
  neck.write(90);
}

//Method for reading ultra sonic sensor and avoiding collisions
int Distance()
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  float USvalue = pulseIn(echo, HIGH) / 29 / 2;
  
  if(USvalue > 200)
  {
    USvalue = 200;
  }
  
  return USvalue;
}

//Method for converting distance into milliseconds
int ConvertDistance(int cm){
  //Factor is time in milleseconds divided by distance in centimeters, eg. 1000/27=37
  int ms = cm * 37;
  return ms;
}

int ConvertDegrees(int degree){
  //Factor is time in milleseconds divided by distance in centimeters, eg. 10000/422=24
  int ms = degree * 24;
  return ms;
}

//Method for random movement
void RandomMove()
{
  //0.8125 is 90 JK
  //1.625 is 180 JK
  //3.25 is 360 JK

  Serial.print("Left: ");
  Serial.println(DistanceLeft);
  Serial.print("Front: ");
  Serial.println(DistanceFront);
  Serial.print("Right: ");
  Serial.println(DistanceRight);

  if(50 >= DistanceLeft || 50 >= DistanceFront || 50 >= DistanceRight)
  {
    if(DistanceLeft >= DistanceFront && DistanceLeft >= DistanceRight)
    {
      ToggleDir(2);
      delay(ConvertDegrees(90));
      
      if(DistanceLeft >= 50)
      {
        ToggleDir(1);
        delay(ConvertDistance(50));
      }
      
      ToggleDir(0);
    }
    
    else if(DistanceFront >= DistanceLeft && DistanceFront >= DistanceRight)
    {
      if(DistanceLeft >= DistanceRight)
      {
        ToggleDir(2);
        delay(ConvertDegrees(90));
        ToggleDir(0);
      }
      
      else
      {
        ToggleDir(3);
        delay(ConvertDegrees(90));
        ToggleDir(0);
      }
    }
    
    else if(DistanceRight >= DistanceLeft && DistanceRight >= DistanceFront)
    {
      ToggleDir(3);
      delay(ConvertDegrees(90));
      if(DistanceRight >= 50)
      {
        ToggleDir(1);
        delay(ConvertDistance(50));
      }
      
      ToggleDir(0);
    }
  }
  
  else
  {
    ToggleDir(1);
    float val = DistanceFront;
    if(val >= 100)
    {
        val -= 25;
    }
    
    val = random(50, val);
    delay(ConvertDistance(val));
    ToggleDir(0);
  }
}

//Method for controlled movement
void ControlMove(int dir)
{
  
}

//Method for toggling the wheels
void ToggleDir(byte dir){
  if(dir == 1){
    digitalWrite(LeftWheels, HIGH);
    digitalWrite(RightWheels, HIGH);
  }
  else if(dir == 2){
    digitalWrite(RightWheels, HIGH);
    digitalWrite(LeftWheels, LOW);
  }
  else if(dir == 3){
    digitalWrite(LeftWheels, HIGH);
    digitalWrite(RightWheels, LOW);
  }
  else if(dir == 0){
    digitalWrite(LeftWheels, LOW);
    digitalWrite(RightWheels, LOW);
  }
}

//Method for sweeping the tail
void Tail()
{
  tail.write(180);
  delay(200);
  tail.write(0);
  delay(400);
  tail.write(90);
}

//Method for changing mood on LCD
void ChangeMood(int i)
{
  if(i == 3)
  {
    HappyBrows();
  }

  if(i == 4)
  {
    SadBrows();
  }

  if(i == 5)
  {
    AngryBrows();
  }

  if(i == 6)
  {
    DisgustedBrows();
  }

  if(i == 7)
  {
    ScaredBrows();
  }

  if(i == 8)
  {
    SleepBrows();
  }
}

void HappyBrows()
{
  byte browOne[8] = {0,0,0b00011,0b00111,0b01111,0b11100,0b01000};
  lcd.createChar(0, browOne);
  byte browTwo[8] = {0,0b11111,0b11111,0b11000,0,0,0,0};
  lcd.createChar(1, browTwo);
  byte browThree[8] = {0,0b11111,0b11111,0,0,0,0,0};
  lcd.createChar(2, browThree);
  byte browFour[8] = {0,0b11110,0b11111,0b00011,0,0,0,0};
  lcd.createChar(3, browFour);
  byte browFive[8] = {0,0b01111,0b11111,0b11000,0,0,0,0};
  lcd.createChar(4, browFive);
  byte browSix[8] = {0,0b11111,0b11111,0,0,0,0,0};
  lcd.createChar(5, browSix);
  byte browSeven[8] = {0,0b11111,0b11111,0b00011,0,0,0,0};
  lcd.createChar(6, browSeven);
  byte browEight[8] = {0,0,0b11000,0b11100,0b11110,0b00111,0b00010};
  lcd.createChar(7, browEight);

  lcd.setCursor(2,1);
  lcd.write(byte(0));
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.write(byte(3));
  lcd.setCursor(10,1);
  lcd.write(byte(4));
  lcd.write(byte(5));
  lcd.write(byte(6));
  lcd.write(byte(7));
}

void AngryBrows()
{
  byte browOne[8] = {0b01111, 0b11111, 0b01111, 0b00000, 0b00000, 0b00000, 0b00000,0};
  lcd.createChar(0, browOne);
  byte browTwo[8] = {0b00000, 0b11100, 0b11111, 0b11111, 0b00111, 0b00000, 0, 0b00000};
  lcd.createChar(1, browTwo);
  byte browThree[8] = {0, 0, 0, 0b11111, 0b11111, 0b11111, 0b00001, 0};
  lcd.createChar(2, browThree);
  byte browFour[8] = {0,0,0,0,0b11000, 0b11110,0b11111,0b00111};
  lcd.createChar(3, browFour);
  byte browFive[8] = {0,0,0,0,0b00011,0b01111,0b11111,0b11100};
  lcd.createChar(4, browFive);
  byte browSix[8] = {0,0,0,0b11111,0b11111,0b11111,0b10000,0};
  lcd.createChar(5, browSix);
  byte browSeven[8] = {0, 0b00111, 0b11111, 0b11111, 0b11100, 0, 0, 0};
  lcd.createChar(6, browSeven);
  byte browEight[8] = {0b11110, 0b11111, 0b11110,0,0,0,0,0};
  lcd.createChar(7, browEight);

  lcd.setCursor(2,1);
  lcd.write(byte(0));
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.write(byte(3));
  lcd.setCursor(10,1);
  lcd.write(byte(4));
  lcd.write(byte(5));
  lcd.write(byte(6));
  lcd.write(byte(7));
}

void DisgustedBrows()
{
  byte browOne[8] = {0,0b00011,0b01111,0b11100,0b11000,0b10000,0,0};
  lcd.createChar(0,browOne);
  byte browTwo[8] = {0b11111,0b11111,0b11000,0,0,0,0,0};
  lcd.createChar(1,browTwo);
  byte browThree[8] = {0b11111,0b11111,0b00011,0,0,0,0,0};
  lcd.createChar(2,browThree);
  byte browFour[8] = {0,0b11000,0b11110,0b00111,0b00011,0b00001,0,0};
  lcd.createChar(3,browFour);
  byte browFive[8] = {0,0,0,0,0,0b11100,0b01111,0b00011};
  lcd.createChar(4,browFive);
  byte browSix[8] = {0,0,0,0,0,0b00011,0b11111,0b11100};
  lcd.createChar(5,browSix);
  byte browSeven[8] = {0,0,0,0b00011,0b11111,0b11100,0,0};
  lcd.createChar(6,browSeven);
  byte browEight[8] = {0,0b00111,0b11110,0b11100,0,0,0,0};
  lcd.createChar(7,browEight);

  lcd.clear();
  lcd.setCursor(2,0);
  lcd.write(byte(0));
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.write(byte(3));
  lcd.setCursor(10,1);
  lcd.write(byte(4));
  lcd.write(byte(5));
  lcd.write(byte(6));
  lcd.write(byte(7));
}

void SadBrows()
{
  byte browOne[8] = {0,0,0,0,0,0b00111,0b11111,0b11100};
  lcd.createChar(0,browOne);
  byte browTwo[8] = {0,0,0,0,0b01111,0b11111,0b11000,0};
  lcd.createChar(1,browTwo);
  byte browThree[8] = {0,0,0,0b01111,0b11111,0b11000,0,0};
  lcd.createChar(2,browThree);
  byte browFour[8] = {0b00110,0b00111,0b01111,0b11110,0b11100,0,0,0};
  lcd.createChar(3,browFour);
  byte browFive[8] = {0b01100,0b11100,0b11110,0b01111,0b00111,0,0,0};
  lcd.createChar(4,browFive);
  byte browSix[8] = {0,0,0,0b11110,0b11111,0b00011,0,0};
  lcd.createChar(5,browSix);
  byte browSeven[8] = {0,0,0,0,0b11110,0b11111,0b00011,0};
  lcd.createChar(6,browSeven);
  byte browEight[8] = {0,0,0,0,0,0b11100,0b11111,0b00111};
  lcd.createChar(7,browEight);

  lcd.clear();
  lcd.setCursor(2,1);
  lcd.write(byte(0));
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.write(byte(3));
  lcd.setCursor(10,1);
  lcd.write(byte(4));
  lcd.write(byte(5));
  lcd.write(byte(6));
  lcd.write(byte(7));
}

void ScaredBrows()
{
  byte browOne[8] = {0b00001,0b00011,0b01110,0b11100,0b11000,0b11000,0b10000};
  lcd.createChar(0,browOne);
  byte browTwo[8] = {0b11111,0b11000,0,0,0,0,0,0};
  lcd.createChar(1,browTwo);
  byte browThree[8] = {0b11111,0,0,0,0,0,0,0};
  lcd.createChar(2,browThree);
  byte browFour[8] = {0,0,0,0,0,0b10100,0b01000,0};
  lcd.createChar(3,browFour);
  byte browFive[8] = {0,0b01010,0b01010,0b00100,0,0,0,0};
  lcd.createChar(4,browFive);
  byte browSix[8] = {0b11111,0,0,0,0,0,0,0};
  lcd.createChar(5,browSix);
  byte browSeven[8] = {0b11111,0b00011,0,0,0,0,0,0};
  lcd.createChar(6,browSeven);
  byte browEight[8] = {0b10000,0b11000,0b01110,0b00111,0b00011,0b00011,0b00001};
  lcd.createChar(7,browEight);

  lcd.clear();
  lcd.setCursor(2,0);
  lcd.write(byte(0));
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.setCursor(11,0);
  lcd.write(byte(5));
  lcd.write(byte(6));
  lcd.write(byte(7));
  lcd.setCursor(1,1);
  lcd.write(byte(3));
  lcd.setCursor(15,0);
  lcd.write(byte(4));
  lcd.setCursor(9,1);
  lcd.write(byte(3));
  lcd.setCursor(3,1);
  lcd.write(byte(4));
}

void SleepBrows()
  {
    lcd.clear();
    lcd.setCursor(2, 1);
    lcd.write("____    ____");
  }
