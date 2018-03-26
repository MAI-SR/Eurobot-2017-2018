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

}

void loop() {
  // put your main code here, to run repeatedly:

}
