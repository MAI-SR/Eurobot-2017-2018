#include <nRF24L01.h>
#include <RF24.h>

#define echoPin 3
#define trigPin 4

const unsigned short towerID = 1;

RF24 radio (7, 8);
const uint64_t adress = 0xF0F0F0F0F0;


void setup() {
  Serial.begin(9600);
  
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);//tmp => cus of power reasons, should be changed to high later to strengthen signal
  radio.setDataRate(RF24_1MBPS);//test RF24_250KBPS later maybe, but i dunno (only works with + variants), this method return a boolean that indicates the success of the setFunktion
  radio.setAutoAck(false);//cus we want >1 devices to listen to this message... think about it a bit, it will make sence
  radio.disableCRC();//maybe this line is not needed, but i dunno

  radioWriteMode();

  Serial.print("Transmitter: ");
  Serial.print("Radio is ");
  Serial.print(radio.isPVariant());
  Serial.println(".");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
}

void prepUS(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
}

void radioListenMode(){
  radio.openReadingPipe(0, adress);
  radio.startListening();
}

void radioWriteMode(){
  radio.stopListening();
  radio.openWritingPipe(adress);
}

void trigger()
{
  radioWriteMode();
  const char message[32] = {'0', 'm', '0' + towerID};
  radio.write(&message, sizeof(message));
  
  prepUS();
  radioListenMode();
  Serial.println("Sent out.");
}

/*
 * Triggers another tower
 * @param true: tower2, false: tower3
 */

void orderTrigger(bool toTrigger)
{
  radioWriteMode();
  const char message[32] = {toTrigger ? '0' : '2', 't', '0' + towerID};
  radio.write(&message, sizeof(message));

  radioListenMode();
  Serial.println("Sent out order.");
}

void loop() {
  trigger();
  delay(210);
  orderTrigger(true);
  delay(220);
  orderTrigger(false);
  delay(220);
}
