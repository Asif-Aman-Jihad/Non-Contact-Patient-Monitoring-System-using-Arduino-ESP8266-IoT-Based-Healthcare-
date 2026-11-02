# Non-Contact Patient Vitals Monitor 🏥

An IoT-based real-time patient vitals monitoring system built with Arduino Uno 
and ESP8266 NodeMCU. Measures body temperature without contact using an infrared 
sensor, and monitors blood oxygen (SpO2) and heart rate using a pulse oximeter. 
Abnormal readings trigger an instant alert on a mobile phone via the Blynk IoT app.

---

## 📌 Features

- 🌡️ Non-contact body temperature measurement (MLX90614 IR sensor)
- ❤️ Heart rate (BPM) and SpO2 monitoring (MAX30100)
- 📟 Live display on 16x2 LCD screen
- 🔔 Buzzer + LED alert for abnormal readings
- 📱 Real-time data sent to Blynk IoT mobile app via WiFi
- ☁️ Cloud logging via Blynk virtual pins

---

## 🔧 Hardware Required

| Component | Purpose |
|---|---|
| Arduino Uno R3 | Main microcontroller — reads MAX30100, drives LCD |
| ESP8266 NodeMCU V2 | WiFi module — reads MLX90614, sends data to Blynk |
| MLX90614 (GY-906) | Non-contact IR temperature sensor |
| MAX30100 (Green PCB) | SpO2 and heart rate sensor |
| LCD 16x2 + PCF8574 I2C adapter | Local data display |
| Active Buzzer | Audio alert for abnormal vitals |
| LED + 220Ω resistor | Visual alert |
| Jumper wires + Breadboard | Prototyping |
| USB Powerbank | Power supply |

---

## 📐 System Architecture
Arduino Uno                          ESP8266 NodeMCU
─────────────────────                ───────────────────────
MAX30100 (A4/A5 I2C)                 MLX90614 (D1/D2 I2C)
LCD 16x2 (A4/A5 I2C)                WiFi + Blynk IoT
│                                    │
└── D11 TX ──[voltage divider]──→ D7 RX
reads BPM + SpO2
adds temperature
sends all 3 to Blynk
---

## 📊 Alert Thresholds

| Parameter | Normal Range | Alert Triggered |
|---|---|---|
| Body Temperature | 36.1°C – 37.2°C | Above 38°C |
| SpO2 | 95% – 100% | Below 94% |
| Heart Rate | 60 – 100 BPM | Below 50 or above 120 |

---

## 🔌 Wiring Summary

### Arduino Uno
| Component | Arduino Pin |
|---|---|
| MAX30100 SDA | A4 |
| MAX30100 SCL | A5 |
| MAX30100 VCC | 3.3V |
| LCD SDA | A4 |
| LCD SCL | A5 |
| LCD VCC | 5V |
| TX to ESP8266 (via 1kΩ+2kΩ divider) | D11 |

### ESP8266 NodeMCU
| Component | ESP8266 Pin |
|---|---|
| MLX90614 SDA | D2 (GPIO4) |
| MLX90614 SCL | D1 (GPIO5) |
| MLX90614 VCC | Vin (5V) |
| RX from Arduino (after divider) | D7 (GPIO13) |

---

## 📚 Libraries Required

### Arduino Uno
- MAX30100lib by OXullo Intersecans
- LiquidCrystal I2C by Frank de Brabander
- SoftwareSerial (built-in)
- Wire (built-in)

### ESP8266 NodeMCU
- Adafruit MLX90614
- Adafruit BusIO
- Blynk by Volodymyr Shymanskyy
- ESP8266WiFi (included in ESP8266 board package)
- Wire (built-in)

---

## ⚙️ Setup Instructions

### 1. Arduino IDE Setup
- Install Arduino IDE from https://www.arduino.cc/en/software
- Install ESP8266 board package:
  - Go to File → Preferences
  - Add this URL to Additional Boards Manager URLs:
    `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
  - Go to Tools → Board → Boards Manager → search ESP8266 → Install

### 2. Install Libraries
- Go to Sketch → Include Library → Manage Libraries
- Install all libraries listed above one by one

### 3. Blynk App Setup
- Download Blynk IoT app on your phone
- Create a new template — device: ESP8266
- Add 3 datastreams:
  - V0 → Heart Rate (BPM)
  - V1 → SpO2 (%)
  - V2 → Temperature (°C)
- Add an Event named `alert` with push notification enabled
- Copy your Template ID and Auth Token

### 4. Upload Arduino Uno Code
- Open `arduino_code/vitals_arduino.ino`
- Select Board: Arduino Uno
- Select correct COM port
- **Disconnect D11 wire (Arduino TX to ESP8266 RX) before uploading.** D10 is not used in this sketch. Reconnect D11 after upload.
- Upload

### 5. Upload ESP8266 Code
- Open `esp_code/vitals_esp8266.ino`
- Fill in your WiFi credentials and Blynk tokens
- Select Board: NodeMCU 1.0 (ESP-12E Module)
- Select correct COM port
- Upload

---

## 📁 Repository Structure
non-contact-patient-vitals-monitor/
│
├── arduino_code/
│   └── vitals_arduino.ino       # MAX30100 + LCD code
│
├── esp_code/
│   └── vitals_esp8266.ino       # MLX90614 + WiFi + Blynk code
│
├── docs/
│   └── circuit_diagram.png      # Full wiring diagram
│   └── complete_guide.docx      # Step-by-step build guide
---

## ⚠️ Important Notes

- Never connect 5V to ESP8266 GPIO pins — 3.3V logic only
- Always disconnect D11 (Arduino TX to ESP8266 RX) wires before uploading to Arduino Uno (D10 is unused by this sketch). Reconnect after upload.
- MAX30100 must be powered from 3.3V not 5V
- MLX90614 VCC connects to 5V but SDA/SCL operate at 3.3V level on ESP8266
- Use Serial.swap() on ESP8266 to receive Arduino data on D7 (GPIO13) without interfering with USB Serial Monitor. Note: Serial.swap() remaps UART0; ensure correct wiring and be cautious during flashing/debugging.
- Place finger flat and still on MAX30100 — wait 15 seconds for stable reading
- Point MLX90614 at forehead from 3–5 cm distance

---

## 🌍 Real World Impact

This project addresses a critical need in healthcare settings in Bangladesh 
and similar developing countries where:
- Nursing staff is limited and cannot continuously monitor every patient
- Infectious disease wards require minimal physical contact with patients
- Rural clinics lack expensive hospital monitoring equipment

This system provides a low-cost alternative (under ৳3,000) to commercial 
patient monitors that cost hundreds of thousands of taka.

---



## 👨‍💻 Author

**Asif Aman Jihad**  
Department of Computer Science  
American International University-Bangladesh

---

## 📜 License

MIT License — free to use, modify and distribute with attribution.