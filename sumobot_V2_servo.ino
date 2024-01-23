//Have to use servo library for the speed controllers
#include <Servo.h>

//#define  module IO Pin
#define MOTOR_SERVO_LEFT 11;
#define MOTOR_SERVO_RIGHT 12;

//1500 is no movement in any direction. The further from 1500, the greater the speed in that direction.
double FORWARD_SPEED = 1580;
double REVERSE_SPEED = 1450;
//int ROTATE_REVERSE_SPEED = 14;
double STOP_SPEED = 1500;
double CURRENT_LSPEED = STOP_SPEED;
double CURRENT_RSPEED = STOP_SPEED;
const int trigPin = 9;
const int echoPin = 10;
const int TRIGGER_DISTANCE = 50;

///  0 = ROTATING 1 = FORWARD,    default to forward
int MOVEMENT_TYPE = 1;
// 0 = RIGHT 1 = LEFT;
int ROTATE_DIR = 1;

//MULETA (red flag), see loop()
boolean MULETA = false;

Servo LEFT_SERVO;
Servo RIGHT_SERVO;

// STOP() is required between switching from rotating and forward to bring values back to baseline 1500
// need to implement logic to stop between rotating and forward when detecting object.

void setup(){
  pinMode(trigPin, OUTPUT); // sets the trigPin as an Output
  pinMode(echoPin, INPUT); // sets the echoPin as an Input
  LEFT_SERVO.attach(11, 1450, 1550); //pin 11 one side of wheels
  RIGHT_SERVO.attach(12, 1450, 1550);   //pin 12 other side of wheels
  delay(1000);
  Serial.begin(9600);
  delay (1000);
  RIGHT_SERVO.writeMicroseconds(STOP_SPEED);
  LEFT_SERVO.writeMicroseconds(STOP_SPEED);
  delay(7000);
}

//Main operating loop. 
//Since we began competition with a judges countdown, we opted to start the robot in the following way:
//start with peice of paper (muleta) in front of sensor (distance of 10), once we remove the paper (muleta = true), the robot will hit the sumo function on a loop.
void loop() {
  Serial.println("Muleta: " + String(MULETA));
  if(!MULETA){
    STOP();
    int distance = read_sensor();
    Serial.println("distance: " + distance);
    if(distance > 10){
      MULETA = true;
      Serial.println("TERMINATE");
    }
  }
  else{
    SUMO();
  }
}

//Rotates until something is within the set distance value from the sensor (50) (SEEK)
//Once distance to object is within range, it will drive forward (DESTROY)
void SUMO(){
  int distance = read_sensor();
  delay(25);

  ////  SEARCH  ////
  if(distance > TRIGGER_DISTANCE){
    if(MOVEMENT_TYPE != 0){
      STOP();
      delay(25);
      MOVEMENT_TYPE = 0;
    }
    if(ROTATE_DIR == 1){
      ROTATE_LEFT();
    }
    else{
      ROTATE_RIGHT();
    }
  }
    
  ////  DESTROY ////
  else if(distance <= TRIGGER_DISTANCE){
    if(MOVEMENT_TYPE != 1){
      STOP();
      delay(25);
      MOVEMENT_TYPE = 1;
    }
    FORWARD();
  }

}

void FORWARD(){ 
  Serial.println("FORWARD");
  while(CURRENT_LSPEED < FORWARD_SPEED){
    int distance = read_sensor();
    LEFT_SERVO.writeMicroseconds(CURRENT_LSPEED);
    RIGHT_SERVO.writeMicroseconds(CURRENT_RSPEED);
    CURRENT_LSPEED += 1;
    CURRENT_RSPEED += 1;
    if(distance > TRIGGER_DISTANCE) break;
    delay(15);
  }
}

//ATTN: MUST STOP FIRST//
void ROTATE_RIGHT(){
    Serial.println("ROTATING");
    while(CURRENT_RSPEED > REVERSE_SPEED){
    RIGHT_SERVO.writeMicroseconds(CURRENT_RSPEED);
    LEFT_SERVO.writeMicroseconds(CURRENT_LSPEED);
    CURRENT_RSPEED -= 2;
    CURRENT_LSPEED += 1;
    delay(5);
  }
}

//ATTN: MUST STOP FIRST///
void ROTATE_LEFT(){
    while(CURRENT_LSPEED > REVERSE_SPEED){
     Serial.println(CURRENT_LSPEED);
    RIGHT_SERVO.writeMicroseconds(CURRENT_RSPEED);
    LEFT_SERVO.writeMicroseconds(CURRENT_LSPEED);
    CURRENT_LSPEED -= 2;
    CURRENT_RSPEED += 1;
    delay(5);
  }
}

void STOP(){
  if(CURRENT_RSPEED > STOP_SPEED && CURRENT_LSPEED > STOP_SPEED){
    STOP_FROM_FORWARD();
  }
  else if(CURRENT_RSPEED < STOP_SPEED && CURRENT_LSPEED > STOP_SPEED){
    STOP_FROM_ROTATE_RIGHT();
  }
  else if(CURRENT_RSPEED > STOP_SPEED && CURRENT_LSPEED < STOP_SPEED){
    STOP_FROM_ROTATE_LEFT();
  }
  //initial state will flash both with 1500
  else{
    LEFT_SERVO.writeMicroseconds(CURRENT_LSPEED);
    RIGHT_SERVO.writeMicroseconds(CURRENT_RSPEED);
  }
}



void STOP_FROM_FORWARD(){
  while(CURRENT_LSPEED > STOP_SPEED && CURRENT_RSPEED > STOP_SPEED){  
    LEFT_SERVO.writeMicroseconds(CURRENT_LSPEED);
    RIGHT_SERVO.writeMicroseconds(CURRENT_RSPEED);
    CURRENT_LSPEED -= 1;
    CURRENT_RSPEED -= 1;
    delay(5);
  }
}

void STOP_FROM_ROTATE_RIGHT(){
  while(CURRENT_LSPEED > STOP_SPEED){  
    LEFT_SERVO.writeMicroseconds(CURRENT_LSPEED);
    RIGHT_SERVO.writeMicroseconds(CURRENT_RSPEED);
    CURRENT_LSPEED -= 1;
    CURRENT_RSPEED += 2;
    delay(5);
  }
  ROTATE_DIR = 1;
}

void STOP_FROM_ROTATE_LEFT(){
  while(CURRENT_RSPEED > STOP_SPEED){  
    LEFT_SERVO.writeMicroseconds(CURRENT_LSPEED);
    RIGHT_SERVO.writeMicroseconds(CURRENT_RSPEED);
    CURRENT_RSPEED -= 1;
    CURRENT_LSPEED += 2;
    delay(5);
  }
  ROTATE_DIR = 0;
}



int read_sensor(){
  // defines arduino pins numbers
  //const int trigPin = 3;
  //const int echoPin = 4;

  // defines variables
  long duration;
  int distance;

  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
  // Prints the distance on the Serial Monitor
  //Serial.print("Distance from the object = ");
  //Serial.print(distance);
  //Serial.println(" cm");
  if (distance > 100) distance = 99;
  return distance;
}

