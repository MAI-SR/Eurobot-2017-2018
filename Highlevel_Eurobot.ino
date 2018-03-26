#include <math.h>

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






void setup() {

}

void loop() {
 
}

void driveTo(double posX, double posY){
  calculatePosition();
  float distX = posX -pos[0];
  float distY = posY-pos[1];
  float distance = sqrt(((distX)*(distX)+(distY)*(distY)));
  short angleAbs;
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
  turnTo(angleAbs);
  //drive(distance);          Method has to be done yet 
}


void turnTo(short angleNew){
  //turn(angleNew-angle);     Method has to be done yet    
  angle=angleNew;
}


void collectBallsOwn(){
  driveTo(collectOwnX,collectOwnY);
  turnTo(90);
  //drive(20);
  gatherBalls();
  score = score +10;
  task = "deliverBallsOwn1";
}

void collectBallsMixed(){
  driveTo(collectMixedX,collectMixedY);
  turnTo(90);
  //drive(20);
  gatherBalls();
  score = score+10;
  task = "deliverBallsWaste";
}

void gatherBalls(){
   for (int i = 0; i <= 7; i++){
    //rotateDrumTo(i);
    delay(2000);  
  }
}

void deliverBallsWaste(){
  driveTo(deliverWasteX,deliverWasteY);
  turnTo(0);
  for(int i = 1;i<=7;i=i+2){
    /*
    * rotateDrumTo(i);         Methods have to be done yet
    *open2();
    *delay(2000);
    *close2();
    */  
  }
  score = score + 20;
  task = "deliverBallsOwn2";
}

void deliverBallsOwn1(){
  driveTo(launchX,launchY);
  turnTo(90);
  for(int i =0; i<=7; i++){
    /*
     * rotateDrumTo(i);        Methods have to be done yet
     * shoot()
     */
  }
  score = score + 40;
}

void deliverBallsOwn2(){
  driveTo(launchX,launchY);
  turnTo(90);
  for(int i =0; i<=7; i= i+2){
    /*
     * rotateDrumTo(i);        Methods have to be done yet
     * shoot()
     */
  }
  score = score + 20;
}



void calculatePosition(){ // Many thanks to Thorben
  pos[0] = -(0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2))+sqrt(pow((0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2)),2)+345.107008*pow(r1,2)*pow(r2,2)-177.053504*pow(r1,4)-177.053504*pow(r2,4)+322.2458758*pow(r2,2)+316.9172736*pow(r1,2)+1482.191429);
  pos[0] = pos[0] / 88.526752;
  pos[1] = sqrt(pow(r2,2)-pow(pos[0],2)-3.188*pos[0]-2.540836);
  if(sqrt((1.594-pos[0])*(1.594-pos[0])+(0.95-pos[1])*(0.95-pos[1]))!=3){
  pos[0] = -(0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2))-sqrt(pow((0.09594605+12.752*pow(r1,2)-12.752*pow(r2,2)),2)+345.107008*pow(r1,2)*pow(r2,2)-177.053504*pow(r1,4)-177.053504*pow(r2,4)+322.2458758*pow(r2,2)+316.9172736*pow(r1,2)+1482.191429);
  pos[0] = pos[0] / 88.526752;
  pos[1] = sqrt(pow(r2,2)-pow(pos[0],2)-3.188*pos[0]-2.540836);
  }
}

