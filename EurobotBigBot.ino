#include <Servo.h>
#include <TimerFour.h>
#include <TimerThree.h>

/////////////
// DEFINES //
/////////////

#define teamSwitch A5
#define starter 43

#define stPinL 13
#define stPinR 15
#define stDirPinL 12
#define stDirPinR 14
#define stDirDrum 3
#define stDrum 2
#define stEnDrum 4

#define TICK_COUNTS 4000

#define echoPinUS 18
#define frontTigUS 17
#define commonTrigUS 51

/////////////
// OBJECTS //
/////////////

Servo cannonGate;
Servo dropGate;

Servo winchServo;
Servo holderServo1;
Servo holderServo2;

//////////
// VARS //
//////////

int lastDrumPosition = 0;
int currentDrumPos = 0;
bool right;
bool left;
bool frontR;
bool frontL;
bool back;
volatile int count = 0;
volatile long echo_start = 0;
volatile int distance = 0;
volatile int trigger_time_count = 0;

////////////
// CONSTS //
////////////

const float tPerStep = 420.0; //minimal delay between the steps in µs
const double sPerStep = 0.00785; //distance driven by executing one step in cm
const float anglePerStep = 0.041043; //angle the Bot turns with a step in different directions

const String name = "C3PO";

//////////////////////
// HELPER FUNCTIONS //
//////////////////////

boolean gatherBalls() {
  Serial.println("Now gathering balls!");
  for (int i = 0; i <= 7; i++) {
    driveWithoutAvoidance(4);

    delay(100);

    turnDrumTo(i);

    delay(100);

    driveWithoutAvoidance(-4);

    delay(100);

    turnWithoutCoA(3);

    delay(100);

    turnWithoutCoA(-3);

    delay(10);

    driveWithoutAvoidance(1);

    delay(100);

    driveWithoutAvoidance(-1);

    delay(1500);
  }
  return true;
}

//////////
// LOOP //
//////////

void loop() {
  if (leftTeam()) {
    Serial.println("Left!");
    hardcodeleft();
  } else {
    Serial.println("Right!");
    hardcoderight();
  }
  while (1) {
    delay(100);
  }
//	drive(100);
//	delay(1000);
//	turn(90);
//	delay(1000);
//	turn(-90);
//	delay(1000);
//	turn(-90);
//	delay(1000);
//	turn(90);
//	delay(1000);
//	drive(-100);
//	delay(1000);
}

///////////
// SETUP //
///////////

void setup() {
  configBot();

  delay(3000);
  Serial.println("Waiting for start allowed");

  //End of Setup
  while (!startAllowed()) {
    delay(100);
  }
  Serial.println("Starting...");

  Timer3.attachInterrupt(timerInt);
}

void configBot() {
  Serial.begin(115200);
  Timer3.initialize(1000000); // 1 sec repeat rate
  Timer4.initialize(50);
  Timer4.attachInterrupt(trigger_pulse);
  initializeServos();
  initializeSteppers();
  initializeUS();
  pinMode(teamSwitch, INPUT);
  pinMode(starter, INPUT);
}

void initializeUS() {
  pinMode(frontTigUS, OUTPUT);
  pinMode(echoPinUS, INPUT);
  pinMode(45, INPUT);
  pinMode(46, INPUT);
  pinMode(commonTrigUS, OUTPUT);
  digitalWrite(frontTigUS, LOW);
  digitalWrite(commonTrigUS, LOW);
  attachInterrupt(digitalPinToInterrupt(echoPinUS), echo_interrupt, CHANGE);  // Attach interrupt for US
}

void initializeSteppers() {
  pinMode(stPinL, OUTPUT);
  pinMode(stPinR, OUTPUT);
  pinMode(stDirPinL, OUTPUT);
  pinMode(stDirPinR, OUTPUT);
  pinMode(stDirDrum, OUTPUT);
  pinMode(stDrum, OUTPUT);
  pinMode(stEnDrum, OUTPUT);
  digitalWrite(stEnDrum, HIGH);
}

