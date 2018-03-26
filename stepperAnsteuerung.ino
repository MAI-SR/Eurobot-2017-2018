#include <Stepper.h>

#define stPinL 13
#define stPinR 15
#define stDirPinL 12
#define stDirPinR 14

//Minimaler delay zwischen zwei Schritten in Mikrosekunden
const int tPerStep = 420;

const int stepsPerRevolution = 200;
//Zurückgelegte Strecke beim Ausführen eines Schrittes in cm
const double sPerStep = 0.00785;
//Winkel, um den sich der Bot dreht, wenn die Stepper einen gegenläufiger Schritt ausführen
const float anglePerStep = 0.041043;

Stepper stepperL = Stepper(200, stPinL, stDirPinL);
Stepper stepperR = Stepper(200, stPinR, stDirPinR);

void initializeSteppers()
{
  pinMode(stPinL, OUTPUT);
  pinMode(stPinR, OUTPUT);
  pinMode(stDirPinL, OUTPUT);
  pinMode(stDirPinR, OUTPUT);
}

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
  drive(steps, pol, !pol);
}

/*
 * @param steps ist die Anzahl an Schritten die gemacht werden soll
 * @param polL ist die Polarisierung des linken Steppers: true ist vorwärts, false rückwärts
 * @param polR ist die Polarisierung des rechten Steppers: true ist vorwärts, false rückwärts
 */
void drive(unsigned int steps, bool polL, bool polR)
{
  Serial.println(steps);
  float a = 0; //für Beschleunigungsphase
  for(int i = 0; i < steps; i++)
  {
    a = 0;
    if(i < 200 && i <= steps/2)
      a = tPerStep * 0.005 * (200 - i);
    if(steps - i < 200 && i > steps/2)
      a = tPerStep * 0.005 * (200 -(steps - i));
    stepperL.step(polL ? 1 : -1);
    stepperR.step(polR ? -1 : 1);
    delayMicroseconds(tPerStep + a);
  }
}

void setup() {
  initializeSteppers();

}

void loop() {
  // put your main code here, to run repeatedly:

}
