/*
SHIV Box Final Code (Last updated 5/22/14)
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

//ADXL345 Setup and Calibration Values
int xOffset;
int yOffset;
int offsetsAcquired = 0;
int wirelessX;
int wirelessY;
//END of ADXL345 Setup and Calibration Values

//Sabertooth Declarations
#include <SoftwareSerial.h>
#include <Sabertooth.h>

SoftwareSerial SWSerial(NOT_A_PIN, 6); // RX on no pin (unused), TX on pin 11 (to S1).
Sabertooth ST(128, SWSerial); // Address 128, and use SWSerial as the serial port.

  //Calculate motors
  int finalX = 0;
  int finalY = 0;
  float x;
  float y;
  float targetX, targetY;
  float easing = 0.07;  //the closer the number is to 1, the less the easing
  
  
void setup()
{
  //More LCD setup stuff. Don't touch.
  lcd.begin (16,2);
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home ();                   // go home
  //End of LCD declarations.
  
  //Serial.begin(9600);
  delay(1000);
  //Serial.println("Nrf24L01 Receiver Starting");
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();;
  
  //Write Welcome LCD Message
  lcd.clear();
  lcd.print("   Welcome to   "); 
  lcd.setCursor ( 0, 1 );
  lcd.print("     SHIV   ");
  delay(2000);
  lcd.clear();
  
  SWSerial.begin(9600);
  ST.autobaud();
  
}//--(end setup )---


void loop()
{
  
  if ( radio.available() )
  {
    // Read the data payload until we've received everything
    bool done = false;
    while (!done)
    {
      // Fetch the data payload
      done = radio.read( joystick, sizeof(joystick) );
      wirelessX = (joystick[0]) * 1.15;
      wirelessY = (joystick[1] * -1) * 1.15;
      
      finalX = map(constrain(wirelessX, -200, 200), -200, 200, -127, 127);
      finalY = map(constrain(wirelessY, -200, 200), -500, 500, -127, 127);
      
      targetX = (finalX + finalY);
      float dx = (finalX + finalY) - x;
      if(abs(dx) > 1){
        x += dx * easing;
      }
      targetY = (finalY - finalX);
      float dy = (finalY - finalX) - y;
      if(abs(dy) > 1){
        y += dy * easing;
      }
      
    
      ST.motor(1, x);  // Send command
      delay(5);
      ST.motor(2, y);  // Send command
      delay(5);
      
      lcd.home ();                   // go home
      lcd.print("Status: Driving   "); 
      lcd.setCursor ( 0, 1 );
      lcd.print("L% ");
      lcd.print((int)x);
      lcd.print("    ");
      lcd.setCursor ( 8, 1 );
      lcd.print("R% ");
      lcd.print((int)y);
      lcd.print("    ");

    
    
    }
  }
  else
  {    
      //Serial.println("No radio available");
  }

}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/


