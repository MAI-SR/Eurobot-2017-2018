#include <math.h>
#include <TimerThree.h>
#include <TimerFour.h>

#define minAkku1 11.1
#define minAkku2 11.1
#define teamSwitch A5
#define starter 43 //little one is 16
#define TICK_COUNTS 4000

String name = "C3PO";
short angle;
float pos[] = {0, 0};
float estPos[] = {0, 0};
int r1; //distance to tower 1 in cm
int r2; //distance to tower 2 in cm
int r3; //distance to tower 3 in cm
float mr1; //distance to tower 1 in m
float mr2; //distance to tower 2 in m
float mr3; //distance to tower 3 in m
int estimatedr1; //estimated distance to tower 1 in cm
int estimatedr2; //estimated distance to tower 2 in cm
int estimatedr3; //estimated distance to tower 3 in cm
float startPosX;
float startPosY;
float collectOwnX;
float collectOwnY;
float collectMixedX;
float collectMixedY;
float deliverWasteX;
float deliverWasteY;
float launchX;
float launchY;
int score;
volatile unsigned int measurements[8][3][2];
int task;    // 0 means collect Balls own, 1 means deliver Balls own, 2 means collect Balls mixed, 3 means deliver Balls own
volatile long echo_start = 0;                         // Records start time of echo pulse
volatile int distance = 0;                            // Distance calculated form US
volatile int trigger_time_count = 0;

int lastDrumPosition = 0;

void loop() {
  if (leftTeam()) {
    Serial.println("Left!");
    hardcodeleft();
  } else {
    Serial.println("Right!");
    hardcoderight();
  }
  while (1)
  {
    delay(100);
  }
  /*if (!gatherBalls()) {
    Serial.println("Gather ball fail!");
    }
    delay(5000);*/
  /*for (int i = 0; i <= 7; i++) {
    shootBall(i);
    }*/

  //  drive(150);
  //  delay(3000);
  //  drive(-150);
  //  delay(3000);
  //
  //  readUS();
  //  delay(250);

  //  Serial.print("Distance: ");
  //  Serial.println(distance);

  //  Serial.println("Hi! My name is " + name);
  //  delay(1);
  //hardcode();
  //safetyCheck();  //needs to be fixed!!!
  //Serial.println(analogRead(A0));
  //tmp();
  //for(int i = 0; i < 8; i++){
  //  shootBallNEU(i);
  //}
  /*mr1 = 1.817;
    mr2 = 1.549;
    mr3 = 1.817;
    calculatePositionTobi(mr1,mr2);
    Serial.print("X:");
    Serial.print(pos[0]);
    Serial.print("  ");
    Serial.print("Y:");
    Serial.print(pos[1]);
  */
}




//---------------------//
//      HIGH-LEVEL     //
//---------------------//



//---------------------//
//      SETUP          //


//---------------------//



void setupHighLevel() {
  pos[0] = startPosX;
  pos[1] = startPosY;
  angle = 0;
  estPos[0] = pos[0];
  estPos[1] = pos[1];
  estimate();
  task = 0;
  score = 0;
}

//---------------------//
//      TACTICS        //
//---------------------//


void gameplan() {
  switch (task) {
    case 0:
      collectBallsOwnHard();
      break;
    case 1:
      deliverBallsOwnHard();
      break;
    case 2:
      collectBallsMixedHard();
      break;
    case 3:
      deliverBallsMixedHard();
      break;
    default:
      dance();
      break;
  }
}


void collectBallsOwnHard() {
  drive(-50);
  delay(500);
  turn(90);
  //gatherBalls();
  delay(500);
  turn(-90);
  delay(500);
  drive(50);
  delay(500);

}

void deliverBallsOwnHard() {
  /*
    for(int i =0; i<=7; i++){
       shootBall(i);
    }*/
}

void collectBallsMixedHard() {
  turn(90);

  delay(500);

  drive(200);

  delay(500);

  turn(90);

  delay(500);

  drive(140);

  delay(500);

  turn(180);

  //gatherBalls();
}

void deliverBallsMixedHard() {
  drive(20);

  delay(500);

  turn(90);

  delay(500);

  drive(-150);

  delay(500);

  //for(int i = 1;i<=7;i=i+2){
  //dropBall(i);
  //}

  drive(-50);

  delay(500);

  turn(-90);

  delay(500);

  drive(120);

  delay(500);

  //for(int i = 0; i<=7; i = i+2){
  //  shootBall(i);
  //}

}

