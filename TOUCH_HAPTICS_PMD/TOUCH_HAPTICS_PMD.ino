/***************************************************************************
 * Precision Microdrives
 * TOUCH_HAPTICS for Haptic Feedback Evaluation Kit 2 (HFEK2) and demos
 * by: Tristan Cool
 * September 2019 
 * Arduino UNO
***************************************************************************/

//*************************** INCLUDE **************************************//
#define __PROG_TYPES_COMPAT__
#include <avr/pgmspace.h>

#include <SPI.h>
//#include <mpr121.h>
#include <i2c.h>

#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

// Debug
#include "debug.h"

// Include our headers
#include "Metro.h"
#include "motor.h"

// Init timer for 2s, and auto-reset when we get a positive check
Metro timer( 2000, 1 );
Motor motor = Motor();

//*************************** WIRING  **************************************//
/* PMD Haptic Feedback Evaluation Kit (HFEK) in Developer Mode
 * MPR121 Vin - 3.3 or 5V
 * MPR121 GND - GND
 * MPR121 SCL - A5
 * MPR121 SDA - A4
 * MPR121 ADDR-3V
 */

//*************************** VARIABLES **************************************//
//Adafruit MPR121 touch board
Adafruit_MPR121 cap = Adafruit_MPR121();

//touch and release
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

// ******** FUNCTION PROTOTYPES ***********


//*************************** SETUP **************************************//
void setup()
{
  //setup Serial
  Serial.begin( 9600 );
  while (!Serial) 
  { 
    delay(10);
  }
  Serial.println(F("PRECISION MICRODRIVES - TOUCH_HAPTICS_PMD DEMO"));
  //Serial.print( F("FreeMem=") );
  //Serial.println( freeRAM() );

  //init DRV (i2c)
  //Default address: 0x5A, to 3.3V: 0x5B, to SDA: 0x5C, to SCL: 0x5D
  setupPins();
  i2cInit( 200 );

  //init MPR121 (i2c)
  // Default address: 0x5A, to 3.3V: 0x5B, to SDA: 0x5C, to SCL: 0x5D
  if (!cap.begin(0x5B)) {
    Serial.println("MPR121 not found....");
    while (1);
  }
  Serial.println("MPR121 found!");
  
  //Set up the motor
  motor.selectMotor(3); //LRA (solenoid)
  motor.autoCalibrate();

  //Ensure any time for calibration is ignored.
  timer.reset();

} //end:SETUP

//*************************** LOOP **************************************//
void loop()
{
  //currently touched pads
  currtouched = cap.touched();
 
  for (uint8_t i=0; i<12; i++) 
  {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) )
    {
      Serial.print(i); Serial.println(" touched");
      TouchEffect(); //TODO
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) 
    {
      Serial.print(i); Serial.println(" released");
    }
  }
  //reset touch state
  lasttouched = currtouched;

  //comment out this line for detailed data from the touch sensor
  return;
  //MPR121_debug();
  
} //end:LOOP


//*************************** FUNCTIONS **************************************//

void setupPins()
{
    pinMode( DRV_2605_EN,       OUTPUT );   // Output for DRV2605 Enable
    pinMode( PWM_OUT,           OUTPUT );   // Output for PWM
    pinMode( SRC_SEL,           OUTPUT );   // HIGH = DRV, LOW = MOS 
    pinMode( GRIP_SEL,          OUTPUT );   // LOW = Haptic Grip Connected
    pinMode( LRA_SEL,           OUTPUT );   // LOW = Haptic Shield LRA Connected
    pinMode( ERM_SEL,           OUTPUT );   // LOW = Haptic Shield ERM Connected

    pinMode( MOTOR_PIN_0,       OUTPUT );   // Motor select 1
    pinMode( MOTOR_PIN_1,       OUTPUT );   // Motor select 2
    
    digitalWrite( PWM_OUT,      LOW );      // PWM output low
    digitalWrite( SRC_SEL,      HIGH );     // Select DRV
    digitalWrite( GRIP_SEL,     HIGH  );    // Select none 
    digitalWrite( ERM_SEL,      HIGH  );    // Select none
    digitalWrite( LRA_SEL,      HIGH  );    // Select none 
} //end:setupPins

void MPR121_debug()
{
  // debugging info
  Serial.print("\t\t\t\t\t\t\t\t\t\t\t\t\t 0x"); Serial.println(cap.touched(), HEX);
  Serial.print("Filt: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print(cap.filteredData(i)); Serial.print("\t");
  }
  Serial.println();
  Serial.print("Base: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print(cap.baselineData(i)); Serial.print("\t");
  }
  Serial.println();
  delay(100);
} //end:MPR121_debug

void TouchEffect()
{
  //TODO
  motor.playFullHaptic( 1, 6);  //click
} //end:TouchEffect
