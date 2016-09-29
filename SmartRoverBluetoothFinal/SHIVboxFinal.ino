
/*
SmartRover v3 Box Final Code (Last updated 4/7/16)
Code written by Dominick Lee.
Distributed until the GPL v2 License for free use.
For more information, please visit:  http://lifebeam.net/SHIV
*/

//This is LCD declarations stuff. Don't touch.
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#define I2C_ADDR    0x27  // Define I2C Address where the PCF8574A is
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
int n = 1;
LiquidCrystal_I2C	lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
//End of LCD declarations.


//Sabertooth Declarations
#include <SoftwareSerial.h>
#include <Sabertooth.h>

int incomingByte = 0;   // for incoming serial data

SoftwareSerial SWSerial(NOT_A_PIN, 6); // RX on no pin (unused), TX on pin 11 (to S1).
Sabertooth ST(128, SWSerial); // Address 128, and use SWSerial as the serial port.

SoftwareSerial BTreceiver(2, 3);

//Calculate motors
int finalX = 0;
int finalY = 0;
int wirelessX;
int wirelessY;

//Bluetooth connection safety (variables used to disable robot when Bluetooth drops)
unsigned long timerActive = 0;
  
void setup()
{
  //More LCD setup stuff. Don't touch.
  lcd.begin (16,2);
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home ();                   // go home
  //End of LCD declarations.
  delay(500);
  
  //Write Welcome LCD Message
  lcd.clear();
  lcd.print("   Welcome to   "); 
  lcd.setCursor ( 0, 1 );
  lcd.print("  SmartRover V2");
  delay(1000);
  
  Serial.begin(9600);        // Initialize serial communications with the PC
  //Serial.setTimeout(5);
  SWSerial.begin(9600);
  ST.autobaud();
  
  BTreceiver.begin(9600);
  BTreceiver.setTimeout(5);
  

  
}//--(end setup )---


void loop()
{
  
  if (BTreceiver.available())
  {
    incomingByte = BTreceiver.read();
    timerActive = millis();
    
    if (incomingByte == 108) {  // header r
    
      float x = getFloatFromSerialMonitor();
      wirelessX = (int)x;
      finalX = map(constrain(wirelessX, -100, 100), -120, 120, -90, 90);
      //SWSerial.listen();
      ST.motor(1, finalX);  // Send command
      delay(5);
    }
    else if (incomingByte == 114) {  // header l
    
      float x = getFloatFromSerialMonitor();
      wirelessY = (int)x;
      finalY = map(constrain(wirelessY, -100, 100), -120, 120, -90, 90);
      //SWSerial.listen();
      ST.motor(2, finalY);  // Send command
      delay(5);
    }
  }
  
  if (millis() - timerActive >= 1500UL)  //Bluetooth dropped more than 1.5 seconds
  {
      ST.motor(1, 0);  // Send command
      delay(5);
      ST.motor(2, 0);  // Send command
      delay(5);
  }
  
}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/

float getFloatFromSerialMonitor(){
  char inData[20];  
  float f=0;    
  int x=0;  
  while (x<1){  
  String str;   
  if (BTreceiver.available()) {
    delay(5); //lower the better
    int i=0;
    while (BTreceiver.available() > 0) {
     char  inByte = BTreceiver.read();
      str=str+inByte;
      inData[i]=inByte;
      i+=1;
      x=2;
    }
    f = atof(inData);
    memset(inData, 0, sizeof(inData));  
  }
  }//END WHILE X<1  
   return f; 
  }