void dance() {
  for (int i = 0; i <= 20; i++) {
    turn(90);
    delay(1000);
    turn(180);
    delay(1000);
    turn(90);
  }
}

void collectBallsOwn() {
  driveTo(collectOwnX, collectOwnY);
  turnTo(90);
  drive(20);
  gatherBalls();
  score = score + 10;
}

void collectBallsMixed() {
  driveTo(collectMixedX, collectMixedY);
  turnTo(90);
  drive(20);
  gatherBalls();
  score = score + 10;
}

boolean gatherBalls() {
  Serial.println("Now gathering balls!");
  for (int i = 0; i <= 7; i++) {

    drive(4);

    delay(100);

    if (!turnDrumTo(i)) {
      Serial.println("Turn drum fail");
      return false;
    }

    delay(100);

    drive(-4);

    delay(100);

    turn(3);

    delay(100);

    turn(-3);

    delay(10);

    drive(1);

    delay(100);

    drive(-1);

    delay(2000);
  }
  return true;
}

void deliverBallsWaste() {
  driveTo(deliverWasteX, deliverWasteY);
  turnTo(0);
  for (int i = 1; i <= 7; i = i + 2) {
    dropBall(i);
  }
  score = score + 20;
}

void deliverBallsOwn1() {
  driveTo(launchX, launchY);
  turnTo(90);
  for (int i = 0; i <= 7; i++) {
    shootBall(i);
  }
  score = score + 40;
}

void deliverBallsOwn2() {
  driveTo(launchX, launchY);
  turnTo(90);
  for (int i = 0; i <= 7; i = i + 2) {
    shootBall(i);
  }
  score = score + 20;
}
//----------------------//
//      DRIVING         //
//----------------------//



void driveTo(double posX, double posY) {
  //calculatePosition();
  float distX = posX - pos[0];
  float distY = posY - pos[1];
  float distance = sqrt(((distX) * (distX) + (distY) * (distY)));
  short angleAbs;
  if (distX != 0) {
    angleAbs = distY / distX;
    angleAbs = degrees(atan(angleAbs));
    if ( distX > 0 && distY > 0) {
      angleAbs += 0;
    }
    else if ( distX < 0 && distY > 0) {
      angleAbs += 90;
    }
    else if ( distX < 0 && distY < 0) {
      angleAbs += 180;
    }
    else if ( distX > 0 && distY < 0) {
      angleAbs += 270;
    }
  }
  else {
    angleAbs = 90;
    if (distY < 0) {
      angleAbs = 270;
    }
  }
  turnTo(angleAbs);
  drive(distance);
  estPos[0] = posX;
  estPos[1] = posY;
  estimate();
}


void turnTo(short angleNew) {
  turn(angleNew - angle);
  angle = angleNew;
}
//------------------------//
//        ORIENTATION     //
//------------------------//



