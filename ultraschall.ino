
#define echoPinUS 18
#define trigPinUS 17

void initializeUS()
{
  pinMode(trigPinUS, OUTPUT);
  pinMode(echoPinUS, INPUT);
  digitalWrite(trigPinUS, LOW);
}

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

void setup() {
  initiliazeUS();

}

void loop() {
  // put your main code here, to run repeatedly:

}
