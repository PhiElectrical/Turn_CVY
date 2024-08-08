#define VERSION "Turn v1.0"

#include <avr/wdt.h>
#include <Arduino.h>
#include <EasyButton.h>
#include <BasicStepperDriver.h>

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 1000;           // interval at which to blink (milliseconds)

// this pin should connect to Ground when want to command the motor

#define For1_pin        35    //ch1 Belt run forward
#define For2_pin        35    //ch2 Belt run forward

#define CW5_pin         17    //CW_90
#define CCW5_pin        16    //CCW_90
#define JF5_pin         23    //Jog CW ch5
#define JR5_pin         27    //Jog CCW ch5

#define POLL 1

// Instance of the button.
EasyButton Forward1(For1_pin);
EasyButton Forward2(For2_pin);

EasyButton Forward5_90(CW5_pin);
EasyButton Reverse5_90(CCW5_pin);
EasyButton JF5(JF5_pin);
EasyButton JR5(JR5_pin);

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200

#define RPM       150
#define J_RPM     5

// Acceleration and deceleration values are always in FULL steps / s^2
#define MOTOR_ACCEL 4000    //1000  // แก้ความเร่ง motor ทีจุดนี้
#define MOTOR_DECEL 4000    //1000  // แก้ความเร่ง motor ทีจุดนี้

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Microstepping mode. If you hardwired it to save pins, set to the same value here.
#define MICROSTEPS    8 //1
#define MICROSTEPS_T  2

#define DIR1    34    //motor1
#define STEP1   36
#define ENABLE1 30

#define DIR2    28    //motor2
#define STEP2   26
#define ENABLE2 24

#define DIR3    48    //motor3
#define STEP3   46
#define ENABLE3 62

#define DIR4    61    //motor4
#define STEP4   60
#define ENABLE4 56

#define DIR5    55    //motor5
#define STEP5   54
#define ENABLE5 38

#include "DRV8825.h"
#define MODE0 PIN_UNCONNECTED
#define MODE1 PIN_UNCONNECTED
#define MODE2 PIN_UNCONNECTED
 
DRV8825 stepper1(MOTOR_STEPS, DIR1, STEP1, ENABLE1, MODE0, MODE1, MODE2);
DRV8825 stepper2(MOTOR_STEPS, DIR2, STEP2, ENABLE2, MODE0, MODE1, MODE2);
//DRV8825 stepper3(MOTOR_STEPS, DIR3, STEP3, ENABLE3, MODE0, MODE1, MODE2);
//DRV8825 stepper4(MOTOR_STEPS, DIR4, STEP4, ENABLE4, MODE0, MODE1, MODE2);
DRV8825 stepper5(MOTOR_STEPS, DIR5, STEP5, ENABLE5, MODE0, MODE1, MODE2);

bool fTurn = false;

// Belt run on ch1 =========================================================

void onPressedForDuration1_For() {
  stepper1.enable();
  stepper1.startRotate( 1000000* 360);
  Serial.print("motor1 Forward");
}

// Belt run on ch2 =========================================================

void onPressedForDuration2_For() {
  stepper2.enable();
  stepper2.startRotate( 1000000* 360);
  Serial.print("motor2 Forward");
}

// Turn run on ch5 =========================================================
// why test set gear ration 2/1 from normally 4/1

