// COMP-10184 – Mohawk College
// PIR Sensor Alarm System
//
// This program acts as a home alarm system, 
// when the user waves their hand near the sensor a 10 second countdown will start.
// If the button is pressed in time before the countdown ends the alarm is disabled, 
// else the alarm will turn into a solid led. 
// Before the user leaves the house, they can reenable the alarm by clicking the button. 
//
// @author     Luca Quacquarelli
// @id         000838997
//
// I, Luca Quacquarelli, 000838997 certify that this material is my original work.  
// No other person's work has been used without due acknowledgement.
//

#include <Arduino.h>
// digital input pin definitions
#define PIN_PIR D5
#define PIN_BUTTON D6

// define all possible alarm states.
// Alarm disabled state
#define ALARM_DISABLED    0
// Alarm enabled state
#define ALARM_ENABLE      1
// Alarm countdown state
#define ALARM_COUNTDOWN   2
// Alarm active state
#define ALARM_ACTIVE      3

// define all possible delays.
// delay at the end of the loop 
#define BUTTON_DELAY      200
// delay between flashing LED's 
#define LED_DELAY         150
// roughly 10 second delay for the alarm 
#define ALARM_DELAY       20
// delay between disabling the alarm 
#define TIMER_DELAY       1

// controls time in milliseconds 
unsigned long iControlTimer = 0;
// switch each alarm state it's default is to be enabled
int iAlarmState = ALARM_ENABLE;

// store movement detections 
int iMyMovement = 0;
// store button detections 
int iButtonState = 0;

unsigned long pirTimer;
// time in milliseconds until the next pir scan takes place
int iPirScanDelay = 100;


// *********************************************************** 
// The collectInputs function gets the button and motion detectors inputs 
// and controls them with a milius timer.
// 
void collectInputs() { 
  if(millis() > pirTimer) {
    pirTimer = millis() + iPirScanDelay;
    iMyMovement = digitalRead(PIN_PIR);
    iButtonState = digitalRead(PIN_BUTTON);
  }// end if
}// end collectInputs()


// *********************************************************** 
// The checkAlarmState function uses a switch case to transition between alarm stages.  
// Right after it detects movement it goes into the countdown stage 
// and stays there till the user either presses a button or the timer runs out. 
// 
void checkAlarmState() { 

    switch(iAlarmState) {

          case ALARM_ENABLE:// Alarm is enabled
            // if motion is detected, Continue to the countdown stage 
            if(iMyMovement > 0) {
              iAlarmState = ALARM_COUNTDOWN;
            }// end if #1
            else { // else the LED is off, and continue trying to find movement 
              digitalWrite(LED_BUILTIN,HIGH); 
              iAlarmState = ALARM_ENABLE;  
            }// end else
            break;

          case ALARM_COUNTDOWN:// Alarm is at countdown stage
            if(millis() > iControlTimer) {
              Serial.println("Motion Detected! - Alarm will sound in 10s!");
              iControlTimer = iControlTimer + ALARM_DELAY;//10 seconds to press button
              
              // Start the countdown
              while (iControlTimer != 0) {
                digitalWrite(LED_BUILTIN, LOW);    // sets the LED on
                delay(LED_DELAY); 
                digitalWrite(LED_BUILTIN, HIGH);   // sets the LED off
                delay(LED_DELAY); 
                iControlTimer--;// Countdown decreasing 

                // for test purposes but needs to stay put 
                // Serial.print("iControlTimer: ");
                // Serial.println(iControlTimer);

                // If the button is pressed
                if (digitalRead(PIN_BUTTON) == LOW) {
                  delay(BUTTON_DELAY); // short delay 
                  iAlarmState = ALARM_DISABLED; 
                  Serial.println("Button pressed: Alarm Disabled");

                  // setting the timer to a complete stop 
                  iControlTimer = TIMER_DELAY; 
                  if (iControlTimer == TIMER_DELAY) {
                    break;
                  }// end if #4
                  
                }// end if #3
                
              }// end while loop
              // if no time is left, the alarm has been activated 
              if (iControlTimer == 0) {
                  Serial.println("Alarm activated!");
                  iAlarmState = ALARM_ACTIVE;
              }// end if #5 
                
            }// end if #2  
            break;

          case ALARM_ACTIVE:// alarm is activated
            if(millis() > iControlTimer) {
              // LED stays on
              digitalWrite(LED_BUILTIN, LOW); 
            }// end if #6
            break;

          case ALARM_DISABLED:// Alarm is disabled
            if(millis() > iControlTimer) {
              // if the button is pressed the alarm is reenabled
              if (digitalRead(PIN_BUTTON) == LOW) {
                Serial.println("Button pressed: Enabled");
                iAlarmState = ALARM_ENABLE;
              }// end if #7
              else { // else stays disabled
                iAlarmState = ALARM_DISABLED;
              }// end else
            }// end if #6
            break;

          default:
            Serial.println("Error...invalid state");
            break;

    }// end switch case
}// end checkAlarmState()


// *************************************************************
void setup() {
  // configure the USB serial monitor
  Serial.begin(115200);
  // configure the LED output
  pinMode(LED_BUILTIN, OUTPUT);
  // PIR sensor is an INPUT
  pinMode(PIN_PIR, INPUT);
  // Button is an INPUT PULLUP
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  // startup messages 
  Serial.println();
  Serial.println("COMP-10184 - Alarm System");
  Serial.println("Name: Luca Quacquarelli");
  Serial.println("Student ID: 000838997");
  Serial.println();

  analogWriteRange(1023); //set default PWM range
}// end setup()
// *************************************************************
void loop() {
  
  // get button and motion detector inputs
  collectInputs(); //checks for movement

  // should we arm/activate/enable/disable the alarm?
  checkAlarmState(); //function controls the system states
  
  // wait before trying again 
  delay(BUTTON_DELAY);
}// end loop()
