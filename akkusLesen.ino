void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  checkVoltage();
}

void checkVoltage() {
  float akku1 = analogRead(A1) * 4.9 * 5.7;
  float akku2 = analogRead(A2) * 4.9 * 11 - akku1;
  
  Serial.print("Akku1 Spannung:");
  Serial.println("mV");
  Serial.print("Akku2 Spannung:");
  Serial.print(akku2);
  Serial.println("mV");
}

//für anderen Akku: Verhältnis 100kOhm zu 10kOhm für Spannung beider Akkus, davon Spannung des ersten abziehen
