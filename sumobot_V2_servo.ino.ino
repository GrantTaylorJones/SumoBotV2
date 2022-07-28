
#include <Servo.h>

//#define  module IO Pin
#define MOTOR_SERVO_LEFT 11;
#define MOTOR_SERVO_RIGHT 12;

int FORWARD_SPEED = 1550;
int REVERSE_SPEED = 1400;
int STOP_SPEED = 1500;
int CURRENT_LSPEED = STOP_SPEED;
int CURRENT_RSPEED = STOP_SPEED;
const int trigPin = 9;
const int echoPin = 10;

////   0 = ROTATING 1 = FORWARD   ///
//default to forward
int MOVEMENT_TYPE = 1;

Servo LEFT_SERVO;
Servo RIGHT_SERVO;

//TODO: reverse is 1400, and forward is 1550.
// STOP() is required between switching from rotating and forward to bring values back to baseline 1500
// need to implement logic to stop between rotating and forward when detecting object.

void setup()
{

pinMode(trigPin, OUTPUT); // Sets the echoPin as an Input
pinMode(echoPin, INPUT); // sets the trigPin as an Output
LEFT_SERVO.attach(11, 1450, 1550);
RIGHT_SERVO.attach(12, 1450, 1550);   
delay(1000);
Serial.begin(9600);
delay (1000);


}

void loop() {
  read_sensor();
  delay(500);
  SUMO();
}


void SUMO(){
  
  int distance = read_sensor();
  delay(50);

  ////  SEARCH  ////
  if(distance > 25){
    if(MOVEMENT_TYPE != 0){
      STOP();
      delay(50);
      MOVEMENT_TYPE = 0;
    }
    ROTATE_RIGHT();
  }
    
  ////  DESTROY ////
  else if(distance < 25){
    if(MOVEMENT_TYPE != 1){
      STOP();
      delay(50);
      MOVEMENT_TYPE = 1;
    }
    FORWARD();
  }

  
}

void FORWARD(){ 
  while(CURRENT_LSPEED < FORWARD_SPEED){
    LEFT_SERVO.writeMicroseconds(CURRENT_LSPEED);
    RIGHT_SERVO.writeMicroseconds(CURRENT_RSPEED);
    CURRENT_LSPEED += 1;
    CURRENT_RSPEED += 1;
    delay(10);
  }
}

//ATTN: MUST STOP FIRST//
void ROTATE_RIGHT(){
    while(CURRENT_RSPEED > REVERSE_SPEED){
    RIGHT_SERVO.writeMicroseconds(CURRENT_RSPEED);
    LEFT_SERVO.writeMicroseconds(CURRENT_LSPEED);
    CURRENT_RSPEED -= 2;
    CURRENT_LSPEED += 1;
    delay(10);
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
    delay(10);
  }
}

void STOP(){
  
  if(CURRENT_RSPEED > 1500 && CURRENT_LSPEED > 1500){
    STOP_FROM_FORWARD();
  }
  else if(CURRENT_RSPEED < 1500 && CURRENT_LSPEED > 1500){
    STOP_FROM_ROTATE_RIGHT();
  }
  else if(CURRENT_RSPEED > 1500 && CURRENT_LSPEED < 1500){
    STOP_FROM_ROTATE_LEFT();
  }
}



void STOP_FROM_FORWARD(){
  while(CURRENT_LSPEED > 1500 && CURRENT_RSPEED > 1500){  
    LEFT_SERVO.writeMicroseconds(CURRENT_LSPEED);
    RIGHT_SERVO.writeMicroseconds(CURRENT_RSPEED);
    CURRENT_LSPEED -= 1;
    CURRENT_RSPEED -= 1;
  }
}

void STOP_FROM_ROTATE_RIGHT(){
  while(CURRENT_LSPEED > 1500){  
    LEFT_SERVO.writeMicroseconds(CURRENT_LSPEED);
    RIGHT_SERVO.writeMicroseconds(CURRENT_RSPEED);
    CURRENT_LSPEED -= 1;
    CURRENT_RSPEED += 2;
  }
}

void STOP_FROM_ROTATE_LEFT(){
  while(CURRENT_RSPEED > 1500){  
    LEFT_SERVO.writeMicroseconds(CURRENT_LSPEED);
    RIGHT_SERVO.writeMicroseconds(CURRENT_RSPEED);
    CURRENT_RSPEED -= 1;
    CURRENT_LSPEED += 2;
  }
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
Serial.print("Distance from the object = ");
Serial.print(distance);
Serial.println(" cm");
if (distance > 100) distance = 99;
//return distance;
}
