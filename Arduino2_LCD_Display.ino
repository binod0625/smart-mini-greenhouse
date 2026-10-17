#include <Wire.h>
#include <LiquidCrystal_I2C.h>


// ================= LCD =================

LiquidCrystal_I2C lcd(0x27, 16, 2);


// ================= DATA =================

int m1 = 0;
int m2 = 0;

float temperature = 0;

bool rain = false;
bool lowLight = false;

float tankDistance = 0;

bool fillingTank = false;

bool pump1 = false;
bool pump2 = false;

bool fan = false;


// ================= SERIAL BUFFER =================

String data = "";


// ================= LCD SCREEN =================

unsigned long previousDisplay = 0;

int screen = 0;


// ================= SETUP =================

void setup() {

  // Arduino 1 D2 → Arduino 2 D0

  Serial.begin(9600);


  // LCD

  lcd.init();

  lcd.backlight();

  lcd.clear();


  lcd.setCursor(0, 0);
  lcd.print("MINI GREENHOUSE");

  lcd.setCursor(0, 1);
  lcd.print("SYSTEM READY");


  delay(2000);

  lcd.clear();
}


// ================= LOOP =================

void loop() {


  // =================================================
  // RECEIVE DATA FROM ARDUINO 1
  // =================================================

  while (Serial.available() > 0) {

    char c = Serial.read();


    if (c == '\n') {

      if (data.length() > 0) {

        parseData(data);

        data = "";
      }

    }

    else if (c != '\r') {

      data += c;
    }


    // Prevent buffer overflow

    if (data.length() > 100) {

      data = "";
    }
  }


  // =================================================
  // CHANGE LCD SCREEN
  // =================================================

  if (millis() - previousDisplay >= 2000) {

    previousDisplay = millis();


    lcd.clear();


    // =================================================
    // SCREEN 0 — MOISTURE
    // =================================================

    if (screen == 0) {

      lcd.setCursor(0, 0);

      lcd.print("M1:");

      if (m1 < 400)
        lcd.print("LOW");
      else
        lcd.print("OK ");


      lcd.print(" P1:");

      if (pump1)
        lcd.print("ON");
      else
        lcd.print("OFF");


      lcd.setCursor(0, 1);

      lcd.print("M2:");

      if (m2 < 400)
        lcd.print("LOW");
      else
        lcd.print("OK ");


      lcd.print(" P2:");

      if (pump2)
        lcd.print("ON");
      else
        lcd.print("OFF");
    }


    // =================================================
    // SCREEN 1 — TEMPERATURE
    // =================================================

    else if (screen == 1) {

      lcd.setCursor(0, 0);

      lcd.print("TEMP:");

      lcd.print(temperature, 1);

      lcd.print("C");


      lcd.setCursor(0, 1);

      lcd.print("FAN:");

      if (fan)
        lcd.print("ON ");
      else
        lcd.print("OFF");
    }


    // =================================================
    // SCREEN 2 — WATER TANK
    // =================================================

    else if (screen == 2) {

      lcd.setCursor(0, 0);

      lcd.print("TANK:");

      if (fillingTank)
        lcd.print("LOW/FILL");
      else
        lcd.print("FULL/OK");


      lcd.setCursor(0, 1);

      lcd.print("MAIN:");

      if (fillingTank)
        lcd.print("ON ");
      else
        lcd.print("OFF");
    }


    // =================================================
    // SCREEN 3 — RAIN / ROOF
    // =================================================

    else if (screen == 3) {

      lcd.setCursor(0, 0);

      lcd.print("RAIN:");

      if (rain)
        lcd.print("YES");
      else
        lcd.print("NO ");


      lcd.setCursor(0, 1);

      lcd.print("ROOF:");

      if (rain)
        lcd.print("CLOSED");
      else
        lcd.print("OPEN");
    }


    // =================================================
    // SCREEN 4 — LIGHT
    // =================================================

    else if (screen == 4) {

      lcd.setCursor(0, 0);

      lcd.print("LIGHT:");

      if (lowLight)
        lcd.print("LOW");
      else
        lcd.print("NORMAL");


      lcd.setCursor(0, 1);

      lcd.print("GROW:");

      if (lowLight)
        lcd.print("ON ");
      else
        lcd.print("OFF");
    }


    // Next screen

    screen++;

    if (screen > 4) {

      screen = 0;
    }
  }
}


// =================================================
// PARSE DATA
// =================================================

void parseData(String data) {

  int values[10];

  int index = 0;

  int start = 0;


  for (int i = 0; i <= data.length(); i++) {

    if (data.charAt(i) == ',' || i == data.length()) {

      String part = data.substring(start, i);

      values[index] = part.toInt();

      index++;

      start = i + 1;


      if (index >= 10) {

        break;
      }
    }
  }


  // Only accept complete data

  if (index == 10) {

    m1 = values[0];

    m2 = values[1];

    temperature = values[2];

    rain = values[3];

    lowLight = values[4];

    tankDistance = values[5];

    fillingTank = values[6];

    pump1 = values[7];

    pump2 = values[8];

    fan = values[9];
  }
}
