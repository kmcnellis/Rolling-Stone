#include <Wire.h>
#include <string.h>
#include <Servo.h>

#undef int
#include <stdio.h>

uint8_t outbuf[6];		// array to store arduino output
int cnt = 0;
int ledPin = 13;
const int center = 1500;
Servo leftServo, rightServo;
signed int leftSpeed= 0;
signed int rightSpeed = 0;
int maxx = 0;
int maxy = 0;
int count = 0;
char nunchuk_decode_byte(char x);
void nunchuck_init();
void print();
void write_zero();

void setup(){
  Serial.begin (9600);
  Serial.print ("Finished setup\n");
  Wire.begin ();    // join i2c bus with address 0x52
  nunchuck_init (); // write the initilization handshake
  leftServo.attach(10);
  rightServo.attach(11);
  leftServo.writeMicroseconds(center);
  rightServo.writeMicroseconds(center);
}

void loop(){
  Wire.requestFrom (0x52, 6); // request data from nunchuck
  while (Wire.available ())
    {
      outbuf[cnt] = nunchuk_decode_byte (Wire.read());  // receive byte as an integer
      digitalWrite (ledPin, HIGH);  // sets the LED on
      cnt++;
    }

  // If we recieved the 6 bytes, then go print them
  if (cnt >= 5)
    {
      print ();
    }

  cnt = 0;
  write_zero (); // write the request for next bytes
  delay (100);
}

// Encode data to format that most wiimote drivers except
// only needed if you use one of the regular wiimote drivers
char nunchuk_decode_byte(char x){
  x = (x ^ 0x17) + 0x17;
  return x;
}

void nunchuck_init(){
  Wire.beginTransmission (0x52);  // transmit to device 0x52

  Wire.write(0x40);   // writes memory address
  Wire.write(0x00);   // writes sent a zero.
  Wire.endTransmission ();  // stop transmitting
}


// Print the input data we have recieved
// accel data is 10 bits long
// so we read 8 bits, then we have to add
// on the last 2 bits.  That is why I
// multiply them by 2 * 2
void print(){
  int joy_x_axis = outbuf[0];
  int joy_y_axis = outbuf[1];
  int accel_x_axis = outbuf[2] * 2 * 2;
  int accel_y_axis = outbuf[3] * 2 * 2;
  int accel_z_axis = outbuf[4] * 2 * 2;

  int z_button = 0;
  int c_button = 0;

 // byte outbuf[5] contains bits for z and c buttons
 // it also contains the least significant bits for the accelerometer data
 // so we have to check each bit of byte outbuf[5]
  if ((outbuf[5] >> 0) & 1)
    {
      z_button = 1;
    }
  if ((outbuf[5] >> 1) & 1)
    {
      c_button = 1;
    }

  if ((outbuf[5] >> 2) & 1)
    {
      accel_x_axis += 2;
    }
  if ((outbuf[5] >> 3) & 1)
    {
      accel_x_axis += 1;
    }

  if ((outbuf[5] >> 4) & 1)
    {
      accel_y_axis += 2;
    }
  if ((outbuf[5] >> 5) & 1)
    {
      accel_y_axis += 1;
    }

  if ((outbuf[5] >> 6) & 1)
    {
      accel_z_axis += 2;
    }
  if ((outbuf[5] >> 7) & 1)
    {
      accel_z_axis += 1;
    }
    
  leftSpeed=((accel_x_axis*20/512)-20)*10;
  rightSpeed=-leftSpeed;
  leftSpeed+=((10*accel_y_axis/512)-10)*10;
  rightSpeed+=((10*accel_y_axis/512)-10)*10;

  if(leftSpeed>200)leftSpeed=200;
  if(leftSpeed<-200)leftSpeed=-200;
  if(rightSpeed>200)rightSpeed=200;
  if(rightSpeed<-200)rightSpeed=-200;
  if(rightSpeed<30&&rightSpeed>-30)rightSpeed=0;
  if(leftSpeed<30&&leftSpeed>-30)leftSpeed=0;
  leftServo.writeMicroseconds(center+leftSpeed);
  rightServo.writeMicroseconds(center-rightSpeed);
  if (count > 100){
    if(accel_x_axis > maxx){
      maxx = accel_x_axis;
    }
    if(accel_y_axis > maxy){
      maxy = accel_y_axis;
    }
  }else{
    count +=1;
  }  
  Serial.print (joy_x_axis);
  Serial.print ("\t");

  Serial.print (joy_y_axis, DEC);
  Serial.print ("\t");

  Serial.print (accel_x_axis, DEC);
  Serial.print ("\t");

  Serial.print (accel_y_axis, DEC);
  Serial.print ("\t");

  Serial.print (accel_z_axis, DEC);
  Serial.print ("\t");

  Serial.print (z_button, DEC);
  Serial.print ("\t");

  Serial.print (c_button, DEC);
  Serial.print ("\t");

  Serial.print (maxx, DEC);
  Serial.print ("\t");
  Serial.print (maxy, DEC);
  Serial.print ("\t");

  Serial.print ("\r\n");
}

void write_zero(){
  Wire.beginTransmission (0x52);  // transmit to device 0x52
  Wire.write (0x00);    // writes one byte
  Wire.endTransmission ();  // stop transmitting
}
