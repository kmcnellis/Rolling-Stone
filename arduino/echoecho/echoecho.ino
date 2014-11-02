#include <string.h>
#include <Servo.h>

int setting_pin = 13;
int echo = 1;
String number = "";
int x=0;
int y=0;
char reading = 0;
int count=0;

const int center = 1500;
Servo leftServo, rightServo;
signed int leftSpeed= 0;
signed int rightSpeed = 0;

void setup() {
  // initialize both serial ports:
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(setting_pin, INPUT);
  echo = digitalRead(setting_pin);
  leftServo.attach(10);
  rightServo.attach(11);
  leftServo.writeMicroseconds(center);
  rightServo.writeMicroseconds(center);
}

void loop() {
  if(echo){
    if (Serial1.available()) {
      char inByte = Serial1.read();
      Serial.print(inByte);
    }
  }else{
    if (Serial1.available()) {
      char inByte = Serial1.read();
      if(inByte=='x'||inByte=='y'){
        count+=1;
        if(number != ""){
          if(reading=='x'){
            x=number.toInt();
          }
          if(reading=='y'){
            y=number.toInt();
          }
          number="";
        }
        reading=inByte;
      }else{
        number+=inByte;
      }
      
      
      leftSpeed=y*100.0/1024;
      rightSpeed=-leftSpeed;
      leftSpeed+=x*100.0/1024;
      rightSpeed+=x*100.0/1024;
      Serial.println(rightSpeed);
      Serial.println(leftSpeed);
      Serial.println("\n"); 

      if(leftSpeed>200)leftSpeed=200;
      if(leftSpeed<-200)leftSpeed=-200;
      if(rightSpeed>200)rightSpeed=200;
      if(rightSpeed<-200)rightSpeed=-200;
      if(rightSpeed<30&&rightSpeed>-30)rightSpeed=0;
      if(leftSpeed<30&&leftSpeed>-30)leftSpeed=0;
      leftServo.writeMicroseconds(center+leftSpeed);
      rightServo.writeMicroseconds(center-rightSpeed);
      if(count>4){
        Serial1.flush();
        number="";
        reading=0;
        count=0;
      }
        
    }
  }
}
