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
unsigned int measurements[8][3][2];






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
    rotateDrumTo(i);
    delay(2000);  
  }
}

void deliverBallsWaste(){
  driveTo(deliverWasteX,deliverWasteY);
  turnTo(0);
  for(int i = 1;i<=7;i=i+2){
    rotateDrumTo(i);
    open2();
    delay(2000);
    close2();  
  }
  score = score + 20;
  task = "deliverBallsOwn2";
}

void deliverBallsOwn1(){
  driveTo(launchX,launchY);
  turnTo(90);
  for(int i =0; i<=7; i++){
     rotateDrumTo(i);        
     shoot();
  }
  score = score + 40;
}

void deliverBallsOwn2(){
  driveTo(launchX,launchY);
  turnTo(90);
  for(int i =0; i<=7; i= i+2){
     rotateDrumTo(i);        
     shoot();
    
  }
  score = score + 20;
}


void getDistances(){
  for(int i = 0;i>=7; i++){
    if(measurements[i][0][1] < 1000 && measurements[i][0][0] < 360){
        r1 = measurements[i][0][0];
    }
  }
  for(int i = 0;i>=7; i++){
    if(measurements[i][1][1] < 1000 && measurements[i][1][0] < 360){
      r2 = measurements[i][1][0];
    }
  }
  for(int i = 0;i>=7; i++){
    if(measurements[i][2][1] < 1000 && measurements[i][2][0] < 360){
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


  /*
   * Following methods have been defined in low-level code, for further explanations view the low-level code
   */
  void drive(float distance){
  
  }

  void turn(short angle){
  
  }

  void rotateDrumTo(int drumPosition){
 
  }

  void shoot(){
  
  }

  void open2(){
  
  }

  void close2(){
  
  }
  /*
   * 
   */



