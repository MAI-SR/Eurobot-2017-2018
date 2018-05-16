#include <math.h>
#include <Servo.h>
#include <TimerFour.h>

const float tPerStep = 540.0;
const float tPerStepTurn = 420.0;

#define stPinL 15
#define stPinR 7
#define stDirPinL 14
#define stDirPinR 6

#define TICK_COUNTS 4000

#define echoLeftUS 20
#define echoRightUS 21
#define commonTrigUS 17

#define teamSwitch A5
#define starter 16

Servo robotArm;

String name = "R2D2";

volatile long echo_start = 0;
volatile long echo_start_two = 0;
volatile int distance = 0;
volatile int distance_one = 0;
volatile int distance_two = 0;
volatile int trigger_time_count = 0;
volatile bool triggeractive = true;

//////////////////////////////
// Collision Avoidance Fkts //
//////////////////////////////

void driveWithoutAvoidance(unsigned int steps , bool dir) {
  digitalWrite(stDirPinL , dir ? HIGH : LOW);
  digitalWrite(stDirPinR , dir ? LOW : HIGH);

  delay(10);

  float a = 0;
  for (unsigned int i = 0; i < steps; i++) {
    a = 0;
    if (i < 700 && i <= steps / 2)
      a = tPerStep * 0.004 * (700 - i);
    if (steps - i < 700 && i > steps / 2)
      a = tPerStep * 0.0055 * (700 - (steps - i));

    digitalWrite(stPinR , HIGH);
    digitalWrite(stPinL , HIGH);
    delayMicroseconds(tPerStep + a);
    digitalWrite(stPinL , LOW);
    digitalWrite(stPinR , LOW);
  }
}

