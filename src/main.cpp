#include <Arduino.h>
#include <SoftwareSerial.h>
#include "ph_surveyor.h"
// lcd libraries
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// temp sensor libraries
#include <OneWire.h>
#include <DallasTemperature.h>

const byte rxPin = 2;
const byte txPin = 3;

const byte trigPin = 4;
const byte echoPin = 5;


// Set up a new SoftwareSerial object
SoftwareSerial mySerial (rxPin, txPin);

String message = "";
bool messageReady = false;

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27,20,4);

const int ONE_WIRE_BUS = 6;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


int distanceFromSensor();
void lcdSetup();
void updateLCD(int distance, double temperature);

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

  lcdSetup();
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement

}

void lcdSetup() {
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
}

void loop() {
  
  // if there is a message to send
  if (messageReady == true) {
    messageReady = false;
    mySerial.write(message.c_str());
    Serial.println("sent");
  }

  // every 500ms, read the distance from the sensor
  if (millis() % 5000 == 0) {
    int distance = distanceFromSensor();
    sensors.requestTemperatures(); // Send the command to get temperatures
    double temperature = sensors.getTempFByIndex(0);
    updateLCD(distance, temperature);
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

void updateLCD(int distance, double temperature) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Distance: ");
  lcd.print(distance);
  lcd.print(" cm");
  lcd.setCursor(0,1);
  lcd.print("Temp: ");
  lcd.print(temperature);
}