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
float tempC = 0;
bool tempAvailable = false;
String incoming = "";
unsigned long lastSend = 0;
unsigned long lastValidData = 0;
bool alertActive = false;
unsigned long lastAlertTime = 0;

#define ALERT_COOLDOWN_MS 60000
#define DATA_TIMEOUT_MS 10000

#define TEMP_HIGH 38.0
#define SPO2_LOW  94.0
#define BPM_LOW   50.0
#define BPM_HIGH  120.0

bool parseHeartbeat(String s, float &hb, float &ox) {
  if (!s.startsWith("H,")) return false;
  int c1 = s.indexOf(',');
  if (c1 < 0) return false;
  int c2 = s.indexOf(',', c1 + 1);
  if (c2 < 0) return false;
  String sbpm = s.substring(c1 + 1, c2);
  String sspo2 = s.substring(c2 + 1);
  if (sbpm.length() == 0 || sspo2.length() == 0) return false;
  hb = sbpm.toFloat();
  ox = sspo2.toFloat();
  if (isnan(hb) || isnan(ox)) return false;
  if (hb <= 0 || ox <= 0) return false;
  if (hb < 40 || hb > 200 || ox < 70 || ox > 100) return false;
  return true;
}

void setup() {
  Serial.begin(9600);
  Serial.swap();

  Wire.begin(D2, D1);
  Wire.setClock(100000);

  if (mlx.begin()) {
    tempAvailable = true;
  } else {
    tempAvailable = false;
  }

  WiFi.begin(ssid, pass);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 15000) {
    delay(500);
  }

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    delay(1000);
  }
  if (Blynk.connected() == false) {
    Blynk.connect();
    delay(1000);
  }

  Blynk.run();

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      float hb = 0, ox = 0;
      if (parseHeartbeat(incoming, hb, ox)) {
        bpm = hb;
        spo2 = ox;
        lastValidData = millis();
      }
      incoming = "";
    } else if (c != '\r') {
      incoming += c;
    }
  }

  if (millis() - lastSend > 2000) {
    Wire.setClock(100000);
    if (tempAvailable) {
      float t = mlx.readObjectTempC();
      if (!isnan(t)) {
        tempC = t;
      } else {
        tempAvailable = false;
      }
    }

    bool dataFresh = (millis() - lastValidData < DATA_TIMEOUT_MS) && bpm > 0 && spo2 > 0;

    if (Blynk.connected()) {
      if (dataFresh) {
        Blynk.virtualWrite(V0, bpm);
        Blynk.virtualWrite(V1, spo2);
      }
      if (tempAvailable) {
        Blynk.virtualWrite(V2, tempC);
      }
    }

    if (dataFresh && Blynk.connected()) {
      bool abnormal = false;
      String alertMsg = "";
      if (tempAvailable && tempC > TEMP_HIGH) {
        abnormal = true;
        alertMsg += "Temp high ";
      }
      if (spo2 < SPO2_LOW) {
        abnormal = true;
        alertMsg += "SpO2 low ";
      }
      if (bpm < BPM_LOW || bpm > BPM_HIGH) {
        abnormal = true;
        alertMsg += "HR abnormal ";
      }
      if (abnormal && (millis() - lastAlertTime > ALERT_COOLDOWN_MS)) {
        Blynk.logEvent("alert", alertMsg);
        lastAlertTime = millis();
        alertActive = true;
      }
    }

    lastSend = millis();
  }
}