void driveWithoutAvoidance(int dist) {
  unsigned int steps = (abs(dist) / 32.3) * 1036;
  Serial.println(steps);
  bool pol = dist > 0;
  driveWithoutAvoidance(steps , pol);
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
	  if (i % 100 == 0) {
		  if (distance < int(stillToDrive) && distance <= 15) {
			  steps += 30;
			  for (int d = 0; d < 6; d++) {
				  i++;
				  a = tPerStep * 0.0055 * d;
				  digitalWrite(stPinR , HIGH);
				  digitalWrite(stPinL , HIGH);
				  delayMicroseconds(tPerStep + a);
				  digitalWrite(stPinL , LOW);
				  digitalWrite(stPinR , LOW);
			  }
			  while (distance < int(stillToDrive) && distance <= 15) {
				  delay(250);
			  }
			  for (int d = 0; d < 3; d++) {
				  i++;
				  a = tPerStep * 0.0055 * (2 - d);
				  digitalWrite(stPinR , HIGH);
				  digitalWrite(stPinL , HIGH);
				  delayMicroseconds(tPerStep + a);
				  digitalWrite(stPinL , LOW);
				  digitalWrite(stPinR , LOW);
			  }
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

void drive(int dist) {
  unsigned int steps = (abs(dist) / 32.3) * 1036;
  Serial.println(steps);
  bool pol = dist > 0;
  drive(steps , pol, dist);
}

void turn(short angle) {
	triggeractive = false;
	unsigned int steps = (((abs(angle) % 360) / (360.0)) * 49.25 / 32.3) * 1036;
	bool pol = angle > 0;
	if (!pol) steps *= 1.03;
	if (pol) steps *= 1.04;
	Serial.println(steps);
	int turnPinUS;
	if(pol){
		turnPinUS = 5;
	} else {
		turnPinUS = 4;
	}
	unsigned int ten = (((abs(10) % 360) / (360.0)) * 49.25 / 32.3) * 1036;
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
	triggeractive = true;
}

void turn(unsigned int steps , bool left) {
  digitalWrite(stDirPinL , left ? HIGH : LOW);
  digitalWrite(stDirPinR , left ? HIGH : LOW);

  delay(10);

  const unsigned int aOverSteps = 1500;

  float a = 0;
  for (unsigned int i = 0; i < steps; i++) {
    a = 0;

    if (i < aOverSteps && i <= steps / 2)
      a = (tPerStepTurn / aOverSteps) * 2.0 * (aOverSteps - i);
    if (steps - i < aOverSteps && i > steps / 2)
      a = (tPerStepTurn / aOverSteps) * 2.3 * (aOverSteps - (steps - i));

    digitalWrite(stPinL , HIGH);
    digitalWrite(stPinR , HIGH);
    delayMicroseconds(tPerStepTurn + a);
    digitalWrite(stPinL , LOW);
    digitalWrite(stPinR , LOW);
    delayMicroseconds(tPerStepTurn + a);
  }
}

void moveArm(String pos)
{
  if (pos == "in")
  {
    robotArm.write(30);
  }
  if (pos == "out")
  {
    robotArm.write(130);
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

void automationPannelRight()
{
  drive(87);
  turn(90);
  driveWithoutAvoidance(25);
}

void automationPannelLeft()
{
  drive(80);
  turn(-90);
  driveWithoutAvoidance(25);
}

void setupStepper() {
  pinMode(stPinL , OUTPUT);
  pinMode(stPinR , OUTPUT);
  pinMode(stDirPinL , OUTPUT);
  pinMode(stDirPinR , OUTPUT);
}

void configPins() {
  setupStepper();
  initializeUS();
  pinMode(teamSwitch, INPUT);
  pinMode(starter, INPUT);
}

void initializeUS() {
  pinMode(commonTrigUS, OUTPUT);
  pinMode(echoLeftUS, INPUT);
  pinMode(echoRightUS, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  digitalWrite(commonTrigUS, LOW);
  attachInterrupt(digitalPinToInterrupt(echoLeftUS), echo_interrupt, CHANGE);  // Attach interrupt for US
  attachInterrupt(digitalPinToInterrupt(echoRightUS), echo_interrupt_two, CHANGE);  // Attach interrupt for US
}

void setup() {
  Serial.begin(115200);
  configPins();
  Timer4.initialize(50);
  Timer4.attachInterrupt(trigger_pulse);
  moveArm("in");
  Serial.println("Waiting for start allowed");
  delay(3000);

  //End of Setup
  while (!startAllowed()) {
    delay(100);
  }
  Serial.println("Starting...");
  delay(4000);
}


void loop() {
  if (leftTeam()) {
    automationPannelLeft();
  } else {
    automationPannelRight();
  }
  while (true) {
    delay(1000);
  }
//	Serial.print("Left: ");
//	Serial.println(distance_one);
//	Serial.print("Right: ");
//	Serial.println(distance_two);
//	Serial.print("Min: ");
//	Serial.println(distance);
//	delay(200);
//	drive(200);
//	delay(1000);
//	turn(90);
//	delay(1000);
//	turn(-90);
//	delay(1000);
//	turn(-90);
//		delay(1000);
//		turn(90);
//		delay(1000);
//	drive(-200);
//	delay(1000);
}

void echo_interrupt() {
  switch (digitalRead(echoLeftUS)) {
    case HIGH:
      echo_start = micros();
      break;

    case LOW:
      distance_one = (micros() - echo_start) / 60;
      distance = min(distance_one, distance_two);
      break;
  }
}

void echo_interrupt_two() {
  switch (digitalRead(echoRightUS)) {
    case HIGH:
      echo_start_two = micros();
      break;

    case LOW:
      distance_two = (micros() - echo_start_two) / 60;
      distance = min(distance_one, distance_two);
      break;
  }
}

void trigger_pulse() {
	if(!triggeractive)
		return;

  static volatile int state = 0;

  if (!(--trigger_time_count)) {
    trigger_time_count = TICK_COUNTS;           // Reload
    state = 1;                                  // Changing to state 1 initiates a pulse
  }

  switch (state)                                 // State machine handles delivery of trigger pulse
  {
    case 0:                                      // Normal state does nothing
      break;

    case 1:                                      // Initiate pulse
      digitalWrite(commonTrigUS, HIGH);              // Set the trigger output high
      state = 2;                                // and set state to 2
      break;

    case 2:                                      // Complete the pulse
    default:
      digitalWrite(commonTrigUS, LOW);               // Set the trigger output low
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
