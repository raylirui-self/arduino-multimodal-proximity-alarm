# 3. IMPLEMENTATION.MD

## Code Structure

### File Organization
```
arduino_project/
├── arduino_project.ino    // Main setup() and loop()
├── config.h               // All constants and pin definitions
├── types.h                // Structs, enums, typedefs
├── sensors.h/.cpp         // Sensor reading functions
├── processing.h/.cpp      // Data processing logic
├── control.h/.cpp         // State machine and control
├── actuators.h/.cpp       // Output device control
├── timing.h/.cpp          // Non-blocking timer utilities
└── debug.h                // Serial debugging macros
```

### Main Loop Pattern
```cpp
void loop() {
    // 1. Read time-critical sensors (every cycle)
    if (shouldUpdate(distanceTimer, DISTANCE_INTERVAL)) {
        systemData.currentDistance = Sensors::readDistance();
        checkProximityTrigger();
    }
    
    // 2. Handle immediate inputs (every cycle)
    handleInterrupts();  // Button and IR
    
    // 3. Update time-based tasks
    if (shouldUpdate(luminosityTimer, LUMINOSITY_INTERVAL)) {
        updateLuminosity();
    }
    
    if (shouldUpdate(blinkTimer, currentBlinkRate)) {
        updateLEDBlink();
    }
    
    if (shouldUpdate(lcdTimer, LCD_INTERVAL)) {
        updateDisplay();
    }
    
    // 4. State machine update (every cycle)
    Control::updateSystemState();
}
```

### Key Implementation Patterns

#### 1. Non-blocking Timer Pattern
```cpp
bool shouldUpdate(uint32_t &lastTime, uint32_t interval) {
    uint32_t currentTime = millis();
    if (currentTime - lastTime >= interval) {
        lastTime = currentTime;
        return true;
    }
    return false;
}
```

#### 2. Debounced Button with Interrupt
```cpp
volatile bool buttonPressed = false;
volatile uint32_t lastButtonTime = 0;
const uint32_t DEBOUNCE_DELAY = 50;

void buttonISR() {
    uint32_t currentTime = millis();
    if (currentTime - lastButtonTime > DEBOUNCE_DELAY) {
        buttonPressed = true;
        lastButtonTime = currentTime;
    }
}

// In main loop
if (buttonPressed) {
    buttonPressed = false;  // Clear flag
    handleButtonPress();
}
```

#### 3. State Machine Pattern
```cpp
void updateSystemState() {
    SystemState nextState = currentState;
    
    switch(currentState) {
        case STATE_NORMAL:
            if (systemData.currentDistance < TRIGGER_DISTANCE) {
                nextState = STATE_SHUTDOWN;
                onEnterShutdown();
            }
            break;
            
        case STATE_SHUTDOWN:
            if (unlockRequested) {
                nextState = STATE_NORMAL;
                onExitShutdown();
            }
            break;
            
        case STATE_RESET_CONFIRM:
            if (confirmReceived) {
                performSystemReset();
                nextState = STATE_RESET_DONE;
            } else if (timeout()) {
                nextState = STATE_NORMAL;
            }
            break;
    }
    
    if (nextState != currentState) {
        onStateTransition(currentState, nextState);
        currentState = nextState;
    }
}
```

#### 4. LCD Screen Management
```cpp
enum LCDScreen {
    SCREEN_DISTANCE,
    SCREEN_LUMINOSITY,
    SCREEN_RESET,
    SCREEN_SHUTDOWN,
    SCREEN_COUNT
};

void updateLCD() {
    static LCDScreen lastScreen = SCREEN_COUNT;
    
    // Clear LCD only on screen change
    if (currentScreen != lastScreen) {
        lcd.clear();
        lastScreen = currentScreen;
    }
    
    switch(currentScreen) {
        case SCREEN_DISTANCE:
            displayDistance();
            break;
        case SCREEN_LUMINOSITY:
            displayLuminosity();
            break;
        // ...
    }
}

void displayDistance() {
    lcd.setCursor(0, 0);
    lcd.print("Dist: ");
    lcd.print(systemData.currentDistance, 1);
    lcd.print(systemData.distanceUnit == UNIT_CM ? "cm" : "in");
    
    lcd.setCursor(0, 1);
    if (systemData.currentDistance < 10) {
        lcd.print("TOO CLOSE!      ");
    } else if (systemData.currentDistance < 30) {
        lcd.print("Warning         ");
    } else {
        lcd.print("Safe            ");
    }
}
```

#### 5. LED Blink Without Delay
```cpp
void updateLEDBlink() {
    static bool ledState = false;
    
    if (systemState == STATE_SHUTDOWN) {
        // Toggle both red and yellow
        ledState = !ledState;
        digitalWrite(YELLOW_LED, ledState);
        digitalWrite(RED_LED, !ledState);
    } else if (systemState == STATE_NORMAL) {
        // Only yellow, rate based on distance
        ledState = !ledState;
        digitalWrite(YELLOW_LED, ledState);
        digitalWrite(RED_LED, LOW);
        
        // Update blink rate for next iteration
        currentBlinkRate = distanceToBlinkRate(systemData.currentDistance);
    }
}
```

#### 6. EEPROM Safe Write
```cpp
void saveUnitPreference(uint8_t unit) {
    uint8_t currentValue = EEPROM.read(EEPROM_UNIT_ADDR);
    if (currentValue != unit) {  // Only write if different
        EEPROM.write(EEPROM_UNIT_ADDR, unit);
    }
}
```

### Configuration Constants
```cpp
// config.h
#pragma once

// Pin Definitions
const uint8_t ULTRASONIC_ECHO = 2;
const uint8_t ULTRASONIC_TRIG = 3;
const uint8_t YELLOW_LED = 5;
const uint8_t RED_LED = 6;
const uint8_t GREEN_LED = 9;
const uint8_t IR_PIN = 11;
const uint8_t BUTTON_PIN = 2;  // INT0
const uint8_t PHOTORESISTOR = A0;
const uint8_t POTENTIOMETER = A1;

// Timing Constants (ms)
const uint32_t DISTANCE_INTERVAL = 50;
const uint32_t LUMINOSITY_INTERVAL = 100;
const uint32_t LCD_INTERVAL = 200;
const uint32_t MIN_BLINK_RATE = 1000;
const uint32_t MAX_BLINK_RATE = 100;

// Thresholds
const float TRIGGER_DISTANCE = 10.0;  // cm
const float WARNING_DISTANCE = 30.0;  // cm
const uint16_t LIGHT_THRESHOLD = 512;

// EEPROM Addresses
const uint8_t EEPROM_UNIT_ADDR = 0;
const uint8_t EEPROM_BRIGHTNESS_ADDR = 1;

// IR Remote Codes (example values - measure actual remote)
const uint32_t IR_PLAY = 0xFFA25D;
const uint32_t IR_UP = 0xFF629D;
const uint32_t IR_DOWN = 0xFFA857;
const uint32_t IR_POWER = 0xFF02FD;
const uint32_t IR_NUM_0 = 0xFF6897;
const uint32_t IR_NUM_1 = 0xFF30CF;
```

### Error Handling
```cpp
// Sensor timeout protection
float readDistanceWithTimeout() {
    uint32_t startTime = micros();
    float distance = Sensors::readDistance();
    
    // If reading took too long, return last known good value
    if (micros() - startTime > 30000) {  // 30ms timeout
        return systemData.currentDistance;  // Use last valid reading
    }
    
    // Sanity check
    if (distance < 0 || distance > 400) {  // Out of sensor range
        return systemData.currentDistance;  // Use last valid reading
    }
    
    return distance;
}
```

---