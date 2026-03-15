# 1. REQUIREMENTS.MD

## Project Overview
Arduino proximity detection system with luminosity sensing, LCD display, and remote control capabilities.

## Feature Requirements

### Core Features
| Feature | Priority | Status | Dependencies |
|---------|----------|--------|--------------|
| Distance Detection | P0 | ✅ | Ultrasonic sensor |
| Proximity LED Warning | P0 | ✅ | Distance detection |
| System Shutdown on Close Proximity | P0 | ✅ | Distance detection, State machine |
| Luminosity-based Green LED | P1 | ✅ | Photoresistor |
| LCD Multi-screen Display | P1 | ✅ | LCD, State machine |
| IR Remote Control | P1 | ✅ | IR receiver |
| EEPROM Unit Persistence | P2 | ✅ | EEPROM library |

### Detailed Feature Specifications

#### 1. Distance Detection System
- **Trigger Distance**: < 10cm triggers shutdown
- **Warning Zone**: 10-30cm increases blink rate
- **Safe Zone**: > 30cm slow/no blinking
- **Units**: Switchable between cm and inches
- **Update Rate**: 50ms

#### 2. LED Indicators
- **Yellow LED**: Blink rate ∝ 1/distance (10-1000ms)
- **Red LED**: Active only in shutdown state
- **Green LED**: Brightness ∝ 1/luminosity (PWM controlled)

#### 3. LCD Display Screens
1. **Distance Screen**: Shows current distance + unit + status message
2. **Luminosity Screen**: Shows light level percentage + raw value
3. **System Reset Screen**: Confirmation prompt for system reset
4. **Shutdown Screen**: Warning message + unlock instructions

#### 4. Control Inputs
- **IR Remote**:
  - PLAY: Unlock from shutdown
  - UP/DOWN: Navigate LCD screens
  - POWER: Initiate system reset
  - 0-9: Change distance unit (0=cm, 1=inch)
- **Physical Button**: Debounced interrupt for emergency unlock

## Hardware Components

### Required Components
| Component | Quantity | Pins Used | Purpose |
|-----------|----------|-----------|---------|
| Arduino UNO | 1 | - | Main controller |
| HC-SR04 Ultrasonic | 1 | D2 (Echo), D3 (Trig) | Distance sensing |
| 16x2 LCD (I2C) | 1 | A4 (SDA), A5 (SCL) | Display |
| IR Receiver | 1 | D11 | Remote control |
| Yellow LED | 1 | D5 (PWM) | Proximity warning |
| Red LED | 1 | D6 (PWM) | Shutdown indicator |
| Green LED | 1 | D9 (PWM) | Luminosity indicator |
| Photoresistor | 1 | A0 | Light sensing |
| Potentiometer | 1 | A1 | Green LED max brightness |
| Push Button | 1 | D2 (INT0) | Manual unlock |
| Resistors | Various | - | Current limiting |

### Pin Allocations
```
Digital Pins:
D2  - Ultrasonic Echo (INT0 capable)
D3  - Ultrasonic Trigger
D5  - Yellow LED (PWM)
D6  - Red LED (PWM)
D9  - Green LED (PWM)
D11 - IR Receiver

Analog Pins:
A0 - Photoresistor
A1 - Potentiometer
A4 - LCD SDA (I2C)
A5 - LCD SCL (I2C)
```

## Non-Functional Requirements
- **Response Time**: < 100ms for user inputs
- **Memory Usage**: < 70% of available SRAM
- **Code Modularity**: Separate files for each subsystem
- **Power Efficiency**: Sleep modes when idle
- **Reliability**: Watchdog timer for crash recovery

---