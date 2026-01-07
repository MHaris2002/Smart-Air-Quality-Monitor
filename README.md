# 🌬️ Smart Air Quality Monitoring System

AI-powered IoT solution for real-time air quality monitoring using ESP32, machine learning, and cloud integration.

[![Wokwi](https://img.shields.io/badge/Wokwi-Simulation-green)](https://wokwi.com)
[![Blynk](https://img.shields.io/badge/Blynk-IoT%20Platform-blue)](https://blynk.io)
[![Python](https://img.shields.io/badge/Python-ML%20Training-yellow)](https://www.python.org/)

## 📋 Project Overview

This project implements an intelligent air quality monitoring system combining:
- **Edge AI Processing**: Random Forest classifier deployed on ESP32 (99.86% accuracy)
- **Multi-Sensor Integration**: MQ-135 gas sensor + DHT22 temperature/humidity
- **Cloud Connectivity**: Real-time data visualization via Blynk IoT platform
- **Multi-Modal Alerts**: LED indicators, LCD display, buzzer, and automated ventilation

**Course**: AI for IoT (LD7182)  
**Institution**: Northumbria University  
**Date**: January 2026

---

## 🎯 Key Features

✅ **Real-time AQI Classification** (6 categories: Good to Severe)  
✅ **Edge AI Inference** (<1 second latency)  
✅ **Cloud Dashboard** (Blynk mobile/web interface)  
✅ **Automated Alerts** (Visual, auditory, mechanical)  
✅ **Low Cost** (~£25 total hardware cost)  
✅ **Simulation Ready** (Fully tested in Wokwi)

---

## 🛠️ Hardware Components

| Component | Model | Purpose |
|-----------|-------|---------|
| Microcontroller | ESP32-S3 | Main processing unit with WiFi |
| Gas Sensor | MQ-135 | Multi-gas detection (CO, NOx, VOCs) |
| Temp/Humidity | DHT22 | Environmental monitoring |
| Display | LCD 16×2 (I2C) | Local status display |
| Indicators | RGB LEDs | Visual air quality signaling |
| Alert | Buzzer | Audible warnings |
| Actuator | Relay Module | Automated ventilation control |

---

## 📂 Repository Structure
```
├── Arduino_Code/          # ESP32 source code
├── Machine_Learning/      # Jupyter notebook for ML training
├── Screenshots/           # System outputs and visualizations
├── Wokwi_Project/        # Importable Wokwi simulation
└── Documentation/         # Architecture diagrams
```

---

## 🚀 Quick Start

### Option 1: Run in Wokwi Simulator (Recommended)

1. **Open Wokwi**: https://wokwi.com/
2. **Create New Project**: Arduino ESP32
3. **Copy Code**:
   - Copy contents from `Arduino_Code/sketch.ino`
   - Copy contents from `Arduino_Code/diagram.json`
4. **Start Simulation**: Click green play button
5. **Observe**: LCD shows air quality status, LEDs indicate levels

### Option 2: Deploy to Physical Hardware

1. **Install Arduino IDE**: https://www.arduino.cc/
2. **Add ESP32 Board Support**: 
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://dl.espressif.com/dl/package_esp32_index.json`
3. **Install Libraries**:
   - DHT sensor library
   - LiquidCrystal I2C
   - Blynk (for cloud features)
4. **Upload Code**: Select ESP32 Dev Module, upload sketch
5. **Configure WiFi**: Update SSID and password in code
6. **Connect Sensors**: Follow circuit diagram in Documentation/

---

## 🧠 Machine Learning Model

**Algorithm**: Random Forest Classifier  
**Dataset**: UCI Air Quality Dataset (9,357 samples)  
**Features**: 7 environmental parameters  
**Accuracy**: 99.86% on test set  
**Categories**: 6 AQI levels (Good to Severe)

### Training the Model

1. Open `Machine_Learning/Air_Quality_ML_Training.ipynb` in Google Colab
2. Run all cells to:
   - Load UCI dataset
   - Preprocess data
   - Train Random Forest & Gradient Boosting models
   - Evaluate performance
   - Visualize results

---

## 📊 System Architecture
```
┌─────────────────┐       ┌─────────────────┐       ┌─────────────────┐
│  SENSING LAYER  │  ───► │  EDGE AI LAYER  │  ───► │   CLOUD LAYER   │
│                 │       │                 │       │                 │
│ • MQ-135 Sensor │       │ • ML Model      │       │ • Blynk Server  │
│ • DHT22 Sensor  │       │ • AQI Classify  │       │ • Dashboard     │
│ • ESP32-S3      │       │ • Real-time     │       │ • Notifications │
│ • LCD Display   │       │   Processing    │       │ • Data Storage  │
└─────────────────┘       └─────────────────┘       └─────────────────┘
```

---

## 🎨 Screenshots

### Blynk Dashboard

| Good Quality | Moderate Quality | Very Poor Quality |
|--------------|------------------|-------------------|
| ![Good](Screenshots/Blynk_Dashboard/good_state.png) | ![Moderate](Screenshots/Blynk_Dashboard/moderate_state.png) | ![Poor](Screenshots/Blynk_Dashboard/verypoor_state.png) |

### Wokwi Simulation

| Circuit Diagram | Green LED (Good) | Red LED (Poor) |
|-----------------|------------------|----------------|
| ![Circuit](Screenshots/Wokwi_Simulation/circuit.png) | ![Green](Screenshots/Wokwi_Simulation/green_led.png) | ![Red](Screenshots/Wokwi_Simulation/red_led.png) |

---

## 📱 Blynk Configuration

**Template ID**: `TMPL5psTX8YqQ`  
**Template Name**: Smart Air Quality Monitor

### Virtual Pin Mapping

| Pin | Parameter | Widget Type | Range |
|-----|-----------|-------------|-------|
| V0 | PM2.5 (µg/m³) | Gauge | 0-500 |
| V1 | PM10 (µg/m³) | Gauge | 0-600 |
| V2 | CO (ppm) | Gauge | 0-1000 |
| V3 | Temperature (°C) | Chart | -10 to 50 |
| V4 | Humidity (%) | Chart | 0-100 |
| V5 | AQI Score | Level | 0-500 |
| V6 | Health Category | Label | Text |
| V7 | Alert Status | LED | Boolean |

---

## 🧪 Testing

### Manual Testing Mode

For demonstration purposes, the system includes manual AQI injection:
```cpp
bool MANUAL_MODE = true;   // Enable manual mode
int MANUAL_AQI = 150;      // Set desired AQI value
```

**Test Scenarios**:
- **AQI 25**: Good (Green LED, no alerts)
- **AQI 75**: Satisfactory (Green LED)
- **AQI 150**: Moderate (Yellow LED)
- **AQI 250**: Poor (Red LED + Buzzer)
- **AQI 350**: Very Poor (Red LED + Buzzer + Fan)
- **AQI 450**: Severe (All alerts, emergency)

---

## 📈 Performance Metrics

| Metric | Value |
|--------|-------|
| ML Model Accuracy | 99.86% |
| Inference Latency | <1 second |
| Update Frequency | 3 seconds |
| System Cost | ~£25 |
| Power Consumption | 0.53W (ESP32) |

---

## 🔧 Troubleshooting

**Issue**: LCD shows "Connecting..."  
**Solution**: Wait 15-20 seconds for WiFi initialization

**Issue**: Blynk shows "Device Offline"  
**Solution**: Verify Auth Token and WiFi credentials

**Issue**: Unrealistic sensor values  
**Solution**: Restart simulation to reset sensors

**Issue**: Manual mode not working  
**Solution**: Ensure `MANUAL_MODE = true` and restart

---

## 📚 Documentation

Full technical report available in course submission including:
- Literature review (IoT, Edge AI, Cloud platforms)
- System design and architecture
- ML model training methodology
- Implementation details
- Ethical considerations
- Critical evaluation

---

## 🤝 Contributing

This is an academic project, but suggestions and improvements are welcome:
1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Open a Pull Request

---

## 📄 License

This project is licensed under the MIT License - see LICENSE file for details.

---

## 👤 Author

**Muhammad Haris**  
Northumbria University  
Module: LD7182 - AI for IoT

---

## 🙏 Acknowledgments

- UCI Machine Learning Repository for air quality dataset
- Wokwi for simulation platform
- Blynk for IoT cloud infrastructure
- Northumbria University for academic support

---

## 📧 Contact

For questions or collaboration:
- **GitHub**: [@yourusername](https://github.com/yourusername)
- **Email**: your.email@university.ac.uk

---

**⭐ Star this repository if you find it useful!**
