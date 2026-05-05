#define BLYNK_TEMPLATE_ID "You will find it in the blynk app"
#define BLYNK_TEMPLATE_NAME "ESP8266"
#define BLYNK_AUTH_TOKEN    "From App add device then find your auth token"
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_MLX90614.h>

char ssid[] = "Your wifi name";
char pass[] = "Your wifi password";

Adafruit_MLX90614 mlx;

float bpm  = 0;
float spo2 = 0;
String incoming = "";
unsigned long lastSend = 0;

void setup() {
  // ── Start Serial then SWAP to D7(RX) D8(TX) ──
  Serial.begin(9600);
  Serial.swap();   // RX moves to GPIO13 = D7, TX moves to GPIO15 = D8

  Wire.begin(D2, D1);       // SDA=D2, SCL=D1
  Wire.setClock(100000);

  if (!mlx.begin()) {
    // continue even if MLX fails — don't block
  }

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();
}

void loop() {
  Blynk.run();

  // ── Read from Arduino on D7 ──
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      if (incoming.startsWith("H,")) {
        int c1 = incoming.indexOf(',');
        int c2 = incoming.indexOf(',', c1 + 1);
        bpm  = incoming.substring(c1 + 1, c2).toFloat();
        spo2 = incoming.substring(c2 + 1).toFloat();
      }
      incoming = "";
    } else if (c != '\r') {
      incoming += c;
    }
  }

  // ── Send to Blynk every 2 seconds ──
  if (millis() - lastSend > 2000) {
    Wire.setClock(100000);
    float temp = mlx.readObjectTempC();

    if (Blynk.connected()) {
      Blynk.virtualWrite(V0, bpm);
      Blynk.virtualWrite(V1, spo2);
      Blynk.virtualWrite(V2, isnan(temp) ? 0 : temp);
    }

    lastSend = millis();
  }
}