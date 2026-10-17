#include <Servo.h>
#include <SoftwareSerial.h>

// ================= COMMUNICATION =================
SoftwareSerial displaySerial(3, 2);
// D3 = RX
// D2 = TX → Arduino 2 D0


// ================= SENSORS =================
const int moisture1 = A0;
const int moisture2 = A1;
const int temperatureSensor = A2;
const int rainSensor = A3;
const int lightSensor = A4;


// ================= OUTPUTS =================
const int pump1 = 8;
const int pump2 = 9;
const int fan = 10;
const int growLight = 12;

const int greenLED = 6;
const int redLED = 7;

const int mainMotor = 13;


// ================= ULTRASONIC =================
const int trigPin = 4;
const int echoPin = 5;


// ================= SERVO =================
Servo roofServo;

const int roofPin = 11;

const int roofOpen = 0;
const int roofClosed = 90;


// ================= LIMITS =================
const int moistureLimit = 400;
const int rainLimit = 400;
const int lightLimit = 400;

const float temperatureLimit = 30.0;

const float waterLowLimit = 18.0;
const float waterFullLimit = 5.0;


// Tank filling memory
bool fillingTank = false;


void setup() {

  Serial.begin(9600);

  displaySerial.begin(9600);


  // Outputs

  pinMode(pump1, OUTPUT);
  pinMode(pump2, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(growLight, OUTPUT);

  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  pinMode(mainMotor, OUTPUT);


  // Ultrasonic

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);


  // Servo

  roofServo.attach(roofPin);

  roofServo.write(roofOpen);


  // Initial OFF

  digitalWrite(pump1, LOW);
  digitalWrite(pump2, LOW);

  digitalWrite(fan, LOW);

  digitalWrite(growLight, LOW);

  digitalWrite(mainMotor, LOW);

  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);


  Serial.println("GREENHOUSE STARTED");
}


void loop() {

  // =================================================
  // MOISTURE 1
  // =================================================

  int m1 = analogRead(moisture1);

  bool p1 = false;

  if (m1 < moistureLimit) {

    digitalWrite(pump1, HIGH);

    p1 = true;

  } else {

    digitalWrite(pump1, LOW);

    p1 = false;
  }


  // =================================================
  // MOISTURE 2
  // =================================================

  int m2 = analogRead(moisture2);

  bool p2 = false;

  if (m2 < moistureLimit) {

    digitalWrite(pump2, HIGH);

    p2 = true;

  } else {

    digitalWrite(pump2, LOW);

    p2 = false;
  }


  // =================================================
  // WATER TANK ULTRASONIC
  // =================================================

  digitalWrite(trigPin, LOW);

  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);

  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);


  long duration = pulseIn(echoPin, HIGH, 30000);

  float distance;


  if (duration == 0) {

    distance = 999;

  } else {

    distance = duration * 0.034 / 2;
  }


  // Low water → start filling

  if (distance >= waterLowLimit) {

    fillingTank = true;
  }


  // Full water → stop filling

  if (distance <= waterFullLimit) {

    fillingTank = false;
  }


  if (fillingTank) {

    digitalWrite(mainMotor, HIGH);

  } else {

    digitalWrite(mainMotor, LOW);
  }


  // =================================================
  // RAIN
  // =================================================

  int rainValue = analogRead(rainSensor);

  bool rain = false;


  if (rainValue < rainLimit) {

    roofServo.write(roofClosed);

    rain = true;

  } else {

    roofServo.write(roofOpen);

    rain = false;
  }


  // =================================================
  // TEMPERATURE
  // =================================================

  int tempRaw = analogRead(temperatureSensor);

  float voltage = tempRaw * (5.0 / 1023.0);

  float temperature = (voltage - 0.5) * 100.0;


  bool fanOn = false;


  if (temperature > temperatureLimit) {

    digitalWrite(fan, HIGH);

    fanOn = true;

  } else {

    digitalWrite(fan, LOW);

    fanOn = false;
  }


  // =================================================
  // LIGHT
  // =================================================

  int lightValue = analogRead(lightSensor);

  bool lowLight = false;


  if (lightValue < lightLimit) {

    digitalWrite(greenLED, HIGH);

    digitalWrite(redLED, LOW);

    digitalWrite(growLight, HIGH);

    lowLight = true;

  } else {

    digitalWrite(greenLED, LOW);

    digitalWrite(redLED, HIGH);

    digitalWrite(growLight, LOW);

    lowLight = false;
  }


  // =================================================
  // SEND DATA TO ARDUINO 2
  // =================================================

  displaySerial.print(m1);
  displaySerial.print(",");

  displaySerial.print(m2);
  displaySerial.print(",");

  displaySerial.print(temperature, 1);
  displaySerial.print(",");

  displaySerial.print(rain);
  displaySerial.print(",");

  displaySerial.print(lowLight);
  displaySerial.print(",");

  displaySerial.print(distance, 1);
  displaySerial.print(",");

  displaySerial.print(fillingTank);
  displaySerial.print(",");

  displaySerial.print(p1);
  displaySerial.print(",");

  displaySerial.print(p2);
  displaySerial.print(",");

  displaySerial.print(fanOn);

  displaySerial.println();


  // Debug only on Arduino 1

  Serial.print("M1:");
  Serial.print(m1);

  Serial.print(" M2:");
  Serial.print(m2);

  Serial.print(" TEMP:");
  Serial.print(temperature);

  Serial.print(" TANK:");
  Serial.print(distance);

  Serial.print(" MAIN:");

  Serial.println(fillingTank ? "ON" : "OFF");


  delay(500);
}
