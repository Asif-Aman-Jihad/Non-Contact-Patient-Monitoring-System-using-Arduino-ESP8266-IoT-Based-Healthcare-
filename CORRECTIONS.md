# CORRECTIONS

## Lote 1
- REA-011: Fixed README paths (arduino_code/, esp_code/), clarified upload/wiring

## Lote 2
- REA-004/010: Added parsing and reading validation (MAX30100)
- REA-006: Skip sending H,0,0 invalid readings
- REA-005: MLX90614 failure marked unavailable, no 0C sent

## Lote 3
- REA-007: WiFi/Blynk reconnection, data freshness tracking

## Lote 4
- REA-002: Blynk alerts (temp>38, SpO2<94, HR<50 or >120) with 60s cooldown

## Lote 5
- REA-001/008: Clarified wiring/upload/Serial.swap()
- REA-003: Evaluated as proposed (no explicit requirement)
- REA-009: No demonstrable redundancy

## Files
- arduino_code/vitals_arduino.ino
- esp_code/vitals_esp8266.ino
- readme.md

## Tests
- Static analysis only (no hardware). Logic validated for truncation/invalid/failure/reconnect.