void initializeServos() {
  cannonGate.attach(11);
  openCannonGate(false);

  winchServo.attach(5);
  holderServo1.attach(7);
  holderServo2.attach(6);
  dropGate.attach(9);
  openDropGate(false);
  securePiston(false);
  pullDown(false);
}

/*
   Controls the gate for the cannon
*/
void openCannonGate(bool open_) {
  if (open_) {
    cannonGate.write(75);
  } else {
    cannonGate.write(145);
  }
}

void openDropGate(bool open_) {
  if (open_) {
    dropGate.write(95);
  } else {
    dropGate.write(25);
  }
}

void pullDown(boolean upOrDown) {
  if (upOrDown) {
    winchServo.write(62);
  } else {
    winchServo.write(33);
  }
}

void securePiston(boolean closedOrOpen) {
  if (closedOrOpen) {
    holderServo1.write(20);
    holderServo2.write(160);
  } else {
    holderServo1.write(160);
    holderServo2.write(20);
  }
}

void turnDrumTo(short pos) {
  Serial.println("Now trying to get to pos: " + String(pos));
  unsigned int steps = abs(pos - currentDrumPos) * ((1036 * 2) / 8);
  bool dir = currentDrumPos - pos > 0; // if > gets < dir will be inverted
  moveDrum(steps, dir);
  currentDrumPos = pos;
}

void moveDrum(unsigned int steps, bool dir) {
  digitalWrite(stDirDrum, dir);

  delay(10);

  digitalWrite(stEnDrum, LOW);

  delay(10);

  float a = 0; //for acceleration
  for (unsigned int i = 0; i < steps; i++) {
	a = 0;
    if (i < 40 && i <= (steps / 2)) {
      a = tPerStep * 0.0025 * (40 - i);
    }

    if ((steps - i) < 40 && i > (steps / 2)) {
      a = tPerStep * 0.0025 * (40 - (steps - i));
    }
    digitalWrite(stDrum, HIGH);
    delayMicroseconds(tPerStep + a);
    digitalWrite(stDrum, LOW);
    delayMicroseconds(tPerStep + a);
  }

  delay(10);

  digitalWrite(stEnDrum, HIGH);
}

void shootBall(unsigned int ball) {
  securePiston(false);
  pullDown(true);

  unsigned int drumIndex = ball + 4;
  if (drumIndex > 7)
    drumIndex -= 8;
  Serial.print("Turning drum to index ");
  Serial.println(drumIndex);
  turnDrumTo(drumIndex);

  delay(2000);
  securePiston(true);
  Serial.println("secure");
  delay(250);

  openCannonGate(true);
  pullDown(false);

  delay(2000);
  openCannonGate(false);
  securePiston(false);
  delay(50);
  Serial.println("free");
}

/*
   Drives the given distance
   s is the distance to drive in cm (negative distance = backwards)
*/
void drive(int dist) {
  unsigned int steps = (abs(dist) / 32.3) * 1036;
  Serial.println(steps);
  bool pol = dist > 0;
  drive(steps, pol, dist);
  delay(35);
}

void driveWithoutAvoidance(int dist) {
  unsigned int steps = (abs(dist) / 32.3) * 1036;
  Serial.println(steps);
  bool pol = dist > 0;
  driveWithoutAvoidance(steps, pol);
  delay(100);
}

/*
   Turns at the given angle
   angle is the angle to turn (negative left, positive right)
*/
void turn(short angle) {
  unsigned int steps = ((abs(angle) % 360) / 360.0) * 2.1 * 1036;
  bool pol = angle > 0;
  if (!pol) steps *= 1.083;
  if (pol) steps *= 1.097;
  Serial.println(steps);
  int turnPinUS;
  	if(pol){
  		turnPinUS = 45;
  	} else {
  		turnPinUS = 46;
  	}
  	unsigned int ten = ((abs(30) % 360) / 360.0) * 2.1 * 1036;
  	while(steps > 0) {
  		if(steps > ten) {
  			while (readUS(turnPinUS) < 15) {
  				delay(250);
  			}
  			turn(ten, pol);
  			steps -= ten;
  		} else {
  			while (readUS(turnPinUS) < 15) {
  				delay(250);
  			}
  			turn(steps, pol);
  			steps = 0;
  		}
  	}
  delay(35);
}

