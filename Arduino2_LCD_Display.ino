#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo roofServo;

// =========================
// ROOF PINS
// =========================

const int openButton  = 4;
const int closeButton = 5;

const int servoPin = 11;

// =========================
// SERVO ANGLES
// =========================

const int roofOpen = 0;
const int roofClosed = 90;

// =========================
// RAIN THRESHOLD
// =========================

const int rainLimit = 600;


// =========================
// VARIABLES
// =========================

int moisture1;
int moisture2;

float temperature;

int rainValue;

bool lowLight;

long distance;

bool fillingTank;

bool pump1;
bool pump2;
bool fan;

bool roofIsClosed = true;

int screen = 0;

unsigned long previousScreenTime = 0;


// =========================
// SETUP
// =========================

void setup() {

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  roofServo.attach(servoPin);

  pinMode(openButton, INPUT_PULLUP);
  pinMode(closeButton, INPUT_PULLUP);

  // Start roof CLOSED
  roofServo.write(roofClosed);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("SMART GREENHOUSE");

  lcd.setCursor(0, 1);
  lcd.print("SYSTEM READY");

  delay(2000);

  lcd.clear();
}


// =========================
// READ DATA FROM ARDUINO 1
// =========================

void readData() {

  if (Serial.available()) {

    String data = Serial.readStringUntil('\n');

    int index = 0;

    moisture1 = data.substring(0, data.indexOf(',')).toInt();
    data.remove(0, data.indexOf(',') + 1);

    moisture2 = data.substring(0, data.indexOf(',')).toInt();
    data.remove(0, data.indexOf(',') + 1);

    temperature = data.substring(0, data.indexOf(',')).toFloat();
    data.remove(0, data.indexOf(',') + 1);

    rainValue = data.substring(0, data.indexOf(',')).toInt();
    data.remove(0, data.indexOf(',') + 1);

    lowLight = data.substring(0, data.indexOf(',')).toInt();
    data.remove(0, data.indexOf(',') + 1);

    distance = data.substring(0, data.indexOf(',')).toInt();
    data.remove(0, data.indexOf(',') + 1);

    fillingTank = data.substring(0, data.indexOf(',')).toInt();
    data.remove(0, data.indexOf(',') + 1);

    pump1 = data.substring(0, data.indexOf(',')).toInt();
    data.remove(0, data.indexOf(',') + 1);

    pump2 = data.substring(0, data.indexOf(',')).toInt();
    data.remove(0, data.indexOf(',') + 1);

    fan = data.toInt();
  }
}


// =========================
// ROOF CONTROL
// =========================

void roofControl() {

  // -------------------------
  // RAIN HAS HIGHEST PRIORITY
  // -------------------------

  if (rainValue > rainLimit) {

    roofServo.write(roofClosed);

    roofIsClosed = true;

    return;
  }


  // -------------------------
  // MANUAL OPEN
  // -------------------------

  if (digitalRead(openButton) == LOW) {

    roofServo.write(roofOpen);

    roofIsClosed = false;

    delay(300);
  }


  // -------------------------
  // MANUAL CLOSE
  // -------------------------

  if (digitalRead(closeButton) == LOW) {

    roofServo.write(roofClosed);

    roofIsClosed = true;

    delay(300);
  }
}


// =========================
// LCD SCREEN
// =========================

void showScreen() {

  lcd.clear();


  // SCREEN 1
  if (screen == 0) {

    lcd.setCursor(0, 0);
    lcd.print("M1:");
    lcd.print(moisture1);

    lcd.setCursor(0, 1);
    lcd.print("M2:");
    lcd.print(moisture2);
  }


  // SCREEN 2
  else if (screen == 1) {

    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(temperature, 1);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("Fan:");
    lcd.print(fan ? "ON" : "OFF");
  }


  // SCREEN 3
  else if (screen == 2) {

    lcd.setCursor(0, 0);
    lcd.print("Tank:");
    lcd.print(distance);
    lcd.print("cm");

    lcd.setCursor(0, 1);
    lcd.print("Motor:");
    lcd.print(fillingTank ? "ON" : "OFF");
  }


  // SCREEN 4
  else if (screen == 3) {

    lcd.setCursor(0, 0);

    if (rainValue > rainLimit) {
      lcd.print("RAIN DETECTED");
    }
    else {
      lcd.print("NO RAIN");
    }

    lcd.setCursor(0, 1);
    lcd.print("Roof:");
    lcd.print(roofIsClosed ? "CLOSED" : "OPEN");
  }


  // SCREEN 5
  else if (screen == 4) {

    lcd.setCursor(0, 0);

    lcd.print("Light:");
    lcd.print(lowLight ? "LOW" : "OK");

    lcd.setCursor(0, 1);

    lcd.print("Grow:");
    lcd.print(lowLight ? "ON" : "OFF");
  }
}


// =========================
// MAIN LOOP
// =========================

void loop() {

  // Receive data
  readData();


  // Control roof
  roofControl();


  // Change LCD screen
  if (millis() - previousScreenTime >= 2000) {

    previousScreenTime = millis();

    showScreen();

    screen++;

    if (screen > 4) {
      screen = 0;
    }
  }
}
