# 2. ARCHITECTURE.MD

## System Architecture Overview

### State Machine Design
```
┌─────────┐  Distance<10cm   ┌──────────┐
│ NORMAL  │─────────────────>│ SHUTDOWN │
└────┬────┘                  └────┬─────┘
     │                             │
     │ Power Btn                   │ Play or Btn
     v                             │
┌─────────┐                        │
│  RESET  │<───────────────────────┘
│ CONFIRM │
└─────────┘
```

### System States
| State | Description | Active Components | Transitions |
|-------|-------------|-------------------|-------------|
| STATE_NORMAL | Normal operation | All sensors, Yellow/Green LEDs | -> SHUTDOWN on proximity |
| STATE_SHUTDOWN | Safety lockout | Red LED, LCD warning | -> NORMAL on unlock |
| STATE_RESET_CONFIRM | Awaiting reset | LCD prompt | -> NORMAL on confirm |
| STATE_RESET_DONE | Reset complete | LCD message | -> NORMAL after 2s |

## Function Architecture

### Layer 1: Hardware Abstraction
```cpp
// sensors.h - Pure sensor reading, no logic
namespace Sensors {
    float readDistanceRaw();        // Returns: pulse duration (μs)
    uint16_t readLuminosityRaw();   // Returns: ADC value (0-1023)
    uint16_t readPotentiometer();   // Returns: ADC value (0-1023)
    bool readButton();               // Returns: debounced state
    uint32_t readIRSignal();        // Returns: IR code or 0
}
```

### Layer 2: Data Processing
```cpp
// processing.h - Convert raw data to useful values
namespace Processing {
    float pulseToDistance(float pulse, bool metric);  // μs -> cm/in
    uint8_t distanceToBlinkRate(float distance);      // cm -> ms delay
    uint8_t luminosityToLEDBrightness(uint16_t lux, uint16_t maxVal);
    SystemState evaluateSystemState(float distance, SystemState current);
}
```

### Layer 3: Control Logic
```cpp
// control.h - Decision making and state management
namespace Control {
    void updateSystemState();        // Main state machine update
    void handleIRCommand(uint32_t cmd);
    void performEmergencyStop();
    void unlockSystem();
    bool confirmReset();
}
```

### Layer 4: Output Management
```cpp
// actuators.h - Hardware output control
namespace Actuators {
    void setLEDState(LED led, uint8_t brightness, uint16_t blinkMs);
    void updateLCDScreen(ScreenType screen, float value1, float value2);
    void buzzAlert(uint16_t frequency, uint16_t duration);
    void saveToEEPROM(uint8_t address, uint8_t value);
}
```

## Data Flow Architecture

### Main Data Flows
```
1. Distance Flow:
   Ultrasonic -> Raw Pulse -> Distance Calc -> State Check -> LED Control
                                            └-> LCD Update

2. Luminosity Flow:
   Photoresistor -> ADC Value -> Brightness Calc -> Green LED PWM
                              └-> LCD Update (if screen active)

3. Control Flow:
   IR/Button -> Command Parse -> State Machine -> System Response
                               └-> EEPROM Save (if settings change)
```

### Timing Architecture

| Task | Interval | Timer Variable | Priority | Function |
|------|----------|----------------|----------|----------|
| Distance Sensing | 50ms | distanceTimer | HIGH | updateDistance() |
| LED Blink Update | Variable | blinkTimer | HIGH | updateLEDBlink() |
| Luminosity Read | 100ms | luminosityTimer | MEDIUM | updateLuminosity() |
| LCD Refresh | 200ms | lcdTimer | LOW | updateLCD() |
| IR Check | 10ms | irTimer | HIGH | checkIRInput() |
| EEPROM Save | On change | - | LOW | saveSettings() |

### Memory Management
```cpp
// Global State Structure (minimize memory fragmentation)
struct SystemData {
    // Sensor Data (8 bytes)
    float currentDistance;      // 4 bytes
    uint16_t currentLuminosity; // 2 bytes
    uint16_t potValue;          // 2 bytes
    
    // System State (4 bytes)
    uint8_t systemState;        // 1 byte
    uint8_t lcdScreen;          // 1 byte
    uint8_t distanceUnit;       // 1 byte
    uint8_t ledStates;          // 1 byte (bit flags)
    
    // Timing (16 bytes)
    uint32_t distanceTimer;     // 4 bytes
    uint32_t luminosityTimer;   // 4 bytes
    uint32_t lcdTimer;          // 4 bytes
    uint32_t blinkTimer;        // 4 bytes
    
    // Settings (2 bytes)
    uint8_t maxBrightness;      // 1 byte
    uint8_t triggerDistance;    // 1 byte
}; // Total: 30 bytes
```

## Module Dependencies

```
┌─────────────┐
│    main     │
└──────┬──────┘
       │ uses
┌──────▼──────┐
│   control   │
└──────┬──────┘
       │ uses
┌──────▼──────────────┐
│  processing         │
└──────┬──────────────┘
       │ uses
┌──────▼──────┐  ┌────────────┐
│   sensors   │  │  actuators │
└─────────────┘  └────────────┘
       │              │
┌──────▼──────────────▼─────────┐
│        Hardware Layer         │
└───────────────────────────────┘
```

---