#include <Stepper.h>

#define stPinL 13
#define stPinR 15
#define stDirPinL 12
#define stDirPinR 14

//Minimaler delay zwischen zwei Schritten in Millisekunden
const int tPerStep = 500; //Placeholder

const int stepsPerRevolution = 200;
//Zurückgelegte Strecke beim Ausführen eines Schrittes in cm
const float sPerStep = 0.1649336;
//Winkel, um den sich der Bot dreht, wenn die Stepper einen gegenläufiger Schritt ausführen
const float anglePerStep = 0.1;

/*
 * Fährt die angegebene Strecke
 * @param s ist die zurückzulegende Strecke in cm; eine negative Strecke bedeutet, dass der Bot rückwärts fährt
 */
void drive(int dist)
{
  bool pol = dist < 0;
  unsigned int steps = abs(dist)/sPerStep;
  drive(steps, pol, pol);
}

/*
 * Dreht sich um den angegebenen Winkel
 * @param angle ist der zu rotierende Winkel in °; + ist links, - ist rechts;
 */
void turn(short angle)
{
  bool pol = angle < 0;
  unsigned int steps = abs(angle)/anglePerStep;
  Serial.println(steps);
  drive(steps, pol, !pol);
}

/*
 * @param steps ist die Anzahl an Schritten die gemacht werden soll
 * @param polL ist die Polarisierung des linken Steppers: true ist vorwärts, false rückwärts
 * @param polR ist die Polarisierung des rechten Steppers: true ist vorwärts, false rückwärts
 */
void drive(unsigned int steps, bool polL, bool polR)
{

   digitalWrite(stDirPinL, polL ? HIGH : LOW);
   digitalWrite(stDirPinR, polR ? LOW : HIGH);
  
  for(int i = 0; i < steps; i++)
  {
    Serial.print("Step");
    Serial.println(i);

    digitalWrite(stPinL, HIGH);
    digitalWrite(stPinR, HIGH);
    delayMicroseconds(30);
    digitalWrite(stPinL, LOW);
    digitalWrite(stPinR, LOW);
    Serial.print("Step");
    Serial.println(i);
    delay(tPerStep);
  }
}

void setup() {
  Serial.begin(9600);
  drive(200);

}

void loop() {
  // put your main code here, to run repeatedly:

}
