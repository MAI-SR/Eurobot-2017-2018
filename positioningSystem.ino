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

RF24 radio (0, 0);
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

void setup() {
  initializePosSystem();

}

void loop() {
  // put your main code here, to run repeatedly:

}
