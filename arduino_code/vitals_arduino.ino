#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MAX30100_PulseOximeter.h>
#include <SoftwareSerial.h>

SoftwareSerial espSerial(10, 11);
LiquidCrystal_I2C lcd(0x27, 16, 2);
PulseOximeter pox;

#define REPORT_MS 1000
uint32_t lastReport = 0;

#define BPM_MIN_VALID 40
#define BPM_MAX_VALID 200
#define SPO2_MIN_VALID 70
#define SPO2_MAX_VALID 100

void onBeatDetected() {}

bool isValidReading(float bpm, float spo2) {
  if (isnan(bpm) || isnan(spo2)) return false;
  if (bpm <= 0 || spo2 <= 0) return false;
  if (bpm < BPM_MIN_VALID || bpm > BPM_MAX_VALID) return false;
  if (spo2 < SPO2_MIN_VALID || spo2 > SPO2_MAX_VALID) return false;
  return true;
}

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
  Wire.begin();
  Wire.setClock(100000);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Heart Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(2000);

  if (!pox.begin()) {
    lcd.clear();
    lcd.print("MAX30100 ERROR!");
    while (1);
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_27_1MA);
  pox.setOnBeatDetectedCallback(onBeatDetected);
  lcd.clear();
  lcd.print("Place finger...");
}

void loop() {
  pox.update();

  if (millis() - lastReport >= REPORT_MS) {
    float bpm  = pox.getHeartRate();
    float spo2 = pox.getSpO2();

    lcd.clear();
    lcd.setCursor(0, 0);
    if (isValidReading(bpm, spo2)) {
      lcd.print("BPM:");
      lcd.print((int)bpm);
      lcd.print(" O2:");
      lcd.print((int)spo2);
      lcd.print("%");

      lcd.setCursor(0, 1);
      if (bpm < 50 || bpm > 120 || spo2 < 94) {
        lcd.print("!! ABNORMAL !!");
      } else {
        lcd.print("Status: Normal");
      }
    } else if (bpm <= 0 && spo2 <= 0) {
      lcd.print("BPM:-- O2:--%");
      lcd.setCursor(0, 1);
      lcd.print("Place finger...");
    } else {
      lcd.print("BPM:-- O2:--%");
      lcd.setCursor(0, 1);
      lcd.print("Reading error");
    }

    if (isValidReading(bpm, spo2)) {
      espSerial.print("H,");
      espSerial.print((int)bpm);
      espSerial.print(",");
      espSerial.println((int)spo2);
      Serial.print("Sent -> BPM:");
      Serial.print((int)bpm);
      Serial.print(" O2:");
      Serial.println((int)spo2);
    } else {
      Serial.println("Skipped invalid reading");
    }

    lastReport = millis();
  }
}
