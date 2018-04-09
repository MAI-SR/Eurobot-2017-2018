#include <math.h>
#include<time.h>

short angle;
String task;
float pos[] = {0, 0};
int r1;
int r2;
int r3;
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






void setup() {
  setupLowLevel();
}

void loop() {
  tmp();
  //testCalc();

}

void driveTo(double posX, double posY){
  calculatePosition();
  float distX = posX -pos[0];
  float distY = posY-pos[1];
  float distance = sqrt(((distX)*(distX)+(distY)*(distY)));
  short angleAbs;
  if (distX != 0){
  angleAbs = distY/distX;
  angleAbs = degrees(atan(angleAbs));
  if( distX > 0 && distY > 0){
    angleAbs += 0;
  }
  else if( distX < 0 && distY > 0){
    angleAbs += 90;
  }
  else if( distX < 0 && distY < 0){
    angleAbs += 180;
  }
  else if ( distX > 0 && distY < 0){
    angleAbs += 270;
  }
  }
  else{
    angleAbs = 90;
    if(distY < 0){
      angleAbs = 270;
    }
  }
  turnTo(angleAbs);
  drive(distance);
}


void turnTo(short angleNew){
  turn(angleNew-angle);
  angle=angleNew;
}


void collectBallsOwn(){
  driveTo(collectOwnX,collectOwnY);
  turnTo(90);
  drive(20);
  gatherBalls();
  score = score +10;
  task = "deliverBallsOwn1";
}

void collectBallsMixed(){
  driveTo(collectMixedX,collectMixedY);
  turnTo(90);
  drive(20);
  gatherBalls();
  score = score+10;
  task = "deliverBallsWaste";
}

void gatherBalls(){
   for (int i = 0; i <= 7; i++){
    turnDrumTo(i);
    delay(2000);  
  }
}

void deliverBallsWaste(){
  driveTo(deliverWasteX,deliverWasteY);
  turnTo(0);
  for(int i = 1;i<=7;i=i+2){
    dropBall(i);
  }
  score = score + 20;
  task = "deliverBallsOwn2";
}

void deliverBallsOwn1(){
  driveTo(launchX,launchY);
  turnTo(90);
  for(int i =0; i<=7; i++){       
     shootBall(i);
  }
  score = score + 40;
}

void deliverBallsOwn2(){
  driveTo(launchX,launchY);
  turnTo(90);
  for(int i =0; i<=7; i= i+2){       
     shootBall(i);
    
  }
  score = score + 20;
}


void getDistances(){
  for(int i = 0;i>=7; i++){
    if( millis()/10- measurements[i][0][1] < 100 && measurements[i][0][0] < 360){
        r1 = measurements[i][0][0];
    }
  }
  for(int i = 0;i>=7; i++){
    if(millis()/10-measurements[i][1][1] < 100 && measurements[i][1][0] < 360){
      r2 = measurements[i][1][0];
    }
  }
  for(int i = 0;i>=7; i++){
    if(millis()/10-measurements[i][2][1] < 100 && measurements[i][2][0] < 360){
      r3 = measurements[i][2][0];
    }
  }
}



