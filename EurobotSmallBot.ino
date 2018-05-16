#include <math.h>
#include <Servo.h>
#include <TimerThree.h>
#include <TimerFour.h>

const float tPerStep = 540.0;
const float tPerStepTurn = 420.0;

#define stPinL 15
#define stPinR 7
#define stDirPinL 14
#define stDirPinR 6

#define TICK_COUNTS 4000

#define teamSwitch A5
#define starter 16

#define echoPinUS 3
#define trigPinUS 4

#define echoPinUS2 20
#define trigPinUS2 21

Servo robotArm;

String name = "R2D2";
volatile long echo_start = 0;                         // Records start time of echo pulse
volatile int distance = 0;                            // Distance calculated form US
volatile int trigger_time_count = 0;

int readUS()
{
  digitalWrite(trigPinUS, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinUS, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinUS, LOW);
  long duration = pulseIn(echoPinUS, HIGH);
  int distance = duration / 2 * 0.034;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("cm");
  return distance;
}

void driveWithoutAvoidance(unsigned int steps , bool dir)
{
  digitalWrite(stDirPinL , dir ? HIGH : LOW);
  digitalWrite(stDirPinR , dir ? LOW : HIGH);

  delay(10);

  float a = 0;
  for (int i = 0; i < steps; i++)
  {
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

void driveWithoutAvoidance(int dist)
{
  unsigned int steps = (abs(dist) / 32.3) * 1036;
  Serial.println(steps);
  bool pol = dist > 0;
  driveWithoutAvoidance(steps , pol);
}

void initializeUS()
{
  pinMode(trigPinUS, OUTPUT);
  pinMode(echoPinUS, INPUT);
  digitalWrite(trigPinUS, LOW);
  attachInterrupt(digitalPinToInterrupt(echoPinUS), echo_interrupt, CHANGE);  // Attach interrupt for US
  //attachInterrupt(digitalPinToInterrupt(echoPinUS2), echo_interrupt2, CHANGE);  // Attach interrupt for US
}

void drive(unsigned int steps , bool dir , int dist)
{
  float stillToDrive = float(dist);
  int measuredDistance = 300;
  digitalWrite(stDirPinL , dir ? HIGH : LOW);
  digitalWrite(stDirPinR , dir ? LOW : HIGH);

  delay(10);

  float distancePerStep = 32.3 / 1036;
  float a = 0;
  for (int i = 0; i < steps; i++)
  {
    stillToDrive = stillToDrive - distancePerStep;
    if (i % 50 == 0)
    {
      if(distance < int(stillToDrive) && distance <= 19){
        steps+=30;
        for(int kuchen = 0; kuchen < 6; kuchen++){
          i++;
          a = tPerStep * 0.0055 * kuchen;
          digitalWrite(stPinR , HIGH);
          digitalWrite(stPinL , HIGH);
          delayMicroseconds(tPerStep + a);
          digitalWrite(stPinL , LOW);
          digitalWrite(stPinR , LOW);
        }
        while (distance < int(stillToDrive) && distance <= 19)
        {
          delay(250);
        }
        for(int kuchen = 0; kuchen < 3; kuchen++){
          i++;
          a = tPerStep * 0.0055 * (2-kuchen);
          digitalWrite(stPinR , HIGH);
          digitalWrite(stPinL , HIGH);
          delayMicroseconds(tPerStep + a);
          digitalWrite(stPinL , LOW);
          digitalWrite(stPinR , LOW);
        }
      }
    }
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

void drive(int dist)
{
  unsigned int steps = (abs(dist) / 32.3) * 1036;
  Serial.println(steps);
  bool pol = dist > 0;
  drive(steps , pol , dist);
}

void turn(short angle)
{
  unsigned int steps = (((abs(angle) % 360) / (360.0)) * 49.25 / 32.3) * 1036;
  bool pol = angle > 0;
  if (!pol) steps *= 1.03;
  if (pol) steps *= 1.04;
  Serial.println(steps);
  turn(steps , pol);
}

void turn(unsigned int steps , bool left)
{
  digitalWrite(stDirPinL , left ? HIGH : LOW);
  digitalWrite(stDirPinR , left ? HIGH : LOW);

  delay(10);

  const unsigned int aOverSteps = 1500;

  float a = 0;
  for (int i = 0; i < steps; i++)
  {
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

void smallBotRight() //topview right (orange start)
{
  delay(2000);
  moveArm("out");
  delay(1000);
  turn(-8);
  drive(160);
  driveWithoutAvoidance(20);
  delay(1000);
  moveArm("in");
  delay(1000);
  driveWithoutAvoidance(-16);
  delay(1000);
  moveArm("out");
  delay(1000);
  driveWithoutAvoidance(16);
  delay(1000);
  moveArm("in");
  delay(1000);
  driveWithoutAvoidance(-10);
  delay(500);
  turn(185);
  drive(80);
  turn(-96);   //counterclockwise
  drive(60);
  turn(82);    //clockwise
  drive(115);
  driveWithoutAvoidance(20);
}

void smallBotRightFake() //topview right (orange start) without turn included
{
  delay(2000);
  moveArm("out");
  delay(1000);
  drive(160);
  driveWithoutAvoidance(20);
  delay(1000);
  moveArm("in");
  delay(1000);
  driveWithoutAvoidance(-16);
  delay(1000);
  moveArm("out");
  delay(1000);
  driveWithoutAvoidance(16);
  delay(1000);
  moveArm("in");
  delay(1000);
  driveWithoutAvoidance(-10);
  delay(500);
  turn(180);
  drive(80);
  turn(-90);   //counterclockwise
  drive(60);
  turn(90);    //clockwise
  drive(115);
  driveWithoutAvoidance(20);
}

void smallBotRightBlocks()
{
  drive(32);
  delay(500);
  turn(-94);
  drive(150);
  turn(-92);
  drive(32);
  turn(90);
  delay(500);
  moveArm("out");
  delay(500);
  drive(15);
  moveArm("in");
  delay(1000);
  drive(-15);
  moveArm("out");
  delay(500);
  drive(20);
  moveArm("in");
  delay(1000);
  drive(-20);
  turn(178);
  drive(40);
  turn(-92);
  drive(85);
  turn(82);
  drive(130);
  driveWithoutAvoidance(20);
}

void smallBotRightBlocksFake() //without correction
{
  drive(32);
  delay(500);
  turn(-90);
  drive(150);
  turn(-90);
  drive(32);
  turn(90);
  delay(500);
  moveArm("out");
  delay(500);
  drive(15);
  moveArm("in");
  delay(1000);
  drive(-15);
  moveArm("out");
  delay(500);
  drive(20);
  moveArm("in");
  delay(1000);
  drive(-20);
  turn(180);
  drive(40);
  turn(-90);
  drive(85);
  turn(90);
  drive(130);
  driveWithoutAvoidance(20);
}

void automationPannelRight()
{
  drive(87);
  turn(90);
  driveWithoutAvoidance(25);
}

void smallBotRightBeeFake() //ignore for now
{
  drive(40);
  turn(-90);
  drive(150);
  turn(-90);
  driveWithoutAvoidance(50);
  turn(90);
  delay(500);
  moveArm("out");
  delay(500);
  driveWithoutAvoidance(10);
  delay(500);
  moveArm("in");
  delay(500);
  driveWithoutAvoidance(-10);
  delay(500);
  moveArm("out");
  delay(500);
  driveWithoutAvoidance(15);
  delay(500);
  moveArm("in");
  delay(500);
  driveWithoutAvoidance(-15);
  delay(500);
  moveArm("out");
  delay(500);
  drive(20);
  delay(500);
  moveArm("in");
  delay(500);
  moveArm("out");
  delay(500);
  moveArm("in");
  delay(500);  
}

void smallBotRightBee() //ignore for now
{
  turn(-2);
  drive(40);
  turn(-96);
  drive(150);
  turn(-94);
  driveWithoutAvoidance(50);
  turn(89);
  delay(500);
  moveArm("out");
  delay(500);
  driveWithoutAvoidance(10);
  delay(500);
  moveArm("in");
  delay(500);
  driveWithoutAvoidance(-10);
  delay(500);
  moveArm("out");
  delay(500);
  turn(-2);
  driveWithoutAvoidance(15);
  delay(500);
  moveArm("in");
  delay(500);
  driveWithoutAvoidance(-15);
  delay(500);
  moveArm("out");
  delay(500);
  turn(-2);
  drive(20);
  delay(500);
  moveArm("in");
  delay(500);
  moveArm("out");
  delay(500);
  moveArm("in");
  delay(500);  
}
void smallBotLeft() //topview left (green start)
{
  delay(2000);
  turn(-8);
  drive(160);
  driveWithoutAvoidance(20);
  delay(1000);
  moveArm("out");
  delay(1000);
  moveArm("in");
  driveWithoutAvoidance(5);
  delay(1000);
  moveArm("out");
  delay(1000);
  moveArm("in");
  delay(1000);
  driveWithoutAvoidance(-5);
  turn(180);
  drive(50);
  turn(90);    //clockwise
  drive(80);
  turn(-90);   //counterclockwise
  drive(110);
  driveWithoutAvoidance(20);
}

void smallBotLeftFake() //topview left (green start) without turn included
{
  delay(2000);
  drive(160);
  driveWithoutAvoidance(20);
  delay(1000);
  moveArm("out");
  delay(1000);
  moveArm("in");
  driveWithoutAvoidance(5);
  delay(1000);
  moveArm("out");
  delay(1000);
  moveArm("in");
  delay(1000);
  driveWithoutAvoidance(-5);
  turn(185);
  drive(50);
  turn(84);    //clockwise
  drive(80);
  turn(-96);   //counterclockwise
  drive(110);
  driveWithoutAvoidance(20);
}

void smallBotLeftBlocks()
{
  drive(32);
  turn(87);
  drive(145);
  turn(88);
  drive(43);
  turn(-91);
  drive(20);
  moveArm("out");
  delay(500);
  drive(-10);
  delay(500);
  moveArm("in");
  drive(15);
  moveArm("out");
  delay(500);
  drive(-15);
  delay(500);
  moveArm("in");
  turn(178);
  drive(40);
  turn(88);
  drive(80);
  turn(-92);
  drive(130);
  driveWithoutAvoidance(10); 
}

void automationPannelLeft()
{
  drive(80);
  turn(-90);
  driveWithoutAvoidance(25);
}
void smallBotLeftBlocksFake() //without correction
{
  drive(40);
  turn(90);
  drive(150);
  turn(90);
  driveWithoutAvoidance(40);
  turn(-90);
  driveWithoutAvoidance(15);
  moveArm("out");
  delay(500);
  driveWithoutAvoidance(-10);
  delay(500);
  driveWithoutAvoidance(15);
  moveArm("out");
  delay(500);
  driveWithoutAvoidance(-15);
  delay(500);
  moveArm("in");
  turn(180);
  drive(60);
  turn(90);
  drive(60);
  turn(-90);
  drive(90);
  driveWithoutAvoidance(20); 
}
void smallBotLeftBeeFake() //ignore for now
{
  drive(40);
  turn(90);
  drive(150);
  turn(90);
  driveWithoutAvoidance(40);
  turn(-90);
  driveWithoutAvoidance(10);
  moveArm("out");
  delay(500);
  driveWithoutAvoidance(-10);
  delay(500);
  moveArm("in");
  driveWithoutAvoidance(15);
  delay(500);
  moveArm("out");
  delay(500);
  driveWithoutAvoidance(-15);
  delay(500);
  moveArm("in");
  delay(500);
  driveWithoutAvoidance(20);
  delay(500);
  moveArm("out");
  delay(500);
  moveArm("in");
  delay(500);
  moveArm("out");
  delay(500);
  moveArm("in");
}

void smallBotLeftBee() //ignore for now
{
  turn(-2);
  drive(40);
  turn(88);
  drive(150);
  turn(88);
  driveWithoutAvoidance(65);
  turn(-91);
  driveWithoutAvoidance(10);
  moveArm("out");
  delay(500);
  driveWithoutAvoidance(-10);
  delay(500);
  moveArm("in");
  turn(-2);
  driveWithoutAvoidance(15);
  delay(500);
  moveArm("out");
  delay(500);
  driveWithoutAvoidance(-15);
  delay(500);
  moveArm("in");
  delay(500);
  turn(-2);
  driveWithoutAvoidance(20);
  delay(500);
  moveArm("out");
  delay(500);
  moveArm("in");
  delay(500);
  moveArm("out");
  delay(500);
  moveArm("in");
}
void testing()
{
  for (int i = 0; i < 7; i++)
  {
    drive(-200);

    delay(2000);

    drive(200);

    delay(2000);
  }
}

void testing2()
{
  for (int i = 0; i < 7; i++)
  {
    digitalWrite(stPinR, HIGH);

    delay(2000);

    digitalWrite(stPinL, LOW);

    delay(2000);
  }
}

void overchargeLiPo()
{
  Serial.println("Self destruct initiated:");
  delay(1000);
  Serial.println(5);
  delay(1000);
  Serial.println(4);
  delay(1000);
  Serial.println(3);
  delay(1000);
  Serial.println(2);
  delay(1000);
  Serial.println(1);
  delay(1000);
  Serial.println("FIRE!");
  delay(1000);
  Serial.println("FIRE!");
  delay(1000);
  Serial.println("FIRE EXCLAMATION MARK");
  delay(1000);
  Serial.println("SEND HELP!");
}

void setup()
{
  Serial.begin(9600);
  pinMode(stPinL , OUTPUT);
  pinMode(stPinR , OUTPUT);
  pinMode(stDirPinL , OUTPUT);
  pinMode(stDirPinR , OUTPUT);
  robotArm.attach(5);
  moveArm("in");
  pinMode(teamSwitch, INPUT);
  pinMode(starter, INPUT);
  initializeUS();
  Serial.println("Waiting for start allowed");
  Timer4.initialize(50);
  Timer4.attachInterrupt(trigger_pulse);
  delay(3000);

  //End of Setup
  while (!startAllowed()) {
    delay(100);
  }
  Serial.println("Starting...");
}


void loop()
{
  if (leftTeam())
  {
    automationPannelLeft();
  }
  else
  {
    automationPannelRight();
  }
  while(true) 
  {
    delay(1000);
  }
}

void echo_interrupt()
{
  switch (digitalRead(echoPinUS))                   // Test to see if the signal is high or low
  {
    case HIGH:                                      // High so must be the start of the echo pulse
      echo_start = micros();                        // Save the start time
      break;

    case LOW:                                       // Low so must be the end of hte echo pulse
        distance = (micros() - echo_start) / 60;
      break;
  }
}

void echo_interrupt2()
{
  switch (digitalRead(echoPinUS2))                   // Test to see if the signal is high or low
  {
    case HIGH:                                      // High so must be the start of the echo pulse
      echo_start = micros();                        // Save the start time
      break;

    case LOW:                                       // Low so must be the end of hte echo pulse
      if(distance > (micros() - echo_start) / 60){
        distance = (micros() - echo_start) / 60;
      }
      break;
  }
}

void trigger_pulse()
{
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
      digitalWrite(trigPinUS, HIGH);              // Set the trigger output high
      digitalWrite(trigPinUS2, HIGH);              // Set the trigger output high
      state = 2;                                // and set state to 2
      break;

    case 2:                                      // Complete the pulse
    default:
      digitalWrite(trigPinUS, LOW);               // Set the trigger output low
      digitalWrite(trigPinUS2, LOW);               // Set the trigger output low
      state = 0;                                // and return state to normal 0
      break;
  }
}


/*for(int i = 0; i < 1; i++)
  {
    Serial.println(isStart());
    delay(100);
    smallBotLeft();
  }

  while(1); */