void getDistances() {
  /*for(int i = 0;i>=7; i++){
    if( millis()/10- measurements[i][0][1] < 100 && measurements[i][0][0] < 360){
      if(abs(measurements[i][0][0]-estimatedr1)<20){
        r1 = measurements[i][0][0];
        break;
      }
      else{
        r1 = estimatedr1;
      }
    }
    }
    for(int i = 0;i>=7; i++){
    if(millis()/10-measurements[i][1][1] < 100 && measurements[i][1][0] < 360 && abs(measurements[i][1][0]-estimatedr2)>20){
      if(abs(measurements[i][0][0]-estimatedr1)<20){
        r2 = measurements[i][1][0];
        break;
      }
      else{
        r2 = estimatedr2;
      }
    }
    }
    for(int i = 0;i>=7; i++){
    if(millis()/10-measurements[i][2][1] < 100 && measurements[i][2][0] < 360 && abs(measurements[i][2][0]-estimatedr2)>20){
      if(abs(measurements[i][0][0]-estimatedr1)<20){
        r3 = measurements[i][2][0];
        break;
      }
      else{
        r3 = estimatedr3;
      }
    }
    }
    Serial.println("getDistance start");
    estimate();
    while(abs(r1-estimatedr1) > 20){
    Serial.println("r1 was not succesful");
    Serial.print("r1:");
    Serial.println(r1);
    }
    while(abs(r2-estimatedr2) > 20){
    Serial.println("r2 was not succesful");
    Serial.print("r2:");
    Serial.println(r2);
    }
    while(abs(r3-estimatedr3) > 20){
    Serial.println("r3 was not succesful");
    Serial.print("r3:");
    Serial.println(r3);
    }
    mr1 = float(r1)/100;
    mr2 = float(r2)/100;
    mr3 = float(r3)/100;
    Serial.println("getDistance end");
    }


    void calculatePosition(){ // Many thanks to Thorben, as all constants in this method have been calculated by him
    float x1;
    float y1;
    float x2;
    float y2;
    float dist1; // distance of the first calculated point to tower 3
    float dist2; // distance of the second calculated point to tower 3
    float d1; // deviation of the first calculated point to the measured distance to tower 3
    float d2; // deviation of the second calculated point to the measured distance to tower 3
    //getDistances();
    while(332,63-(r1+r2)> 0)
    {
    r1 = r1+1;
    r2 = r2+1;
    }
    float discr = pow((0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2)),2)+345.107008*pow(r1,2)*pow(r2,2)-177.053504*pow(r1,4)-177.053504*pow(r2,4)+322.2458758*pow(r2,2)+316.9172736*pow(r1,2)+1482.191429;
    if (discr < 0)
    {
    Serial.print("ERROR: Discriminant < 0 -> the math is incorrect"); // In this case we really are fucked
    }
    else if(discr ==0)
    {
    pos[0] =-(0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2));
    pos[0] = x1/88.526752;
    pos[1] = sqrt(pow(r2,2)-pow(pos[0],2)-3.188*pos[0]-2.540836);
    }
    else
    {
    x1 = -(0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2))+sqrt(discr);
    x1 = x1/88.526752;
    y1= sqrt(pow(r2,2)-pow(x1,2)-3.188*x1-2.540836);
    x2 = -(0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2))-sqrt(discr);
    x2 = x2/88.526752;
    y2 = sqrt(pow(r2,2)-pow(x2,2)-3.188*x2-2.540836);
    float distx1 = x1-1.594;
    float disty1 = y1-0.95;
    dist1 = sqrt(((distx1)*(distx1)+(disty1)*(disty1)));
    d1 = abs(r3-dist1);
    float distx2 = x2-1.594;
    float disty2 = y2-0.95;
    dist1 = sqrt(((distx2)*(distx2)+(disty2)*(disty2)));
    d2 = abs(r3-dist2);
    if (d1<d2){
      pos[0] = x1;
      pos[1] = y1;
    }else{
      pos[0] = x2;
      pos[1] = y2;
    }
    }
    /**
    pos[0] = -(0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2))+sqrt(pow((0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2)),2)+345.107008*pow(r1,2)*pow(r2,2)-177.053504*pow(r1,4)-177.053504*pow(r2,4)+322.2458758*pow(r2,2)+316.9172736*pow(r1,2)+1482.191429);
    pos[0] = pos[0] / 88.526752;
    pos[1] = sqrt(pow(r2,2)-pow(pos[0],2)-3.188*pos[0]-2.540836);
    if(sqrt((1.594-pos[0])*(1.594-pos[0])+(0.95-pos[1])*(0.95-pos[1]))!=r3){
    pos[0] = -(0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2))-sqrt(pow((0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2)),2)+345.107008*pow(r1,2)*pow(r2,2)-177.053504*pow(r1,4)-177.053504*pow(r2,4)+322.2458758*pow(r2,2)+316.9172736*pow(r1,2)+1482.191429);
    pos[0] = pos[0] / 88.526752;
    pos[1] = sqrt(pow(r2,2)-pow(pos[0],2)-3.188*pos[0]-2.540836);
    =
  **/
}


void estimate() {
  //Serial.println("estimate start");
  estimatedr1 = sqrt(pow(-1.594 - estPos[0], 2) + pow(0.95 - estPos[1], 2));
  //Serial.print("estimatedr1:");
  //Serial.println(estimatedr1);
  estimatedr2 = sqrt(pow(1.594 - estPos[0], 2) + pow(estPos[1], 2));
  //Serial.print("estimatedr2:");
  //Serial.println(estimatedr2);
  estimatedr3 = sqrt(pow(-1.594 - estPos[0], 2) + pow(-0.95 - estPos[1], 2));
  //Serial.print("estimatedr3:");
  //Serial.println(estimatedr3);
  //Serial.println("estimate end");
}
//------------------------------//
//          AKKUS               //
//------------------------------//



