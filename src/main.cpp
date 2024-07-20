#include <Arduino.h>
#include <SoftwareSerial.h>
// ph probe library
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
OneWire oneWire(ONE_WIRE_BUS); // dont forget the pullup resistor (3.3v to data via 4.7k Ohm resistor)

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Number of temperature devices found
int numberOfDevices;

// We'll use this variable to store a found device address
DeviceAddress tempDeviceAddress;


Surveyor_pH pH = Surveyor_pH(A0);   


uint8_t user_bytes_received = 0;                
const uint8_t bufferlen = 32;                   
char user_data[bufferlen];                     



int distanceFromSensor();
void lcdSetup();
void updateLCD(float ph);
void parse_cmd(char* string);
float getTempForIndex(int index);

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

  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();

  delay(200);
  Serial.println(F("Use commands \"7\", \"4\", and \"10\" to calibrate the circuit to those respective values"));
  Serial.println(F("Use command \"CAL,CLEAR\" to clear the calibration (DOESNT WORK YET)"));
  if (pH.begin()) {                                     
    Serial.println("Loaded EEPROM");
  }

}

void lcdSetup() {
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
}

double temp1;
double temp2;
double temp3;
int distance;
int ph;

// function to get the temperature for a given index
float getTempForIndex(int index) {
  sensors.getAddress(tempDeviceAddress, index);
  return sensors.getTempF(tempDeviceAddress);
}

// function to update all the temperatures
void updateAllTemps() {
  sensors.requestTemperatures();  // Send the command to get temperatures
  temp1 = getTempForIndex(0);
  temp2 = getTempForIndex(1);
  temp3 = getTempForIndex(2);
}

void loop() {

  
  // if there is a message to send
  if (messageReady == true) {
    messageReady = false;
    mySerial.write(message.c_str());
    Serial.println("sent");
  }
  /*
  if (Serial.available() > 0) {                                                      
    user_bytes_received = Serial.readBytesUntil(13, user_data, sizeof(user_data));   
  }

  if (user_bytes_received) {                                                      
    parse_cmd(user_data);                                                          
    user_bytes_received = 0;                                                        
    memset(user_data, 0, sizeof(user_data));                                         
  }*/

 /*
  if (millis() % 500 == 0) {
    distance = distanceFromSensor();
    sensors.requestTemperatures();  // Send the command to get temperatures
    temp1 = getTempForIndex(0);
    temp2 = getTempForIndex(1);
    temp3 = getTempForIndex(2);
    updateLCD(distance, temp1);
  }
*/
  // every 500ms, read the distance from the sensor
  if (millis() % 1000 == 0) {
    Serial.println("Reading pH");
    distance = distanceFromSensor();
    sensors.requestTemperatures();  // Send the command to get temperatures
    temp1 = getTempForIndex(0);
    temp2 = getTempForIndex(1);
    temp3 = getTempForIndex(2);
    float ph = pH.read_ph();
    updateLCD(ph);
    String data = "5;" + String(distance) + ";" + String(temp1) + ";" + String(temp2) + ";" + String(temp3) + ";" + String(ph);
    Serial.println(data);
    message = data;
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

void updateLCD(float ph) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("0:");
  lcd.print(temp1);
  lcd.print(" ");
  lcd.print("1:");
  lcd.print(temp2);
  lcd.setCursor(0,1);
  lcd.print("2:");
  lcd.print(temp3);
  lcd.print(" ");
  lcd.print("P:");
  lcd.print(ph);
}

void parse_cmd(char* string) {                   
  strupr(string);
  int calNum = String(string).toInt();                                
  if (calNum == 7) {       
    pH.cal_mid();                                
    Serial.println("MID CALIBRATED");
  }
  else if (calNum == 4) {            
    pH.cal_low();                                
    Serial.println("LOW CALIBRATED");
  }
  else if (calNum == 10) {      
    pH.cal_high();                               
    Serial.println("HIGH CALIBRATED");
  }
  else if (strcmp(string, "CAL,CLEAR") == 0) { 
    pH.cal_clear();                              
    Serial.println("CALIBRATION CLEARED");
  }
}