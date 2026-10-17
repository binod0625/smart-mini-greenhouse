#include <SoftwareSerial.h>

// Arduino 1 -> Arduino 2
// D2 = TX
SoftwareSerial displaySerial(3, 2);

// =========================
// SENSOR PINS
// =========================
const int moisture1Pin = A0;
const int moisture2Pin = A1;
const int tempPin      = A2;
const int rainPin      = A3;
const int lightPin     = A4;

// =========================
// ULTRASONIC
// =========================
const int trigPin = 4;
const int echoPin = 5;

// =========================
// OUTPUT PINS
// =========================
const int greenLED = 6;
const int redLED   = 7;

const int pump1Relay = 8;
const int pump2Relay = 9;
const int fanRelay   = 10;

const int growLightRelay = 12;
const int mainMotorRelay = 13;

// =========================
// THRESHOLDS
// =========================
const int moistureLimit = 400;
const int tempLimit = 30;
const int rainLimit = 600;
const int lightLimit = 400;

// Tank levels
const int tankFullDistance = 5;
const int tankLowDistance  = 18;

bool fillingTank = false;

// =========================
// SETUP
// =========================
void setup() {

  Serial.begin(9600);
  displaySerial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  pinMode(pump1Relay, OUTPUT);
  pinMode(pump2Relay, OUTPUT);
  pinMode(fanRelay, OUTPUT);

  pinMode(growLightRelay, OUTPUT);
  pinMode(mainMotorRelay, OUTPUT);

  // Start everything OFF
  digitalWrite(pump1Relay, LOW);
  digitalWrite(pump2Relay, LOW);
  digitalWrite(fanRelay, LOW);
  digitalWrite(growLightRelay, LOW);
  digitalWrite(mainMotorRelay, LOW);

  Serial.println("MAIN GREENHOUSE SYSTEM READY");
}

// =========================
// ULTRASONIC FUNCTION
// =========================
long getDistance() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);

  long distance = duration * 0.034 / 2;

  return distance;
}

// =========================
// TMP36 TEMPERATURE
// =========================
float getTemperature() {

  int rawValue = analogRead(tempPin);

  float voltage = rawValue * (5.0 / 1023.0);

  float temperature = (voltage - 0.5) * 100.0;

  return temperature;
}

// =========================
// MAIN LOOP
// =========================
void loop() {

  // -------------------------
  // READ SENSORS
  // -------------------------
  int moisture1 = analogRead(moisture1Pin);
  int moisture2 = analogRead(moisture2Pin);

  float temperature = getTemperature();

  int rainValue = analogRead(rainPin);

  int lightValue = analogRead(lightPin);

  long distance = getDistance();


  // =========================
  // MOISTURE / PUMP 1
  // =========================

  bool pump1 = false;
  bool pump2 = false;

  if (moisture1 < moistureLimit) {
    digitalWrite(pump1Relay, HIGH);
    pump1 = true;
  } 
  else {
    digitalWrite(pump1Relay, LOW);
    pump1 = false;
  }


  // =========================
  // MOISTURE / PUMP 2
  // =========================

  if (moisture2 < moistureLimit) {
    digitalWrite(pump2Relay, HIGH);
    pump2 = true;
  } 
  else {
    digitalWrite(pump2Relay, LOW);
    pump2 = false;
  }


  // =========================
  // TEMPERATURE / FAN
  // =========================

  bool fan = false;

  if (temperature > tempLimit) {

    digitalWrite(fanRelay, HIGH);
    fan = true;

  } 
  else {

    digitalWrite(fanRelay, LOW);
    fan = false;
  }


  // =========================
  // LIGHT
  // =========================

  bool lowLight = false;

  if (lightValue < lightLimit) {

    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);

    digitalWrite(growLightRelay, HIGH);

    lowLight = true;

  } 
  else {

    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);

    digitalWrite(growLightRelay, LOW);

    lowLight = false;
  }


  // =========================
  // WATER TANK
  // =========================

  if (distance >= tankLowDistance) {

    fillingTank = true;

  }

  if (distance <= tankFullDistance) {

    fillingTank = false;
  }


  if (fillingTank) {

    digitalWrite(mainMotorRelay, HIGH);

  } 
  else {

    digitalWrite(mainMotorRelay, LOW);
  }


  // =========================
  // SEND DATA TO ARDUINO 2
  // =========================
  //
  // m1,m2,temp,rain,lowLight,
  // distance,fillingTank,pump1,
  // pump2,fan
  //
  // Arduino 2 will also use
  // rain value for roof control.
  // =========================

  displaySerial.print(moisture1);
  displaySerial.print(",");

  displaySerial.print(moisture2);
  displaySerial.print(",");

  displaySerial.print(temperature);
  displaySerial.print(",");

  displaySerial.print(rainValue);
  displaySerial.print(",");

  displaySerial.print(lowLight);
  displaySerial.print(",");

  displaySerial.print(distance);
  displaySerial.print(",");

  displaySerial.print(fillingTank);
  displaySerial.print(",");

  displaySerial.print(pump1);
  displaySerial.print(",");

  displaySerial.print(pump2);
  displaySerial.print(",");

  displaySerial.println(fan);


  // =========================
  // SERIAL MONITOR
  // =========================

  Serial.print("M1:");
  Serial.print(moisture1);

  Serial.print(" | M2:");
  Serial.print(moisture2);

  Serial.print(" | Temp:");
  Serial.print(temperature);

  Serial.print(" | Rain:");
  Serial.print(rainValue);

  Serial.print(" | Light:");
  Serial.print(lightValue);

  Serial.print(" | Tank:");
  Serial.print(distance);

  Serial.println(" cm");


  delay(500);
}
