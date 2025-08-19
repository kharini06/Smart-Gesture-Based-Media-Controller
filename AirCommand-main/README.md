# AirCommand ✋📶
*Gesture-Controlled Laptop Interface using ESP32 + MPU6050 + Edge Impulse*

**AirCommand** is a gesture-based control system that allows you to control your laptop over Wi-Fi using hand movements detected by an ESP32 + MPU6050 sensor. The ESP32 runs a gesture classification model and sends the detected gestures to a laptop or server via Wi-Fi, where appropriate actions like volume control or desktop switching are executed.

While our long-term goal is to embed this into a wearable form.

---

## Overview
AirCommand uses:
- An **ESP32 DevKit** for connectivity and processing  
- An **MPU6050 sensor** to detect motion  
- A **pre-trained ML model** from **Edge Impulse** for gesture classification  
- A **Python server** on your laptop to receive gesture commands and control the system

---

## Demo Use Case
Raise your hand, swipe, or tap—your laptop responds instantly:
- Make Up-down movement to increase volume  
- Make Left-right to decrease volume
- Tap to pause/play media  

*Totally touchless. Totally awesome.*

---

## Project Structure
```
AirCommand/
├── Aurdino_wifi.ino           # ESP32 Arduino code for gesture detection & Wi-Fi HTTP requests
├── esp32_acc_inferencing.zip  # Edge Impulse library (import into Arduino)
├── gesture_server.py          # Python script to receive gestures and trigger actions
├── README.md                  # This documentation
```

---

## Hardware Requirements
- **ESP32 DevKit v1 (WROOM-32)**
- **MPU6050 Accelerometer + Gyroscope**
- USB cable for ESP32  
- Wi-Fi-enabled laptop or PC  

---

## Wiring Diagram
| MPU6050 Pin | ESP32 Pin |
|-------------|-----------|
| VCC         | 3.3V      |
| GND         | GND       |
| SCL         | GPIO 22   |
| SDA         | GPIO 21   |

---

## Getting Started
### 1. Arduino IDE Setup
- Install Arduino IDE: [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)
- Install ESP32 board support via Board Manager
- Install required libraries:
  - `Wire.h`
  - `WiFi.h`
  - `esp32_acc_inferencing.h` (included in the zip)
- Download and extract `esp32_acc_inferencing.zip`
- Go to `Sketch > Include Library > Add .ZIP Library`
- Import the zip file to include the Edge Impulse model

### 2. Upload Arduino Code
- Open `Aurdino_wifi.ino`
- Replace the placeholders with your **Wi-Fi SSID**, **password**, and **server IP**
- Upload the sketch to the ESP32

### 3. Run Python Server on Laptop
Install dependencies:
```bash
pip install pyautogui pynput
```

Run the script:
```bash
python gesture_server.py
```

Your laptop is now listening for HTTP POST requests from the ESP32.

---

## How It Works
1. MPU6050 collects real-time motion data
2. ESP32 runs a gesture classification model locally
3. On detecting a gesture (`"U"`, `"L"`, `"T"`), it sends a request to the Python server
4. The Python server maps these gestures to system actions using `pyautogui` or `pynput`

---

## Supported Gestures
| Gesture Code | Action               |
|--------------|----------------------|
| "U"          | Volume Up            |
| "L"          | Volume Down          |
| "T"          | Play/Pause Media     |

---

## Future Improvements
- Miniaturize into a wearable device (wristband)  
- Add more customizable gestures  
- Include real-time gesture feedback (vibration or LEDs)  

---

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
