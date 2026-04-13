#include <Arduino.h>
#include <ESP32Servo.h>
#include <AccelStepper.h>

// --- PIN DEFINITIONS ---
const int DISTANCE_SWITCH = 4;
const int SERVO_1_PIN = 18; // White Gate
const int SERVO_2_PIN = 19; // Red Gate
const int SERVO_3_PIN = 22; // Blue Gate



// --- CALIBRATION (Adjust these!) ---
const int CLICKS_TO_WHITE = 500;  // Clicks from camera to Gate 1
const int CLICKS_TO_RED = 800;   // Clicks from camera to Gate 2
const int CLICKS_TO_BLUE = 1200;  // Clicks from camera to Gate 3

const int GATE_CLOSED = 0;
const int GATE_OPEN = 126;       // 70% of 180 degrees
const unsigned long GATE_HOLD_TIME = 1000; // Time (ms) gate stays open

// --- HARDWARE OBJECTS ---
AccelStepper beltStepper(1, 14, 12); // (Type:Driver, StepPin, DirPin)
Servo gateWhite;
Servo gateRed;
Servo gateBlue;

// --- TRACKING VARIABLES ---
volatile unsigned long totalClicks = 0; 

// struct to define a tracked puck
struct TrackedPuck {
  char color;
  unsigned long targetClick;
};

// A queue to hold ( 10 ) pucks currently on the belt
const int MAX_PUCKS = 10;
TrackedPuck puckQueue[MAX_PUCKS];
int activePucks = 0;

// Variables to handle closing the gates non-blockingly
unsigned long gateWhiteCloseTime = 0;
unsigned long gateRedCloseTime = 0;
unsigned long gateBlueCloseTime = 0;


// --- INTERRUPT SERVICE ROUTINE ---
// This runs automatically every time the switch clicks
void IRAM_ATTR countClicks() {
  totalClicks++;
}

void setup() {
  Serial.begin(115200);

  

  // Distance Switch Interrupt
  pinMode(DISTANCE_SWITCH, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(DISTANCE_SWITCH), countClicks, RISING);

  // Stepper (XY42STH34-0354A)
  beltStepper.setMaxSpeed(1000); 
  beltStepper.setSpeed(600); // Constant running speed
  //TODO need to add stop for when smth goes under camera
  //   counting clicks will stop and everything will essentially just pause (so just stop clicks or more too.. need to check) 
  // -- how long does camera need to make its decision?

  // Servos 
  gateWhite.attach(SERVO_1_PIN, 500, 2400);
  gateRed.attach(SERVO_2_PIN, 500, 2400);
  gateBlue.attach(SERVO_3_PIN, 500, 2400);
  
  // Initialize gates to closed
  gateWhite.write(GATE_CLOSED);
  gateRed.write(GATE_CLOSED);
  gateBlue.write(GATE_CLOSED);
}

void loop() {


  // KEEP THE BELT MOVING
  beltStepper.runSpeed();

  // CHECK FOR REVPI COMMANDS
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    if ((cmd == 'W' || cmd == 'R' || cmd == 'B') && activePucks < MAX_PUCKS) {
      // see where this specific puck needs to go
      unsigned long target = 0;
      if (cmd == 'W') target = totalClicks + CLICKS_TO_WHITE;
      if (cmd == 'R') target = totalClicks + CLICKS_TO_RED;
      if (cmd == 'B') target = totalClicks + CLICKS_TO_BLUE;

      // Add to the tracking queue
      puckQueue[activePucks].color = cmd;
      puckQueue[activePucks].targetClick = target;
      activePucks++;
      
      Serial.println("Puck Added to Queue!");
    }
  }

  // CHECK IF ANY PUCK HAS REACHED ITS GATE
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

      // Remove the puck from the list by shifting the rest down
      for (int j = i; j < activePucks - 1; j++) {
        puckQueue[j] = puckQueue[j + 1];
      }
      activePucks--;
      i--; 
    }
  }

  // CLOSE GATES IF THEIR TIMERS ARE UP
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
