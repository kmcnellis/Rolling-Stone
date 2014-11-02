#include <string.h>
#include <Servo.h>

#undef int
#include <stdio.h>

const int center = 1500;
Servo leftServo, rightServo;
signed int leftSpeed= 0;
signed int rightSpeed = 0;

void setup(){
  Serial.begin (9600);
  Serial.print ("Finished setup\n");
  leftServo.attach(10);
  rightServo.attach(11);
  leftServo.writeMicroseconds(center);
  rightServo.writeMicroseconds(center);
}

void loop(){
  leftSpeed=((x*40/1024)-20);
  rightSpeed=-leftSpeed;
  leftSpeed+=((20*y/255)-10)*10;
  rightSpeed+=((20*y/255)-10)*10;

  if(leftSpeed>200)leftSpeed=200;
  if(leftSpeed<-200)leftSpeed=-200;
  if(rightSpeed>200)rightSpeed=200;
  if(rightSpeed<-200)rightSpeed=-200;
  if(rightSpeed<30&&rightSpeed>-30)rightSpeed=0;
  if(leftSpeed<30&&leftSpeed>-30)leftSpeed=0;
  leftServo.writeMicroseconds(center+leftSpeed);
  rightServo.writeMicroseconds(center-rightSpeed);
}