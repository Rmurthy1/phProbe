
/*const int LOADCELL_DOUT_PIN = 14; //D5;
const int LOADCELL_SCK_PIN = 12; //D6;
const int tareButtonPin = 13; // TARE BUTTON  // D7 is GPIO13;

// required to prevent too frequent reading (maybe)
int period = 20; 
unsigned long time_now = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(tareButtonPin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  long scaleValue;
   if (millis() > time_now + period) {
    time_now = millis();
    
   }
   int tareButtonState = digitalRead(tareButtonPin);
  if (tareButtonState == HIGH) { // refactor to use interrupts
   
    Serial.print("tare pressed. offset: ");
    
  }
}*/
#include <Arduino.h>
#include <SoftwareSerial.h>

const byte rxPin = 2;
const byte txPin = 3;

const byte trigPin = 4;
const byte echoPin = 5;


// Set up a new SoftwareSerial object
SoftwareSerial mySerial (rxPin, txPin);

String message = "";
bool messageReady = false;

int distanceFromSensor();


void setup() {
  Serial.begin(9600);

// pins for software serial, may get rid of when i use hardware serial
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  // pins for the HC-SR04
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
    
  // Set the baud rate for the SoftwareSerial object
  mySerial.begin(9600);
}

void loop() {
  // if there is data available from the software serial port
  /*while (Serial.available()) { 
    message = Serial.readString();
    messageReady = true;
    Serial.println("wrote");
  }*/
  
  // if there is a message to send
  if (messageReady == true) {
    messageReady = false;
    mySerial.write(message.c_str());
    Serial.println("sent");
  }

  // every 500ms, read the distance from the sensor
  if (millis() % 5000 == 0) {
    int distance = distanceFromSensor();
    Serial.println(distance);
    message = String(distance);
    messageReady = true;
  }
}

// function to read the distance from the ultrasonic sensor
int distanceFromSensor() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  long distance = (duration/2) / 29.1;
  return distance;
}
