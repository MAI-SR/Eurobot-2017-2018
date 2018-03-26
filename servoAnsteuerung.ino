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

void shootBallNEU(){
  chargeCannon();
  

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
      return;//power cable not connected to trommel
    }
    if(is == 1023){
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
      drumServo.writeMicroseconds(1530+offset);//make this close to 1520 if the "trommel" jitters
    } else {
      drumServo.writeMicroseconds(1485+offset);//make this close to 1520 if the "trommel" jitters
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

void testDrum()
{
  for(short i = 0; i < 7; i++)
  {
    turnDrumTo(i);
    delay(2000);
  }
}

void setup()
{
  initializeServos();
}

void loop()
{
  
}

