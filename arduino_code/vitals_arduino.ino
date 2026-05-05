#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MAX30100_PulseOximeter.h>
#include <SoftwareSerial.h>

SoftwareSerial espSerial(10, 11); // RX=D10, TX=D11
LiquidCrystal_I2C lcd(0x27, 16, 2);
PulseOximeter pox;

#define REPORT_MS 1000
uint32_t lastReport = 0;

void onBeatDetected() {}

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

    // ── LCD ──
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BPM:");
    lcd.print((int)bpm);
    lcd.print(" O2:");
    lcd.print((int)spo2);
    lcd.print("%");

    lcd.setCursor(0, 1);
    if (bpm == 0 && spo2 == 0) {
      lcd.print("Place finger...");
    } else if (bpm < 50 || bpm > 120 || spo2 < 94) {
      lcd.print("!! ABNORMAL !!");
    } else {
      lcd.print("Status: Normal");
    }

    // ── Send to ESP8266 — clean format only ──
    espSerial.print("H,");
    espSerial.print((int)bpm);
    espSerial.print(",");
    espSerial.println((int)spo2);

    Serial.print("Sent → BPM:");
    Serial.print((int)bpm);
    Serial.print(" O2:");
    Serial.println((int)spo2);

    lastReport = millis();
  }
}