#include <Arduino.h>
#include <ESP32Servo.h>
#include <AccelStepper.h>

// PIN STUFF
// Stepper
const int STEPPER_STEP_PIN   = 23;
const int STEPPER_DIR_PIN    = 27;
const int STEPPER_ENABLE_PIN = 26;

/*
// Break beam sensors (active LOW — beam broken = pin goes LOW)
const int BREAKBEAM_1 = 33; // before camera one
const int BREAKBEAM_2 = 34;
const int BREAKBEAM_3 = 35;
const int BREAKBEAM_4 = 36;
const int BREAKBEAM_5 = 39;
const int BREAKBEAM_6 = 32; // GPIO32 listed as stepper motor- double-check
*/

// RevPi Digital Outputs
const int WHITE_SERVO_CTRL = 32;
const int RED_SERVO_CTRL = 33;
const int BLUE_SERVO_CTRL = 25;

static int lastWhiteState = -1;
static int lastRedState = -1;
static int lastBlueState = -1;


// Servos
const int SERVO_1_PIN = 18; // White Gate
const int SERVO_2_PIN = 19; // Red Gate
const int SERVO_3_PIN = 21; // Blue Gate -- changed this, check if its okay
//const int SERVO_4_PIN = 21;

/*
// CALIBRATION ( TODO: need to adjust these later) 
const int CLICKS_TO_CAMERA = 50;     
const int CLICKS_TO_WHITE  = 500;
const int CLICKS_TO_RED    = 800;
const int CLICKS_TO_BLUE   = 900;
*/

const int GATE_CLOSED = 0;
const int GATE_OPEN   = 126;
/*
const unsigned long GATE_HOLD_TIME   = 1000; // ?
const unsigned long CAMERA_PAUSE_TIME = 1000; // ?
*/

// HARDWARE 
AccelStepper beltStepper(AccelStepper::DRIVER, STEPPER_STEP_PIN, STEPPER_DIR_PIN);
Servo gateWhite;
Servo gateRed;
Servo gateBlue;

// CLICK COUNTING
// totalClicks - incremented by the stepper step clock (GPIO25),
// pulses once per step- distance counter.
/*
volatile unsigned long totalClicks = 0;
volatile bool beltRunning = true;

void IRAM_ATTR countClicks() {
  if (beltRunning) {
    totalClicks++;
  }
}
*/


/*
// PUCK QUEUE 
struct TrackedPuck {
  char color;
  unsigned long targetClick;
};

const int MAX_PUCKS = 10;
TrackedPuck puckQueue[MAX_PUCKS];
int activePucks = 0;
*/

/*
// GATE CLOSE TIMERS 
unsigned long gateWhiteCloseTime = 0; // ?
unsigned long gateRedCloseTime   = 0; // ?
unsigned long gateBlueCloseTime  = 0; // ?
*/

/*
// CAMERA PAUSE STATE
bool          pausePending     = false; // ?
unsigned long pauseTargetClick = 0; // ?
bool          beltPaused       = false; // ?
unsigned long pauseEndTime     = 0; // ?
*/

/*
// BREAK BEAM
void IRAM_ATTR onBreakBeam1() {
  // when beam is broken by an incoming puck.
  if (!beltPaused && !pausePending) {
    pausePending     = true;
    pauseTargetClick = totalClicks + CLICKS_TO_CAMERA;
  }
}
*/

