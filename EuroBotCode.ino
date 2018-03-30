
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

//für Lukas
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


//für Lukas
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

//für Lukas
/*
 * Fires the cannon
 */

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
    turnTo -= 7;
  turnDrumTo(turnTo);
  
  openCannonGate(true);
  pullDown(false);
  Serial.println("up");
  delay(2000);
  openCannonGate(false);
  securePiston(false);
  Serial.println("free");
}
 
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

void moveServo(int should){
  if((should < 50)||(should > 980)){//if commandet position is outside safety range dont do anything
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
      drumServo.writeMicroseconds(1540+offset);//make this close to 1520 if the "trommel" jitters
    } else {
      drumServo.writeMicroseconds(1470+offset);//make this close to 1520 if the "trommel" jitters
    }
    is = analogRead(A0);
  }
  Serial.println("");
  drumServo.writeMicroseconds(1520);
}

void readValues() {
  Serial.println(analogRead(A0));
  delay(10);
}

void doServoSpeed(){
  drumServo.write(800+analogRead(A0));
  Serial.println(800+analogRead(A0));
  delay(50);
}

//für Lukas
/*
 * Dreht die Trommel in eine von acht vorgegebenen Positionen
 * @param pos anzufahrende Position 0-7
 */
void turnDrumTo(unsigned short pos)
{
  switch(pos)
  {
    case 0:
    moveServo(420);
    break;
    case 1:
    moveServo(492);
    break;
    case 2:
    moveServo(580);
    break;
    case 3:
    moveServo(660);
    break;
    case 4:
    moveServo(718);
    break;
    case 5:
    moveServo(788);
    break;
    case 6:
    moveServo(857);
    break;
    case 7:
    moveServo(922);
    break;
    default:
    Serial.println("ERROR: Drum can only be turned to positions zero through seven.");
    break;
    
  }
}

void tmp()
{
  turnDrumTo(0);
  delay(3000);
  turnDrumTo(3);
  delay(3000);
  
  //testDrum();
}

void testDrum()
{
  for(short i = 0; i < 7; i++)
  {
    turnDrumTo(i);
    delay(2000);
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
 * erste Dimension: ausgewählter Tower
 * zweite Dimension:
 *  0: Abstand in cm;
 *  1: Zeitstempel in Zentisekunden;
 */
volatile unsigned short measurements[3][2] = {{0,0}, {0,0}, {0,0}};
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
 *  2: Zeitstempel in Zentisekunden
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
}

void measure(){
  if(digitalRead(echoPinPos) == HIGH){
    startTime = micros();
  } else{
    long duration = micros()-startTime; 
    int distance = duration*0.034;
    measurements[activeTower][0] = distance;
    measurements[activeTower][1] = millis() * 10;
    Serial.print("Distance to tower");
    Serial.print(activeTower);
    Serial.print(": ");
    Serial.print(distance);
    Serial.println(" cm.");
  }
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

//Sends the bots coordinates to the other bot; First character of the data is the bot that sent it (0-3); Coordinates are separated by ','.
void orderedSendPos() {
  radioWriteMode();
  String msg = String("5i0" + lKPos[0][0]) + "," + String(lKPos[0][1]);
  char text[32] = "";
  msg.toCharArray(text, sizeof(text));
  radio.write(&text, sizeof(text));
  radioListenMode();
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

void setup() {
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

void loop() 
{
  drive(100);
  delay(1500);
  turn(90);
  delay(1500);
}