void calculatePosition(){ // Many thanks to Thorben
    float x1;
    float y1;
    float x2;
    float y2;
    float dist1; // distance of the first calculated point to tower 3
    float dist2; // distance of the second calculated point to tower 3
    float d1; // deviation of the first calculated point to the measured distance to tower 3
    float d2; // deviation of the second calculated point to the measured distance to tower 3
    getDistances();
    while(332,63-(r1+r2)> 0)
    {
      r1=r1+1;
      r2=r2+1;
    }
    float discr = pow((0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2)),2)+345.107008*pow(r1,2)*pow(r2,2)-177.053504*pow(r1,4)-177.053504*pow(r2,4)+322.2458758*pow(r2,2)+316.9172736*pow(r1,2)+1482.191429;
    if (discr < 0)
    {
      Serial.print("Well, we're fucked"); // In this case we really are fucked
    }
    else if(discr ==0)
    {
      pos[0]=-(0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2));
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
      dist1 = sqrt(((distx1)*(distx1)+(disty1)*(dist1)));
      d1 = abs(r3-dist1);
      float distx2 = x2-1.594;
      float disty2 = y2-0.95;
      dist1 = sqrt(((distx2)*(distx2)+(disty2)*(dist2)));
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
    *pos[0] = -(0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2))+sqrt(pow((0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2)),2)+345.107008*pow(r1,2)*pow(r2,2)-177.053504*pow(r1,4)-177.053504*pow(r2,4)+322.2458758*pow(r2,2)+316.9172736*pow(r1,2)+1482.191429);
    *pos[0] = pos[0] / 88.526752;
    *pos[1] = sqrt(pow(r2,2)-pow(pos[0],2)-3.188*pos[0]-2.540836);
    *if(sqrt((1.594-pos[0])*(1.594-pos[0])+(0.95-pos[1])*(0.95-pos[1]))!=r3){
    *pos[0] = -(0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2))-sqrt(pow((0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2)),2)+345.107008*pow(r1,2)*pow(r2,2)-177.053504*pow(r1,4)-177.053504*pow(r2,4)+322.2458758*pow(r2,2)+316.9172736*pow(r1,2)+1482.191429);
    *pos[0] = pos[0] / 88.526752;
    *pos[1] = sqrt(pow(r2,2)-pow(pos[0],2)-3.188*pos[0]-2.540836);
    *=
    **/
}


void testCalc(){
  calculatePosition();
  Serial.print("X:");
  Serial.print(pos[0]);
  Serial.print("  ");
  Serial.print("Y:");
  Serial.print(pos[1]);
  drive(20);
}

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

  delay(3000);

  drumServo.attach(8);
  pinMode(A0, INPUT);
  //turnDrumTo(0);
}

/*
 * Controlls the gate for the cannon
 */
void openCannonGate(bool open_)
{
  if(open_)
  {
    cannonGate.write(75);
  }
  else
  {
    cannonGate.write(145);
  }
}

/*
 * Controlls the gate which drops the balls
 */
void openDropGate(bool open_)
{
  if(open_)
  {
    dropGate.write(95);
  }
  else
  {
    dropGate.write(25);
  }
}

void pullDown(boolean upOrDown){
  if(upOrDown){
    winchServo.write(62);
  }else{
    winchServo.write(33);
  }
}

void securePiston(boolean closedOrOpen){
  if(closedOrOpen){
    holderServo1.write(20);
    holderServo2.write(160);
  }else{
    holderServo1.write(160);
    holderServo2.write(20);
  }
}

