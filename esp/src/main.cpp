#include <Arduino.h>
#include <ESP32Servo.h>
#include <AccelStepper.h>

// PIN STUFF
// Stepper
const int STEPPER_STEP_PIN   = 23;
const int STEPPER_DIR_PIN    = 27;
//const int STEPPER_ENABLE_PIN = 26; //controlled by RevPi to switch conveyor on/off

// RevPi Digital Outputs
const int WHITE_SERVO_CTRL = 39; //uses pin for 5th breakbeam, control signal from RevPi for white servo acutation
const int RED_SERVO_CTRL = 32; //uses pin for 6th breakbeam, control signal from RevPi for red servo acutation
//const int BLUE_SERVO_CTRL = 21 //uses pin for 4th servo, control signal from RevPi for blue servo acutation

static int lastWhiteState = -1;
static int lastRedState = -1;
static int lastBlueState = -1;


// Servos
const int SERVO_1_PIN = 18; // White Gate
const int SERVO_2_PIN = 19; // Red Gate
//const int SERVO_3_PIN = 20; // Blue Gate, GPIO20 does not exist on ESP, unable to be implemented at this pin


const int GATE_CLOSED = 0;
const int GATE_OPEN   = 126;

// HARDWARE 
AccelStepper beltStepper(AccelStepper::DRIVER, STEPPER_STEP_PIN, STEPPER_DIR_PIN);
Servo gateWhite;
Servo gateRed;
//Servo gateBlue;

void setup() {

  // Stepper setup
  //pinMode(STEPPER_ENABLE_PIN, OUTPUT);
  //digitalWrite(STEPPER_ENABLE_PIN, HIGH);

  beltStepper.setMaxSpeed(1000);
  beltStepper.setSpeed(50); //counts per second, stepper motor has 200 counts/rev 

  pinMode(WHITE_SERVO_CTRL, INPUT);
  pinMode(RED_SERVO_CTRL, INPUT);
  //pinMode(BLUE_SERVO_CTRL, INPUT);

  // Servos
  gateWhite.attach(SERVO_1_PIN, 500, 2400);
  gateRed.attach(SERVO_2_PIN,   500, 2400);
  //gateBlue.attach(SERVO_3_PIN,  500, 2400);

  gateWhite.write(GATE_CLOSED);
  gateRed.write(GATE_CLOSED);
  //gateBlue.write(GATE_CLOSED);

  lastWhiteState = digitalRead(WHITE_SERVO_CTRL);
  lastRedState   = digitalRead(RED_SERVO_CTRL);
  //lastBlueState  = digitalRead(BLUE_SERVO_CTRL);
}

void loop() {
  int currentWhiteState = digitalRead(WHITE_SERVO_CTRL);
  int currentRedState = digitalRead(RED_SERVO_CTRL);
  //int currentBlueState = digitalRead(BLUE_SERVO_CTRL);

  if (currentWhiteState != lastWhiteState) { //only update servo state when control signal changes
    if (currentWhiteState == LOW) {
      gateWhite.write(GATE_CLOSED);
    } else {
      gateWhite.write(GATE_OPEN);
    }
    lastWhiteState = currentWhiteState;
  }
  if (currentRedState != lastRedState) {
    if (currentRedState == LOW) {
      gateRed.write(GATE_CLOSED);
    } else {
      gateRed.write(GATE_OPEN);
    }
    lastRedState = currentRedState;
  }

  beltStepper.runSpeed(); //stepper runs at constant speed

}