void setup() {
  /*
  Serial.begin(115200);
  */

  // Stepper setup
  pinMode(STEPPER_ENABLE_PIN, OUTPUT);
  digitalWrite(STEPPER_ENABLE_PIN, LOW);
  beltStepper.setMaxSpeed(1000);
  beltStepper.setSpeed(600);

  
  // Step clock counter
  /*
  pinMode(25, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(25), countClicks, RISING);
  */
  

  /*
  // Break beams
  // GPIO33 = normal pin, internal pullup is fine
  pinMode(BREAKBEAM_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BREAKBEAM_1), onBreakBeam1, FALLING);

  // GPIO34, 35, 36, 39 = input-only pins, NO internal pullup allowed
  pinMode(BREAKBEAM_2, INPUT);   // GPIO34
  pinMode(BREAKBEAM_3, INPUT);   // GPIO35
  pinMode(BREAKBEAM_4, INPUT);   // GPIO36
  pinMode(BREAKBEAM_5, INPUT);   // GPIO39
  */

  pinMode(WHITE_SERVO_CTRL, INPUT);
  pinMode(RED_SERVO_CTRL, INPUT);
  pinMode(BLUE_SERVO_CTRL, INPUT);

  // Servos
  gateWhite.attach(SERVO_1_PIN, 500, 2400);
  gateRed.attach(SERVO_2_PIN,   500, 2400);
  gateBlue.attach(SERVO_3_PIN,  500, 2400);

  gateWhite.write(GATE_CLOSED);
  gateRed.write(GATE_CLOSED);
  gateBlue.write(GATE_CLOSED);

  lastWhiteState = digitalRead(WHITE_SERVO_CTRL);
  lastRedState   = digitalRead(RED_SERVO_CTRL);
  lastBlueState  = digitalRead(BLUE_SERVO_CTRL);
}

void loop() {
  int currentWhiteState = digitalRead(WHITE_SERVO_CTRL);
  int currentRedState = digitalRead(RED_SERVO_CTRL);
  int currentBlueState = digitalRead(BLUE_SERVO_CTRL);

  if (currentWhiteState != lastWhiteState) {
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
  if (currentBlueState != lastBlueState) {
    if (currentBlueState == LOW) {
      gateBlue.write(GATE_CLOSED);
    } else {
      gateBlue.write(GATE_OPEN);
    }
    lastBlueState = currentBlueState;
  }

  // CAMERA PAUSING

  // Puck has done CLICKS_TO_CAMERA steps past the beam -- so we stop now
  /*
  if (pausePending && totalClicks >= pauseTargetClick) {
    pausePending  = false;
    beltPaused    = true;
    beltRunning   = false;                    // stop click counting
    pauseEndTime  = millis() + CAMERA_PAUSE_TIME;
    beltStepper.stop();
    digitalWrite(STEPPER_ENABLE_PIN, HIGH);
    Serial.println("Belt paused for camera.");
  }
  */

  /*
  // Pause timer expired -- resume belt
  if (beltPaused && millis() >= pauseEndTime) {
    beltPaused  = false;
    beltRunning = true;
    digitalWrite(STEPPER_ENABLE_PIN, LOW);    // Re-enable driver
    beltStepper.setSpeed(600);                // stop() clears speed, reset
    Serial.println("Belt resumed.");
  }
  */

  // RUN BELT (skipped while paused)
  beltStepper.runSpeed();

  //  REVPI COMMANDS
  /*
  if (Serial.available() > 0) { // ? reading the serial output of the revpi?
    char cmd = Serial.read();

    if ((cmd == 'W' || cmd == 'R' || cmd == 'B') && activePucks < MAX_PUCKS) {
      unsigned long target = 0;
      if (cmd == 'W') target = totalClicks + CLICKS_TO_WHITE;
      if (cmd == 'R') target = totalClicks + CLICKS_TO_RED;
      if (cmd == 'B') target = totalClicks + CLICKS_TO_BLUE;

      puckQueue[activePucks].color       = cmd;
      puckQueue[activePucks].targetClick = target;
      activePucks++;

      Serial.println("Puck Added to Queue!");
    }
  */

  // GATE TRIGGER CHECK
  /*
  for (int i = 0; i < activePucks; i++) {
    if (totalClicks >= puckQueue[i].targetClick) {

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

      // Shift queue down
      for (int j = i; j < activePucks - 1; j++) {
        puckQueue[j] = puckQueue[j + 1];
      }
      activePucks--;
      i--;
    }
  }
  */

  // CLOSE GATES
  /*
  unsigned long now = millis();

  if (gateWhiteCloseTime > 0 && now >= gateWhiteCloseTime) {
    gateWhite.write(GATE_CLOSED);
    gateWhiteCloseTime = 0;
  }
  if (gateRedCloseTime > 0 && now >= gateRedCloseTime) {
    gateRed.write(GATE_CLOSED);
    gateRedCloseTime = 0;
  }
  if (gateBlueCloseTime > 0 && now >= gateBlueCloseTime) {
    gateBlue.write(GATE_CLOSED);
    gateBlueCloseTime = 0;
  }
  */
}