/*
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

void shootBallNEU(unsigned short ball){
  chargeCannon();
  
  unsigned short turnTo = ball + 4;
  if(turnTo > 7)
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
*/
void shootBall(){
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
void moveServo(int should){
  if((should < 50)||(should > 980)){//if commanded position is outside safety range dont do anything
    return;
  }
  int is = analogRead(A0);
  while(is != should){
    if(is == 0){
      drumServo.writeMicroseconds(1520);
      return;//power cable not connected to trommel
    }
    if(is == 1023){
      drumServo.writeMicroseconds(1520);
      return;//power cable not connected to trommel
    }
    Serial.print("is = ");
    Serial.print(is);
    Serial.print(" should = ");
    Serial.println(should);
    int offset = (should-is);
    if(offset > 700){
      offset = 700;
    }
    if(offset < -700){
      offset = -700;
    }
    if(offset > 0){
      drumServo.writeMicroseconds(1535+offset);//make this close to 1505 if the "trommel" jitters
    } else {
      drumServo.writeMicroseconds(1465+offset);//make this close to 1505 if the "trommel" jitters
    }
    is = analogRead(A0);
  }
  Serial.println("");
  drumServo.writeMicroseconds(1500);
}

void readValues() {
  Serial.println(analogRead(A0));
  delay(10);
}

//für Lukas
/*
 * Dreht die Trommel in eine von acht vorgegebenen Positionen
 * Die Bälle sind gegen den Uhrzeigersinn nummeriert, ausgehend von dem Slot mit dem MAI-Logo
 * Position x bedeutet, dass ball x unter die Aufsammelstelle gedreht wird
 * @param pos anzufahrende Position 0-7
 */
void turnDrumTo(unsigned short pos)
{
  switch(pos)
  {
    case 0:
    moveServo(867);
    break;
    case 1:
    moveServo(805);
    break;
    case 2:
    moveServo(738);
    break;
    case 3:
    moveServo(674);
    break;
    case 4:
    moveServo(609);
    break;
    case 5:
    moveServo(538);
    break;
    case 6:
    moveServo(442);
    break;
    case 7:
    moveServo(375);
    break;
    default:
    Serial.println("ERROR: Drum can only be turned to positions zero through seven.");
    break;
    
  }
}
void tmp()
{ 
  Serial.println(analogRead(A0));
  return;
  testDrum();
  delay(5000);
  turnDrumTo(4);
  delay(5000);
}
/*
 * Lädt den gewünschten Ball in die Kanone und feuert
 * Die Bälle sind gegen den Uhrzeigersinn nummeriert, ausgehend von dem Slot mit dem MAI-Logo
 */
void shootBall(unsigned int ball)
{
  securePiston(false);
  pullDown(true);

  unsigned int sysTime = millis();
  unsigned int drumIndex = ball + 4;
  if(drumIndex > 7)
    drumIndex -= 8;
  turnDrumTo(drumIndex);
  if((millis() - sysTime) < 1400)
    delay(1400 - abs((millis() - sysTime)));
  securePiston(true);
  Serial.println("secure");
  delay(250);

  openCannonGate(true);
  pullDown(false);

  delay(2000);
  openCannonGate(false);
  securePiston(false);
  Serial.println("free");
}

/*
 * Lässt den gewünschten Ball fallen
 * Die Bälle sind gegen den Uhrzeigersinn nummeriert, ausgehend von dem Slot mit dem MAI-Logo
 */
void dropBall(unsigned int ball)
{
  unsigned int drumIndex = ball + 6;
  if(drumIndex > 7)
    drumIndex -= 8;
  turnDrumTo(drumIndex);
  openDropGate(true);
  delay(2000);
  openDropGate(false);
}

void testDrum()
{
  for(short i = 0; i < 8; i++)
  {
    turnDrumTo(i);
    delay(1000);
  }
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
 * Fährt die angegebene Strecke
 * @param s ist die zurückzulegende Strecke in cm; eine negative Strecke bedeutet, dass der Bot rückwärts fährt
 */
void drive(int dist)
{

  unsigned int steps = (abs(dist) / 32.3) * 1036;
  Serial.println(steps);
  bool pol = dist > 0;
  drive(steps, pol);
}

//für Lukas
/*
 * Dreht sich um den angegebenen Winkel
 * @param angle ist der zu rotierende Winkel in °; + ist links, - ist rechts;
 */
void turn(short angle)
{
  
  unsigned int steps = (abs(angle) / 360.0) * 2.1 * 1036;
  bool pol = angle > 0;
  if(!pol) steps*=1.03;
  if(pol) steps*=1.04;
  Serial.println(steps);
  turn(steps, pol);
  // unsigned int steps = abs(angle)/anglePerStep;

/*
  digitalWrite(stDirPinL, pol ? HIGH : LOW);
  digitalWrite(stDirPinR, pol ? HIGH : LOW);
  
  float a = 0; //für Beschleunigungsphase
  for(int i = 0; i < steps; i++)
  {
    a = 0;
    
    if(i < 800 && i <= steps/2)
      a = (tPerStep / 800) * (800 - i);
    if(steps - i < 800 && i > steps/2)
      a = (tPerStep / 800) * (800 - (steps - i));
    
    digitalWrite(stPinL, HIGH);
    digitalWrite(stPinR, HIGH);
    delayMicroseconds(tPerStep + a);
    digitalWrite(stPinL, LOW);
    digitalWrite(stPinR, LOW);
    delayMicroseconds(tPerStep + a);
  }
 */
}

/*
 * @param steps ist die Anzahl an Schritten die gemacht werden soll
 * @param polL ist die Polarisierung des linken Steppers: true ist vorwärts, false rückwärts
 * @param polR ist die Polarisierung des rechten Steppers: true ist vorwärts, false rückwärts
 */
void turn(unsigned int steps, bool left)
{
  digitalWrite(stDirPinL, left ? HIGH : LOW);
  digitalWrite(stDirPinR, left ? HIGH : LOW);

  delay(10);
  
  const unsigned int aOverSteps = 1500;
  
  float a = 0; //für Beschleunigungsphase
  for(int i = 0; i < steps; i++)
  {
    a = 0;
    if(i < aOverSteps && i <= steps/2)
      a = (tPerStep/aOverSteps) *2.0 * (aOverSteps - i);
    if(steps - i < aOverSteps && i > steps/2)
      a = (tPerStep/aOverSteps) *2.3 * (aOverSteps -(steps - i));
      
    digitalWrite(stPinL, HIGH);
    digitalWrite(stPinR, HIGH);
    delayMicroseconds(tPerStep + a);
    digitalWrite(stPinL, LOW);
    digitalWrite(stPinR, LOW);
    delayMicroseconds(tPerStep + a);
  }
}

void drive(unsigned int steps, bool dir)
{
  digitalWrite(stDirPinL, dir ? HIGH : LOW);
  digitalWrite(stDirPinR, dir ? LOW : HIGH);

  delay(10);
  
  float a = 0; //für Beschleunigungsphase
  for(int i = 0; i < steps; i++)
  {
    a = 0;
    if(i < 400 && i <= steps/2)
      a = tPerStep * 0.0025 * (400 - i);
    if(steps - i < 400 && i > steps/2)
      a = tPerStep * 0.0025 * (400 -(steps - i));
      
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
}

//für Lukas
int readUS(){
  digitalWrite(trigPinUS, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinUS, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinUS, LOW);
  long duration = pulseIn(echoPinUS, HIGH);
  int distance = duration/2*0.034;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("cm");
  return distance;
}

//----------------------------//
//        Positioning         //
//----------------------------//

#include <Stepper.h>
#include <nRF24L01.h>
#include <RF24.h>

#define echoPinPos 3
#define trigPinPos 4

/*
 * erste Dimension: 0-7 neuste bis älteste Messungen
 * zweite Dimension: ausgewählter Tower 0-2
 * dritte Dimension:
 *  0: Abstand in cm;
 *  1: Zeitstempel in Zentisekunden;
 */
volatile short activeTower;
volatile unsigned long startTime = 0;

/*
 * Letzte bekannte Position
 * erste Dimension: ausgewählter Bot
 *  0: dieser Bot
 *  1: unser zweiter Bot
 *  2: gegnerischer erster Bot
 *  3: gegnerischer zweiter Bot
 * zweite Dimension:
 *  0: X-Koordinate in cm;
 *  1: Y-Koordinate in cm;
 *  2: Zeitstempel in Millisekunden
 */
volatile short lKPos[4][3];

RF24 radio (45, 46);
const uint64_t adress = 0xF0F0F0F0F0;

void initializePosSystem()
{
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_1MBPS);//test RF24_250KBPS later maybe, but i dunno (only works with + variants), this method returns a boolean that indicates the success of the setFunction
  radio.setAutoAck(false);//cus we want >1 devices to listen to this message... think about it a bit, it will make sense
  radio.disableCRC();


  radio.maskIRQ(1, 1, 0);//only interrupt in receiving data
  attachInterrupt(digitalPinToInterrupt(2), receivedMessage, FALLING);
  radioListenMode();

  
  Serial.print("Receiver: ");
  Serial.print("Radio is ");
  Serial.print(radio.isPVariant());
  Serial.println(".");

  pinMode(trigPinPos, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(echoPinPos), measure, CHANGE);
  digitalWrite(trigPinPos, LOW);

  for(int i = 0; i < 8; i++)
  {
    for(int j = 0; i < 3; i++)
    {
      measurements[i][j][0] = 32000;
      measurements[i][j][1] = 0;
    }
  }
}

void measure(){
  if(digitalRead(echoPinPos) == HIGH){
    startTime = micros();
  } else{
    long duration = micros()-startTime; 
    int distance = duration*0.034;
    addMeasurement(activeTower, distance, millis() * 10);
    Serial.print("Distance to tower");
    Serial.print(activeTower);
    Serial.print(": ");
    Serial.print(distance);
    Serial.println(" cm.");
  }
}

void addMeasurement(unsigned short tower, unsigned int distance, unsigned int timestamp)
{
  for(int i = 7; i > 0; i--)
  {
    measurements[i][tower][0] = measurements[i - 1][tower][0];
    measurements[i][tower][1] = measurements[i - 1][tower][1];
  }
  measurements[0][tower][0] = distance;
  measurements[0][tower][1] = timestamp;
}

/*
 * sender/receiver: 0 = all, 1 = tower 1, 2 = tower 2, 3 = tower 3, 4 = bot 1, 5 = bot 2;
 * order: 'm' = measure, 'i' = incoming, 's' = send position to other bot;
 * message syntax: "receiver","order","data"
 */
void receivedMessage() {
  if(radio.available()) {
    char text[32] = "";
    
    radio.read(&text, sizeof(text));

    char receiver = text[0];
    
    if(receiver != '0' && receiver != '4') return;

    char order = text[1];
    String data = String(text).substring(2);

    if(order == 'm') 
      orderedMeasure(data);
    else
    {
      if(order == 'i')
        incomingPos(data);
      else
      {
        if(order == 's')
          orderedSendPos();
        else
        {
          Serial.print("ERROR: Invalid order '");
          Serial.print(order);
          Serial.println("'.");
        }
      }
    }
    Serial.println("Signal");
  } else{
    Serial.println("ERROR: Interrupt triggered, even thought there is no message out there.");
  }
}

//inzwischen wohl überflüssig
/*
 * Leitet die Distanzmessung zu einem bestimmten Tower ein
 * @param data erster character entspricht dem Tower 0-2, zu dem die Distanz gemessen werden soll
 */
void orderedMeasure(String data)
{
  char c = data.charAt(0);
  if(isdigit(c)){
    unsigned short a = data.charAt(0) - '0';
    if(a <= 2)
    {
      activeTower = a;
      prepUS();
      return;
    }
  }
  Serial.print("ERROR: '");
  Serial.print(c);
  Serial.print("' is not a valid tower.");
}


//inzwischen wohl überflüssig
/*
 * erster char: nummer des Bots (0-3)
 * Rest des Strings: zwei ints, getrennt durch ein ','
 */
void incomingPos(String data)
{
  char c = data.charAt(0);
  if(isdigit(c))
  {
    unsigned short bot = data.charAt(0) - '0';
    if(bot <= 3)
    {
      short separatorIndex = data.indexOf(',');
      if(separatorIndex != -1)
      {
        lKPos[bot][0] = data.substring(1, separatorIndex - 1).toInt();
        lKPos[bot][1] = data.substring(separatorIndex + 1).toInt();
        
      }
      else
      {
        Serial.println("ERROR: Submitted coordinates are invalid.");
      }
      return;
    }
  }
    Serial.print("ERROR: '");
    Serial.print(c);
    Serial.print("' is not a valid bot.");
}

//inzwischen wohl überflüssig
/*
 * Sends the bots coordinates to the other bot; First character of the data is the bot that sent it (0-3); Coordinates are separated by ','.
 */
void orderedSendPos() {
  radioWriteMode();
  String msg = String("5i0" + lKPos[0][0]) + "," + String(lKPos[0][1]);
  char text[32] = "";
  msg.toCharArray(text, sizeof(text));
  radio.write(&text, sizeof(text));
  radioListenMode();
}

void prepUS() {
  digitalWrite(trigPinPos, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinPos, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinPos, LOW);
}

void radioListenMode() {
  radio.openReadingPipe(0, adress);
  radio.startListening();
}

void radioWriteMode() {
  radio.stopListening();
  radio.openWritingPipe(adress);
}

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

void setupLowLevel() {
  Serial.begin(9600);
  initializeServos();
  initializeSteppers();
  initializeUS();
  initializePosSystem();
  
  //funktionalitaetTesten();
  
}

void funktionalitaetTesten()
{
  checkVoltage();
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