boolean safetyCheck() {
  float akku1ges = 0.0;
  float akku2ges = 0.0;
  for (int i = 0; i < 5; i++) {
    float akku1 = analogRead(A1) * 5.7;
    float akku2 = analogRead(A2) * 11 - akku1;
    akku1ges = akku1ges + akku1;
    akku2ges = akku2ges + akku2;
  }
  akku1ges = akku1ges / 5.0;
  akku2ges = akku2ges / 5.0;
  //akku2ges -= akku1ges;

  Serial.print("Akku1:");
  Serial.println(akku1ges);
  Serial.print("Akku2:");
  Serial.println(akku2ges);


  if (akku1ges <= minAkku1) {
    Serial.print("Akku1 to low! Akku1:");
    Serial.println(akku1ges);
    delay(1000);
    return false;
  }
  if (akku2ges <= minAkku2) {
    Serial.print("Akku2 to low! Akku2:");
    Serial.println(akku2ges);
    delay(1000);
    return false;
  }
  return true;
}
//------------------------------//
//        TESTING               //
//------------------------------//



/*void testCalc(){
  Serial.println("testCalc start");
  getDistances();
  calculatePosition();
  //calculatePositionTobi();
  Serial.print("X:");
  Serial.print(pos[0]);
  Serial.print("  ");
  Serial.print("Y:");
  Serial.print(pos[1]);
  drive(20);
  Serial.println("testCalc end");
  }
*/


//------------------------------//
//          LOW-LEVEL           //
//------------------------------//



//----------------------------//
//           Servos           //
//----------------------------//

#include <Servo.h>

Servo cannonGate;
Servo dropGate; //Drop-Gate

Servo winchServo;
Servo holderServo1;
Servo holderServo2;

Servo drumServo;

void initializeServos()
{
  dropGate.attach(9);
  cannonGate.attach(11);
  openCannonGate(false);
  openDropGate(false);

  winchServo.attach(5);
  holderServo1.attach(7);
  holderServo2.attach(6);
  securePiston(false);
  pullDown(false);

  //delay(3000);

  drumServo.attach(8);
  pinMode(A0, INPUT);
  //turnDrumTo(0);
}

/*
   Controlls the gate for the cannon
*/
void openCannonGate(bool open_)
{
  if (open_)
  {
    cannonGate.write(75);
  }
  else
  {
    cannonGate.write(145);
  }
}

/*
   Controlls the gate which drops the balls
*/
void openDropGate(bool open_)
{
  if (open_)
  {
    dropGate.write(95);
  }
  else
  {
    dropGate.write(25);
  }
}

void pullDown(boolean upOrDown) {
  if (upOrDown) {
    winchServo.write(62);
  }
  else {
    winchServo.write(33);
  }
}

void securePiston(boolean closedOrOpen) {
  if (closedOrOpen) {
    holderServo1.write(20);
    holderServo2.write(160);
  }
  else {
    holderServo1.write(160);
    holderServo2.write(20);
  }
}


void chargeCannon()
{
  securePiston(false);
  pullDown(true);
  Serial.println("down");
  delay(1400);
  securePiston(true);
  delay(250);
  Serial.println("secure");
}

void shootBallNEU(unsigned short ball) {
  chargeCannon();

  unsigned short turnTo = ball + 4;
  if (turnTo > 7)
    turnTo -= 8;
  turnDrumTo(turnTo);

  openCannonGate(true);
  pullDown(false);
  Serial.println("up");
  delay(2000);
  openCannonGate(false);
  securePiston(false);
  Serial.println("free");
}

void shootBall() {
  pullDown(false);
  securePiston(false);
  Serial.println("free");
  delay(5000);
  pullDown(true);
  Serial.println("down");
  delay(2000);
  securePiston(true);
  Serial.println("secure");
  delay(1000);
  pullDown(false);
  Serial.println("up");
  delay(2000);
  securePiston(false);
  Serial.println("free");
}

