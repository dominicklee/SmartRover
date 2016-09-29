/*
SHIV Gauntlet Final Code (Last updated 5/22/14)
Code written by Dominick Lee.
Distributed under the GPL v2 License for free use.
For more information, please visit:  http://lifebeam.net/SHIV
*/

//Wireless Transceiver Declarations

  /*-----( Import needed libraries )-----*/
  #include <SPI.h>
  #include <nRF24L01.h>
  #include <RF24.h>
  /*-----( Declare Constants and Pin Numbers )-----*/
  #define CE_PIN   9
  #define CSN_PIN 10
  
  // NOTE: the "LL" at the end of the constant is "LongLong" type
  const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe
  
  /*-----( Declare objects )-----*/
  RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
  /*-----( Declare Variables )-----*/
  int joystick[2];  // 2 element array holding Joystick readings

//END of Wireless Transceiver Declarations

//ADXL345 Declarations
#include <Wire.h>
#include <ADXL345.h>
ADXL345 adxl; //variable adxl is an instance of the ADXL345 library
//END of ADXL345 Declarations

//ADXL345 Setup and Calibration Values 
int xOffset = 0;
int yOffset = 0;
//END of ADXL345 Setup and Calibration Values

//Sensor and LED pins
int fsrPin = 0;     // the FSR and 10K pulldown are connected to a0
int fsrReading;     // the analog reading from the FSR resistor divider
int redLED = 8;
int greenLED = 7;
//END of Sensor and LED pins


void setup(){
  
Serial.begin(9600); // Begins the serial
adxl.powerOn();     // Power up ADXL345

//Start up the Wireless Transceiver
  radio.begin();
  radio.openWritingPipe(pipe);
  
//Wait and Calibrate
int x,y,z;
adxl.readAccel(&x, &y, &z); // read the accelerometer values and store them in variables x,y,z
delay(2000);
xOffset = x;
yOffset = y;

//Declare LED Indicator
pinMode(redLED, OUTPUT);
pinMode(greenLED, OUTPUT);

//Turn on Red LED
digitalWrite(redLED, HIGH);
digitalWrite(greenLED, LOW);

}

void loop(){

int x,y,z;
adxl.readAccel(&x, &y, &z); // read the accelerometer values and store them in variables x,y,z
fsrReading = analogRead(fsrPin); // Get analog reading of force sensor
 
if (fsrReading < 700) {        // Not squeezed or barely squeezed.
  //Turn on Red LED
  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, LOW);
  
  /* Serial Debugging 
  Serial.print(0);
  Serial.print("  ");
  Serial.print(0);
  Serial.println();
  */
  
  //Send accelerometer final values to transceiver
  joystick[0] = 0;
  joystick[1] = 0;
  radio.write( joystick, sizeof(joystick) );
  
  
} else {                        //User squeezed it. Send gyro data.
  //Turn on Green LED
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, HIGH);
  
  
  // Serial Debugging 
  Serial.print(xVal);
  Serial.print("  ");
  Serial.print(yVal);
  Serial.println();
  //
  
  //Send accelerometer final values to transceiver
  joystick[0] = x;
  joystick[1] = y;

  radio.write( joystick, sizeof(joystick) );
  delay(5);
}


} //end void loop