void turn(unsigned int steps, bool left) {
  digitalWrite(stDirPinL, left ? HIGH : LOW);
  digitalWrite(stDirPinR, left ? HIGH : LOW);

  delay(10);

  const unsigned int aOverSteps = 1490;

  float a = 0; //for acceleration
  for (unsigned int i = 0; i < steps; i++) {
    a = 0;

    if (i < aOverSteps && i <= steps / 2) {
      a = (tPerStep / aOverSteps) * 2.0 * (aOverSteps - i);
    }
    if (steps - i < aOverSteps && i > steps / 2) {
      a = (tPerStep / aOverSteps) * 2.3 * (aOverSteps - (steps - i));
    }

    digitalWrite(stPinL, HIGH);
    digitalWrite(stPinR, HIGH);
    delayMicroseconds(tPerStep + a);
    digitalWrite(stPinL, LOW);
    digitalWrite(stPinR, LOW);
    delayMicroseconds(tPerStep + a);
  }
}

void turnWithoutCoA(short angle) {
	unsigned int steps = ((abs(angle) % 360) / 360.0) * 2.1 * 1036;
	bool pol = angle > 0;
	if (!pol) steps *= 1.083;
	if (pol) steps *= 1.097;
	Serial.println(steps);
	turnWithoutCoA(steps, pol);
	delay(100);
}

void turnWithoutCoA(unsigned int steps, bool left) {
	digitalWrite(stDirPinL, left ? HIGH : LOW);
	digitalWrite(stDirPinR, left ? HIGH : LOW);

	delay(10);

	const unsigned int aOverSteps = 1490;

	float a = 0; //for acceleration
	for (unsigned int i = 0; i < steps; i++) {
	  a = 0;
	  if (i < aOverSteps && i <= steps / 2) {
	    a = (tPerStep / aOverSteps) * 2.0 * (aOverSteps - i);
	  }
	  if (steps - i < aOverSteps && i > steps / 2) {
	    a = (tPerStep / aOverSteps) * 2.3 * (aOverSteps - (steps - i));
	  }

	  digitalWrite(stPinL, HIGH);
	  digitalWrite(stPinR, HIGH);
	  delayMicroseconds(tPerStep + a);
	  digitalWrite(stPinL, LOW);
	  digitalWrite(stPinR, LOW);
	  delayMicroseconds(tPerStep + a);
	}
}

void drive(unsigned int steps, bool dir, int dist) {
  float stillToDrive = float(dist);
  digitalWrite(stDirPinL, dir ? HIGH : LOW);
  digitalWrite(stDirPinR, dir ? LOW : HIGH);

  delay(10);

  float distancePerStep = 32.3 / 1036;
  float a = 0;
  for (unsigned int i = 0; i < steps; i++) {
    stillToDrive = stillToDrive - distancePerStep;
    if (i % 50 == 0) {
      while (distance < int(stillToDrive) && distance <= 13) {
        delay(250);
      }
    }
    a = 0;
    if (i < 400 && i <= steps / 2)
      a = tPerStep * 0.0025 * (400 - i);
    if (steps - i < 400 && i > steps / 2)
      a = tPerStep * 0.0025 * (400 - (steps - i));

    digitalWrite(stPinL, HIGH);
    digitalWrite(stPinR, HIGH);
    delayMicroseconds(tPerStep + a);
    digitalWrite(stPinL, LOW);
    digitalWrite(stPinR, LOW);
    delayMicroseconds(tPerStep + a);
  }
}