//NICHT RUMPFUSCHEN
boolean moveServo(int should) {
  //long end = micros() + 5000;
  int lastIs = 0;
  int count = 0;
  if ((should < 50) || (should > 980)) { //if commanded position is outside safety range dont do anything
    return false;
  }
  int is = analogRead(A0);
  while (is != should) {
    if (is  < 50) { //== 0){
      drumServo.writeMicroseconds(1490);
      return false;//power cable not connected to trommel or rotation is unsafe
    }
    if (is  > 980) { //== 1023){
      drumServo.writeMicroseconds(1490);
      return false;//power cable not connected to trommel or rotation is unsafe
    }
    Serial.print("is = ");
    Serial.print(is);
    Serial.print(" should = ");
    Serial.println(should);
    /*Serial.print("is = ");
      Serial.println(is);*/
    int offset = (should - is);
    if (offset > 700) {
      offset = 700;
    }
    if (offset < -700) {
      offset = -700;
    }

    if (abs(offset) < 15)
    {
      if (offset < 0)
      {
        drumServo.writeMicroseconds(1462);
      }
      else
      {
        drumServo.writeMicroseconds(1537);
      }
    }
    else {
      if (offset > 0) {
        drumServo.writeMicroseconds(1540 + offset); //make this close to 1490 if the "trommel" jitters
      } else {
        drumServo.writeMicroseconds(1460 + offset); //make this close to 1505 if the "trommel" jitters
      }
    }
    lastIs = is;
    is = analogRead(A0);

    if (lastIs == is) {
      count++;
      Serial.println(count);
    }

    if (count >= 750) {
      drumServo.writeMicroseconds(1490);
      Serial.println("We just stoped  the drum");
      return false;
    }

  }
  drumServo.writeMicroseconds(1490);
  Serial.print("is = ");
  Serial.print(is);
  Serial.print(" should = ");
  Serial.println(should);
  return true;
}

void readValues() {
  Serial.println(analogRead(A0));
  delay(10);
}

