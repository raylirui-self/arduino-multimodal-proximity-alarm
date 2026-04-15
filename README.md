# Arduino Multi-Modal Proximity Alarm System

> Note: This repo is part of my personal journey to learn Arduino and embedded systems.

An Arduino-based proximity alarm featuring ultrasonic sensing, multi-mode LCD display, IR remote control, and adaptive LED indicators.

## Project Structure
- **Phase 1**: Initial implementation based on course requirements
- **Phase 2**: Refactoring specs and architecture design

## Hardware
- Arduino UNO R3
- HC-SR04 ultrasonic sensor
- 16x2 LCD display
- IR remote controller
- LEDs (red, yellow, green), photoresistor, push button
- Resistors and potentiometer

## Features
- Real-time distance detection
- Multi-mode display: distance, luminosity, system status
- Adaptive LEDs: yellow blink rate tracks distance; green brightness inverse to ambient light
- Full IR remote control
- cm/inch toggle with EEPROM persistence
- Automatic safety lock when objects are too close

## Documentation
- [Phase 1 Notes](phase1-initial/notes.md)
- [Phase 2 Requirements](phase2-refactored/REQUIREMENTS.md)
- [Phase 2 Architecture](phase2-refactored/ARCHITECTURE.md)
- [Phase 2 Implementation](phase2-refactored/IMPLEMENTATION.md)
- [Phase 2 Testing](phase2-refactored/TESTING.md)
- [Hardware BOM](hardware/bom.csv)
- [Hardware Schematics](hardware/schematics.pdf)

## Reference
Based on the final project from "Arduino For Beginners - 2025 Complete Course" by Edouard Renard (Udemy).

## License
MIT - see [LICENSE](LICENSE).

## Connect
www.linkedin.com/in/rui-li-ray