void driveWithoutAvoidance(unsigned int steps, bool dir) {
  digitalWrite(stDirPinL, dir ? HIGH : LOW);
  digitalWrite(stDirPinR, dir ? LOW : HIGH);

  delay(10);

  float a = 0; //for acceleration
  for (unsigned int i = 0; i < steps; i++) {
    a = 0;
    if (i < 400 && i <= steps / 2) {
      a = tPerStep * 0.0025 * (400 - i);
    }

    if (steps - i < 400 && i > steps / 2) {
      a = tPerStep * 0.0025 * (400 - (steps - i));
    }

    digitalWrite(stPinL, HIGH);
    digitalWrite(stPinR, HIGH);
    delayMicroseconds(tPerStep + a);
    digitalWrite(stPinL, LOW);
    digitalWrite(stPinR, LOW);
    delayMicroseconds(tPerStep + a);
  }
}

// > 800 right
// < 200 left
boolean leftTeam() {
  if (analogRead(teamSwitch) < 200) {
    return true;
  } else {
    return false;
  }
}

boolean startAllowed() {
  return !digitalRead(starter);
}

void stopAllTimers() {
  TCCR0B = 0;
  TCCR1B = 0;
  TCCR2B = 0;
  Serial.println("Stopped Timer0");
  Serial.println("Stopped Timer1");
  Serial.println("Stopped Timer2");
  Timer3.detachInterrupt();
}

void timerInt() {
  count++;
  Serial.println(count);

  if (count >= 100) {
    stopAllTimers();
  }
}

//////////////////////
// HARDCODED ROUTES //
//////////////////////

void hardcodeleft() {
  drive(30);
  turnWithoutCoA(85);
  drive(40);
  turn(90);
  drive(26);
  turn(90);
  turnWithoutCoA(92);
  drive(-3);
  if (!gatherBalls()) {
    drive(4);
  } else {
    drive(8);
  }
  turn(-91);
  drive(52);

  turn(-12);
  for (int i = 0; i <= 7; i++) {
    shootBall(i);
  }
}

void hardcoderight() {
  drive(30);
  turnWithoutCoA(-90);
  drive(37);
  turn(-90);
  drive(22);
  turn(90);
  turnWithoutCoA(90);
  drive(-3);
  if (!gatherBalls()) {
    drive(4);
  } else {
    drive(8);
  }
  turn(90);
  drive(45);
  turn(10);

  for (int i = 0; i <= 7; i++) {
    shootBall(i);
  }
}

void echo_interrupt()
{
  switch (digitalRead(echoPinUS))                     // Test to see if the signal is high or low
  {
    case HIGH:                                      // High so must be the start of the echo pulse
      echo_start = micros();                        // Save the start time
      break;

    case LOW:                                       // Low so must be the end of hte echo pulse
      distance = (micros() - echo_start) / 60;
      break;
  }
}
void trigger_pulse() {
  static volatile int state = 0;                 // State machine variable

  if (!(--trigger_time_count))                   // Count to 200mS
  { // Time out - Initiate trigger pulse
    trigger_time_count = TICK_COUNTS;           // Reload
    state = 1;                                  // Changing to state 1 initiates a pulse
  }

  switch (state)                                 // State machine handles delivery of trigger pulse
  {
    case 0:                                      // Normal state does nothing
      break;

    case 1:                                      // Initiate pulse
      digitalWrite(frontTigUS, HIGH);              // Set the trigger output high
      state = 2;                                // and set state to 2
      break;

    case 2:                                      // Complete the pulse
    default:
      digitalWrite(frontTigUS, LOW);               // Set the trigger output low
      state = 0;                                // and return state to normal 0
      break;
  }
}

int readUS(int pin) {
  digitalWrite(commonTrigUS, LOW);
  delayMicroseconds(2);
  digitalWrite(commonTrigUS, HIGH);
  delayMicroseconds(10);
  digitalWrite(commonTrigUS, LOW);
  long duration = pulseIn(pin, HIGH);
  int distance = duration / 2 * 0.034;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("cm");
  return distance;
}
