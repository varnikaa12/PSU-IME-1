#include <Arduino.h>
#include <ESP32Servo.h>
#include <AccelStepper.h>

// PIN stuff 
const int DISTANCE_SWITCH = 4;
const int SERVO_1_PIN = 18; // White Gate
const int SERVO_2_PIN = 19; // Red Gate
const int SERVO_3_PIN = 22; // Blue Gate

// CALIBRATION - need to adjust these based on actual measurements 
const int CLICKS_TO_WHITE = 50;  // num Clicks from camera to Gate 1
const int CLICKS_TO_RED = 100;   // num Clicks from camera to Gate 2
const int CLICKS_TO_BLUE = 150;  // num Clicks from camera to Gate 3

const int GATE_CLOSED = 0;
const int GATE_OPEN = 126;       // 70% of 180 degrees
const unsigned long GATE_HOLD_TIME = 1000; // Time (ms) gate stays open

//  OBJECTS 
AccelStepper beltStepper(1, 14, 12); // (Type:Driver, StepPin, DirPin)
Servo gateWhite;
Servo gateRed;
Servo gateBlue;

// TRACKING VARIABLES 
// 'volatile' tells the ESP32 this variable changes in the background (via interrupt)
volatile unsigned long totalClicks = 0; 

// Struct to define a tracked puck (adding in queue for later))
struct TrackedPuck {
  char color;
  unsigned long targetClick;
};

// A queue to hold up (say 10 for now) pucks currently on the belt
const int MAX_PUCKS = 10;
TrackedPuck puckQueue[MAX_PUCKS];
int activePucks = 0;

// Variables to handle closing the gates (after certain amt of time)
unsigned long gateWhiteCloseTime = 0;
unsigned long gateRedCloseTime = 0;
unsigned long gateBlueCloseTime = 0;


// INTERRUPT SERVICE ROUTINE 
// This runs instantly every time the switch clicks
void IRAM_ATTR countClicks() {
  totalClicks++;
}

void setup() {
  Serial.begin(115200);

  // Setup Distance Switch Interrupt
  pinMode(DISTANCE_SWITCH, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(DISTANCE_SWITCH), countClicks, RISING);

  // Setup Stepper (XY42STH34-0354A)
  beltStepper.setMaxSpeed(1000); 
  beltStepper.setSpeed(600); // Constant running speed

  // Setup Servos (Adafruit 169 limits)
  gateWhite.attach(SERVO_1_PIN, 500, 2400);
  gateRed.attach(SERVO_2_PIN, 500, 2400);
  gateBlue.attach(SERVO_3_PIN, 500, 2400);
  
  // Initializing gates to closed
  gateWhite.write(GATE_CLOSED);
  gateRed.write(GATE_CLOSED);
  gateBlue.write(GATE_CLOSED);
}

void loop() {
  // KEEP THE BELT MOVING //for this, it needs to stop under the camera for color sensing
  //todo change
  beltStepper.runSpeed();

  // CHECK FOR REVPI COMMANDS for puck info
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    if ((cmd == 'W' || cmd == 'R' || cmd == 'B') && activePucks < MAX_PUCKS) {
      // Calculate where this specific puck needs to go
      unsigned long target = 0;
      if (cmd == 'W') target = totalClicks + CLICKS_TO_WHITE;
      if (cmd == 'R') target = totalClicks + CLICKS_TO_RED;
      if (cmd == 'B') target = totalClicks + CLICKS_TO_BLUE;

      // Add to the tracking queue
      puckQueue[activePucks].color = cmd;
      puckQueue[activePucks].targetClick = target;
      activePucks++;
      
      Serial.println("Puck Added to Queue");
    }
  }

  //  CHECK IF ANY PUCK HAS REACHED ITS GATE
  for (int i = 0; i < activePucks; i++) {
    if (totalClicks >= puckQueue[i].targetClick) {
      
      // signal to correct gate and set its closing timer
      if (puckQueue[i].color == 'W') {
        gateWhite.write(GATE_OPEN);
        gateWhiteCloseTime = millis() + GATE_HOLD_TIME;
      } 
      else if (puckQueue[i].color == 'R') {
        gateRed.write(GATE_OPEN);
        gateRedCloseTime = millis() + GATE_HOLD_TIME;
      } 
      else if (puckQueue[i].color == 'B') {
        gateBlue.write(GATE_OPEN);
        gateBlueCloseTime = millis() + GATE_HOLD_TIME;
      }

      // Remove puck from list w shift down
      for (int j = i; j < activePucks - 1; j++) {
        puckQueue[j] = puckQueue[j + 1];
      }
      activePucks--;
      i--; // Adjust index since we shifted array
    }
  }

  // CLOSE GATES after time 
  unsigned long currentTime = millis();
  
  if (gateWhiteCloseTime > 0 && currentTime >= gateWhiteCloseTime) {
    gateWhite.write(GATE_CLOSED);
    gateWhiteCloseTime = 0;
  }
  if (gateRedCloseTime > 0 && currentTime >= gateRedCloseTime) {
    gateRed.write(GATE_CLOSED);
    gateRedCloseTime = 0;
  }
  if (gateBlueCloseTime > 0 && currentTime >= gateBlueCloseTime) {
    gateBlue.write(GATE_CLOSED);
    gateBlueCloseTime = 0;
  }
}