void onPressedForDuration5_For_90() {  //CW
  fTurn = true;
  stepper5.setSpeedProfile(stepper5.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
  stepper5.begin(RPM, MICROSTEPS_T);
  stepper5.enable();
  stepper5.startRotate(-360); //-360);  //(-90 * 4);
  Serial.print("motor5 Forward_90");
}

// Reverse90 ===============================================================

void onPressedForDuration5_Rev_90() {  //CCW
  fTurn = true;
  stepper5.setSpeedProfile(stepper5.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
  stepper5.begin(RPM, MICROSTEPS_T);
  stepper5.enable();
  stepper5.startRotate(360);  //360);  //(90 * 4);
  Serial.print("motor5 Reverse_90");
}

// JOG  ====================================================================

void onPressedForDuration5_JF() { //CW
  stepper5.setSpeedProfile(stepper5.CONSTANT_SPEED, 1000, 1000);
  stepper5.begin(J_RPM, MICROSTEPS_T);
  stepper5.enable();
  stepper5.startRotate(- 14500000);
  Serial.print("motor5 JF");
}


void onPressedForDuration5_JR() { //CCW
  stepper5.setSpeedProfile(stepper5.CONSTANT_SPEED, 1000, 1000);
  stepper5.begin(J_RPM, MICROSTEPS_T);
  stepper5.enable();
  stepper5.startRotate(14500000);
  Serial.print("motor5 JR");
}

int data;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  Serial.println(VERSION);

  //pinMode(For1_pin, INPUT_PULLUP); 
  pinMode(For2_pin, INPUT_PULLUP);
   
  pinMode(CW5_pin, INPUT_PULLUP);
  pinMode(CCW5_pin, INPUT_PULLUP); 
  pinMode(JF5_pin, INPUT_PULLUP);
  pinMode(JR5_pin, INPUT_PULLUP);

  Forward1.begin();
  Forward1.onPressedFor(100, onPressedForDuration1_For);
  
  Forward2.begin();
  Forward2.onPressedFor(100, onPressedForDuration2_For);
   
  Forward5_90.begin();
  Forward5_90.onPressedFor(100, onPressedForDuration5_For_90);

  Reverse5_90.begin();
  Reverse5_90.onPressedFor(100, onPressedForDuration5_Rev_90);

  JF5.begin();
  JF5.onPressedFor(100, onPressedForDuration5_JF); 
  
  JR5.begin();
  JR5.onPressedFor(100, onPressedForDuration5_JR);

  stepper1.setEnableActiveState(LOW); //cvy1
  stepper2.setEnableActiveState(LOW); //T cvy2
  stepper5.setEnableActiveState(LOW); //T

  stepper1.setSpeedProfile(stepper1.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
  stepper1.begin(RPM, MICROSTEPS);
//  stepper1.enable();

  stepper2.setSpeedProfile(stepper2.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
  stepper2.begin(RPM, MICROSTEPS);
//  stepper2.enable();

  Serial.print("STARTED");
}

void loop() {
  Forward1.read();
  Forward2.read();
  
  Forward5_90.read();
  Reverse5_90.read();
  JF5.read();
  JR5.read();
  
  if (Forward1.releasedFor(18))
  {
    stepper1.stop();
    stepper1.disable();
  }

  if (Forward2.releasedFor(18))
  {
    stepper2.stop();
    stepper2.disable();
  }

  if (fTurn == false && (JF5.releasedFor(18) && JR5.releasedFor(18)))
  {
     stepper5.stop();
  }

  //    motor control loop - send pulse and return how long to wait until next pulse
  stepper1.nextAction();
  stepper2.nextAction(); 
  unsigned wait_time = stepper5.nextAction();

  //HeartBeat
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

     if (fTurn == false) Serial.println("false");
     else Serial.println("true");
     Serial.println(wait_time);
  
    if (digitalRead(LED_BUILTIN))
      digitalWrite(LED_BUILTIN, LOW);
    else
      digitalWrite(LED_BUILTIN, HIGH);
  }

  if (fTurn == true)
  {    if (wait_time == 0)
          fTurn = false;
  }

  wdt_reset();

  //debug();
}

void debug()
{

    if (Serial.available()) {  
      char data = Serial.read();
    
     if (data == 'A') //cvy 1 DEBUG run
     {
        stepper1.setSpeedProfile(stepper1.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
        stepper1.begin(RPM, MICROSTEPS);
        stepper1.enable();
        stepper1.startRotate(10000000);
        Serial.print("A");
     }
     
     if (data == 'B') //T cvy 1 DEBUG run
     {
        stepper2.setSpeedProfile(stepper2.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
        stepper2.begin(RPM, MICROSTEPS);
        stepper2.enable();
        stepper2.startRotate(-10000000);
        Serial.print("B");
     }

     if (data == 'C') //T cvy DEBUG CW
     {
        stepper5.setSpeedProfile(stepper5.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
        stepper5.begin(RPM, MICROSTEPS_T);
        stepper5.enable();
        stepper5.startRotate(-360);     //(-360); //-90 * 4);
        Serial.print("C");
     }
     
     if (data == 'D') //T cvy DEBUG CCW
     {
        stepper5.setSpeedProfile(stepper5.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
        stepper5.begin(RPM, MICROSTEPS_T);
        stepper5.enable();
        stepper5.startRotate(360);  //90 * 4);
        Serial.print("D");
        
     }

     if (data == 'E') //T cvy DEBUG JF
     {
        stepper5.setSpeedProfile(stepper5.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
        stepper5.begin(J_RPM, MICROSTEPS_T);
        stepper5.enable();
        stepper5.startRotate(- 1000000);
        Serial.print("E");
     }
     
     if (data == 'F') //T cvy DEBUG JR
     {
        stepper5.setSpeedProfile(stepper5.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
        stepper5.begin(J_RPM, MICROSTEPS_T);
        stepper5.enable();
        stepper5.startRotate(1000000);
        Serial.print("F");
     }

     if (data == 'G') //T cvy SVOF
     {
        stepper5.disable();
        Serial.print("G");
     }
       if (data == 'C') //T cvy DEBUG CW
     {
        stepper5.setSpeedProfile(stepper5.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
        stepper5.begin(RPM, MICROSTEPS_T);
        stepper5.enable();
        stepper5.startRotate(-364);     //(-360); //-90 * 4);
        Serial.print("C");
     }
     
     if (data == 'D') //T cvy DEBUG CCW
     {
        stepper5.setSpeedProfile(stepper5.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
        stepper5.begin(RPM, MICROSTEPS_T);
        stepper5.enable();
        stepper5.startRotate(364);  //90 * 4);
        Serial.print("D");
        
     }
  }
}