//für Lukas
/*
   Dreht die Trommel in eine von acht vorgegebenen Positionen
   Die Bälle sind gegen den Uhrzeigersinn nummeriert, ausgehend von dem Slot mit dem MAI-Logo
   Position x bedeutet, dass ball x unter die Aufsammelstelle gedreht wird
   @param pos anzufahrende Position 0-7
*/
boolean turnDrumTo(unsigned short pos)
{
  Serial.println("Now trying to get to pos: " + String(pos));
  switch (pos)
  {
    case 0:
      if (!moveServo(699)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    case 1:
      if (!moveServo(619)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    case 2:
      if (!moveServo(538)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    case 3:
      if (!moveServo(459)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    case 4:
      if (!moveServo(391)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    case 5:
      if (!moveServo(316)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    case 6:
      if (!moveServo(235)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    case 7:
      if (!moveServo(164)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    default:
      Serial.println("ERROR: Drum can only be turned to positions zero through seven.");
      break;
  }
}

boolean turnDrumToShoot(unsigned short pos)
{
  Serial.println("Now trying to get to pos: " + String(pos));
  switch (pos)
  {
    case 0:
      if (!moveServo(703)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    case 1:
      if (!moveServo(622)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    case 2:
      if (!moveServo(539)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    case 3:
      if (!moveServo(455)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    case 4:
      if (!moveServo(380)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    case 5:
      if (!moveServo(313)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    case 6:
      if (!moveServo(235)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    case 7:
      if (!moveServo(166)) {
        turnDrumTo(lastDrumPosition);
        Serial.println("Now going back to: " + String(lastDrumPosition));
        lastDrumPosition = pos;
        return false;
      }
      return true;
    default:
      Serial.println("ERROR: Drum can only be turned to positions zero through seven.");
      break;
  }
}

void shakeDrum()
{

  drumServo.writeMicroseconds(1530);
  delay(700);
  drumServo.writeMicroseconds(1470);
  delay(1400);
  drumServo.writeMicroseconds(1530);
  delay(700);

}

void tmp()
{
  //Serial.println(analogRead(A0));
  //return;

  testDrum();

  //for (int i = 0; i < 8; i++)
  //{
  //  shootBall(i);
  //}
}

/*
   Lädt den gewünschten Ball in die Kanone und feuert
   Die Bälle sind gegen den Uhrzeigersinn nummeriert, ausgehend von dem Slot mit dem MAI-Logo
*/
void shootBall(unsigned int ball)
{
  securePiston(false);
  pullDown(true);

  //unsigned int sysTime = millis();
  unsigned int drumIndex = ball + 4;
  if (drumIndex > 7)
    drumIndex -= 8;
  Serial.print("Turning drum to index ");
  Serial.println(drumIndex);
  turnDrumToShoot(drumIndex);
  /*if ((millis() - sysTime) < 1400){
    delay(1400 - abs((millis() - sysTime)));
    }*/
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
   Lässt den gewünschten Ball fallen
   Die Bälle sind gegen den Uhrzeigersinn nummeriert, ausgehend von dem Slot mit dem MAI-Logo
*/
void dropBall(unsigned int ball)
{
  unsigned int drumIndex = ball + 6;
  if (drumIndex > 7)
    drumIndex -= 8;
  turnDrumTo(drumIndex);
  openDropGate(true);
  delay(2000);
  openDropGate(false);
}

void testDrum()
{
  for (short i = 0; i < 8; i++)
  {
    openDropGate(false);
    delay(50);
    turnDrumTo(i);
    Serial.print("Drum-Positon:");
    Serial.println(i);
    //shakeDrum();
    drumServo.writeMicroseconds(1490);
    openDropGate(true);
    delay(1000);
  }
  drumServo.writeMicroseconds(1490);
}

//----------------------------//
//          Driving           //
//----------------------------//

#define stPinL 13
#define stPinR 15
#define stDirPinL 12
#define stDirPinR 14

//Minimaler delay zwischen zwei Schritten in Mikrosekunden
const float tPerStep = 420.0;

const int stepsPerRevolution = 200;
//Zurückgelegte Strecke beim Ausführen eines Schrittes in cm
const double sPerStep = 0.00785;
//Winkel, um den sich der Bot dreht, wenn die Stepper einen gegenläufiger Schritt ausführen
const float anglePerStep = 0.041043;

void initializeSteppers()
{
  pinMode(stPinL, OUTPUT);
  pinMode(stPinR, OUTPUT);
  pinMode(stDirPinL, OUTPUT);
  pinMode(stDirPinR, OUTPUT);
}

//für Lukas
/*
   Fährt die angegebene Strecke
   @param s ist die zurückzulegende Strecke in cm; eine negative Strecke bedeutet, dass der Bot rückwärts fährt
*/
void drive(int dist)
{

  unsigned int steps = (abs(dist) / 32.3) * 1036;
  Serial.println(steps);
  bool pol = dist > 0;
  drive(steps, pol, dist);
  delay(200);
}

void driveWithoutAvoidance(int dist)
{

  unsigned int steps = (abs(dist) / 32.3) * 1036;
  Serial.println(steps);
  bool pol = dist > 0;
  driveWithoutAvoidance(steps, pol);
  delay(200);
}


//für Lukas
/*
   Dreht sich um den angegebenen Winkel
   @param angle ist der zu rotierende Winkel in °; + ist links, - ist rechts;
*/
void turn(short angle)
{

  unsigned int steps = ((abs(angle) % 360) / 360.0) * 2.1 * 1036;
  bool pol = angle > 0;
  if (!pol) steps *= 1.083;
  if (pol) steps *= 1.097;
  Serial.println(steps);
  turn(steps, pol);
  delay(200);
}

/*
   @param steps ist die Anzahl an Schritten die gemacht werden soll
   @param polL ist die Polarisierung des linken Steppers: true ist vorwärts, false rückwärts
   @param polR ist die Polarisierung des rechten Steppers: true ist vorwärts, false rückwärts
*/
void turn(unsigned int steps, bool left)
{
  digitalWrite(stDirPinL, left ? HIGH : LOW);
  digitalWrite(stDirPinR, left ? HIGH : LOW);

  delay(10);

  const unsigned int aOverSteps = 1490;

  float a = 0; //für Beschleunigungsphase
  for (int i = 0; i < steps; i++)
  {
    a = 0;
    if (i < aOverSteps && i <= steps / 2)
      a = (tPerStep / aOverSteps) * 2.0 * (aOverSteps - i);
    if (steps - i < aOverSteps && i > steps / 2)
      a = (tPerStep / aOverSteps) * 2.3 * (aOverSteps - (steps - i));

    digitalWrite(stPinL, HIGH);
    digitalWrite(stPinR, HIGH);
    delayMicroseconds(tPerStep + a);
    digitalWrite(stPinL, LOW);
    digitalWrite(stPinR, LOW);
    delayMicroseconds(tPerStep + a);
  }
}

void drive(unsigned int steps, bool dir, int dist)
{
  float stillToDrive = float(dist);
  int measuredDistance = 300;
  digitalWrite(stDirPinL, dir ? HIGH : LOW);
  digitalWrite(stDirPinR, dir ? LOW : HIGH);

  delay(10);

  float distancePerStep = 32.3 / 1036;
  float a = 0; //für Beschleunigungsphase
  for (int i = 0; i < steps; i++)
  {
    stillToDrive = stillToDrive - distancePerStep;
    if (i % 100 == 0)
    {
      while (distance < int(stillToDrive) && distance <= 16)
      {
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

void driveWithoutAvoidance(unsigned int steps, bool dir)
{
  digitalWrite(stDirPinL, dir ? HIGH : LOW);
  digitalWrite(stDirPinR, dir ? LOW : HIGH);

  delay(10);

  float a = 0; //für Beschleunigungsphase
  for (int i = 0; i < steps; i++)
  {
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

//----------------------------//
//     Collision-Avoidance    //
//----------------------------//

#define echoPinUS 18
#define trigPinUS 17

void initializeUS()
{
  pinMode(trigPinUS, OUTPUT);
  pinMode(echoPinUS, INPUT);
  digitalWrite(trigPinUS, LOW);
  attachInterrupt(digitalPinToInterrupt(echoPinUS), echo_interrupt, CHANGE);  // Attach interrupt for US
}

//für Lukas
int readUS() {
  digitalWrite(trigPinUS, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinUS, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinUS, LOW);
  //long duration = pulseIn(echoPinUS, HIGH);
  //int distance = duration / 2 * 0.034;
  int distance = 15;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("cm");
  return distance;
}

//----------------------------//
//        Positioning         //
//----------------------------//



//----------------------------//
//          General           //
//----------------------------//

void checkVoltage() {
  float akku1 = analogRead(A1) * 4.9 * 5.7;
  float akku2 = analogRead(A2) * 4.9 * 11 - akku1;

  Serial.print("Akku1 Spannung:");
  Serial.print(akku1);
  Serial.println("mV");
  Serial.print("Akku2 Spannung:");
  Serial.print(akku2);
  Serial.println("mV");
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

void setupLowLevel()
{
  Serial.begin(115200);
  initializeServos();
  initializeSteppers();
  //turnDrumTo(0);
  initializeUS();
  //initializePosSystem();

  //funktionalitaetTesten();

}

void funktionalitaetTesten()
{
  //checkVoltage();
  delay(1000);

  //turnDrumTo(0);

  //testDrum();
  //openCannonGate(true);
  //shootBall();
  //openCannonGate(false);

  //openDropGate(true);
  //delay(1000);
  //openDropGate(false);

  //delay(5000);

  //drive(50);
  //turn(90);
}

void stopAllTimers() {
  TCCR0B = 0;
  TCCR1B = 0;
  TCCR2B = 0;
  Serial.println("Stopped Timer0");
  Serial.println("Stopped Timer1");
  Serial.println("Stopped Timer2");
  Timer3.detachInterrupt();
  drumServo.writeMicroseconds(1490);
}
volatile int count = 0;

void timerInt() {
  count++;
  Serial.println(count);

  if (count >= 100) {
    stopAllTimers();
  }
}


void setup() {
  //Serial.begin(9600);
  setupLowLevel();
  pinMode(teamSwitch, INPUT);
  pinMode(starter, INPUT);
  Serial.println("Waiting for start allowed");
  drumServo.writeMicroseconds(1490);
  Timer3.initialize(1000000);
  Timer4.initialize(50);
  Timer4.attachInterrupt(trigger_pulse);
  delay(3000);

  //  if (!safetyCheck()) {
  //    while (true) {
  //      delay(10000);
  //    }
  //  }

  //End of Setup
  while (!startAllowed()) {
    delay(100);
  }
  Serial.println("Starting...");
  Timer3.attachInterrupt(timerInt);

  Serial.println(analogRead(A0));
  delay(3000);
  
}

//----------------------------//
//          Hardcode          //
//----------------------------//

void hardcodeleft() {
  drive(30);
  turn(85);
  drive(40);
  turn(90);
  drive(26);
  turn(-190);
  drive(-3);
  if (!gatherBalls()) {
    drive(4);
  } else {
    drive(8);
  }
  turn(-90);
  drive(45);


  turn(-15);
  for (int i = 0; i <= 7; i++) {
    shootBall(i);
  }
  /*
    driveToBallsOwn();//Drives infront of the singlecoloured water recuperator

    collectBalls();//Turns, Collects balls, turns back

    driveToClean1();//Drives infront of the water tower

    deliverOwn();//Shoots the balls into the water tower

    driveToBallsMixed();//Drives infront of the multicoloured water recuperator

    collectBalls();//Turns, Collects balls, turns back

    driveToDirty();//Drives infront of the treatment plant

    deliverMixedDirty();//Somehow drives right next to the treatment plant and drops the dirty balls

    driveToClean2();//Drives infront of the water tower

    deliverMixedClean();//Shoots the clean water into the water tower
  */
}

void driveToBallsOwn() {
  drive(30);
  turn(90);
  drive(36);
  turn(90);
  drive(30);
  //should now be in position for collectBalls()
}

void collectBalls() {
  drive(3);
  turn(180);
  drive(-3);//maybe not neccesary if its already standing perfectly under the tower after turning
  gatherBalls();
  drive(3);
  turn(-180);
  drive(-3);
}

void driveToClean1() {
  drive(-7);
  turn(-90);
  drive(-36);
  turn(-90);
  drive(-7);
}

void deliverOwn() {
  turn(-90);
  drive(20);//depends on the power of the rubber bands
  for (int i = 0; i <= 7; i++) {
    shootBall(i);
  }
  drive(-20);
  turn(90);
}

void driveToBallsMixed() {
  drive(200);
  turn(90);
  drive(140);
  //should now be in position for collectBalls()
}

void driveToDirty() {
  turn(90);
  drive(100);
}

void deliverMixedDirty() {
  for (int i = 1; i <= 7; i  = i + 2) {
    dropBall(i);
    drive(10);  //it's driving forward to prevent the balls from getting stuck in the drop exit(should drive 40 in total)
  }
}

void driveToClean2() {
  drive(60);
  turn(90);
  drive(140);
}

void deliverMixedClean() {
  drive(20);//depends on the power of the rubber bands
  for (int i = 0; i <= 7 ; i = i + 2) {
    shootBall(i);
  }
  drive(-20);
}


void hardcoderight() {
  drive(30);
  turn(-90);
  drive(37);
  turn(-90);
  drive(22);
  turn(180);
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
  /*
    driveToBallsOwn_2();//Drives infront of the singlecoloured water recuperator

    collectBalls_2();//Turns, Collects balls, turns back

    driveToClean1_2();//Drives infront of the water tower

    deliverOwn_2();//Shoots the balls into the water tower

    driveToBallsMixed_2();//Drives infront of the multicoloured water recuperator

    collectBalls_2();//Turns, Collects balls, turns back

    driveToDirty_2();//Drives infront of the treatment plant

    deliverMixedDirty_2();//Somehow drives right next to the treatment plant and drops the dirty balls

    driveToClean2_2();//Drives infront of the water tower

    deliverMixedClean_2();//Shoots the clean water into the water tower
  */
}

void driveToBallsOwn_2() {
  drive(30);
  turn(-90);
  drive(36);
  turn(-90);
  drive(30);
  //should now be in position for collectBalls()
}

void collectBalls_2() {
  drive(3);
  turn(-180);
  drive(-3);//maybe not neccesary if its already standing perfectly under the tower after turning
  gatherBalls();
  drive(3);
  turn(180);
  drive(-3);
}

void driveToClean1_2() {
  drive(-7);
  turn(90);
  drive(-36);
  turn(-90);
  drive(-7);
}

void deliverOwn_2() {
  turn(90);
  drive(20);//depends on the power of the rubber bands
  for (int i = 0; i <= 7; i++) {
    shootBall(i);
  }
  drive(-20);
  turn(-90);
}

void driveToBallsMixed_2() {
  drive(200);
  turn(-90);
  drive(140);
  //should now be in position for collectBalls()
}

void driveToDirty_2() {
  turn(90);
  drive(-100);
}

void deliverMixedDirty_2() {
  for (int i = 1; i <= 7; i  = i + 2) {
    dropBall(i);
    drive(-10);  //it's driving forward to prevent the balls from getting stuck in the drop exit(should drive -40 in total)
  }
}

void driveToClean2_2() {
  drive(-60);
  turn(90);
  drive(140);
}

void deliverMixedClean_2() {
  drive(20);//depends on the power of the rubber bands
  for (int i = 0; i <= 7 ; i = i + 2) {
    shootBall(i);
  }
  drive(-20);
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
      state = 2;                                // and set state to 2
      break;

    case 2:                                      // Complete the pulse
    default:
      digitalWrite(trigPinUS, LOW);               // Set the trigger output low
      state = 0;                                // and return state to normal 0
      break;
  }
}

