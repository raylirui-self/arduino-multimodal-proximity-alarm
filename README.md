# Arduino Multi-Modal Proximity Alarm System

A comprehensive Arduino-based proximity detection and alarm system featuring ultrasonic sensing, multi-mode LCD display, IR remote control, and adaptive LED indicators.

## 🎯 Project Overview
This project demonstrates the integration of multiple sensors and actuators in an embedded system, showcasing skills in:
- Embedded C/C++ programming
- Hardware-software integration
- Interrupt-driven programming
- Non-blocking code design
- EEPROM data persistence

## 📁 Project Structure
- **Phase 1**: Initial implementation based on course requirements
- **Phase 2**: Refactoring specs and architecture design

## 🛠️ Hardware Components
- Arduino UNO R3
- HC-SR04 Ultrasonic sensor
- 16x2 LCD Display
- IR Remote Controller
- LEDs (Red, Yellow, Green)
- Photoresistor
- Push button
- Various resistors and potentiometer

## 🚀 Features
- **Distance Detection**: Real-time object proximity monitoring
- **Multi-Mode Display**: Toggle between distance, luminosity, and system status
- **Adaptive LED Indicators**: 
  - Yellow LED blink rate proportional to distance
  - Green LED brightness inverse to ambient light
  - Red/Yellow combination for system lock
- **Remote Control**: Full system control via IR remote
- **Unit Conversion**: Toggle between cm/inches with EEPROM persistence
- **Safety Lock**: Automatic system lock when objects too close

## 📚 Learning Journey
This repository documents my progression from Arduino beginner to implementing professional embedded systems practices.

### Skills Demonstrated
- Interrupt Service Routines (ISR)
- Non-blocking timing with millis()
- EEPROM data persistence
- Debouncing techniques
- Memory optimization
- Modular programming

## 📖 Documentation
- [Phase 1 Notes](phase1-initial/notes.md) - Initial learnings and mistakes
- [Phase 2 Requirements](phase2-refactored/REQUIREMENTS.md) - Feature and hardware specs
- [Phase 2 Architecture](phase2-refactored/ARCHITECTURE.md) - System design and patterns
- [Phase 2 Implementation](phase2-refactored/IMPLEMENTATION.md) - Code structure and patterns
- [Phase 2 Testing](phase2-refactored/TESTING.md) - Test plan and debugging
- [Hardware BOM](hardware/bom.csv) - Bill of materials
- [Hardware Schematics](hardware/schematics.pdf) - Circuit schematics

## 🔗 Course Reference
Based on concepts and Final project requirements from "Arduino For Beginners - 2025 Complete Course" by Edouard Renard (Udemy)

## 📝 License
MIT License - See [LICENSE](LICENSE) file

## 🤝 Connect
www.linkedin.com/in/rui-li-ray