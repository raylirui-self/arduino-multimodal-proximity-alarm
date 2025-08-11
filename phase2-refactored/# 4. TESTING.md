# 4. TESTING.MD

## Test Plan Overview

### Testing Phases
1. **Unit Testing** - Individual function validation
2. **Integration Testing** - Module interaction testing
3. **System Testing** - Full system validation
4. **Stress Testing** - Edge cases and limits

## Unit Tests

### Sensor Tests

#### TEST_001: Ultrasonic Sensor Reading
```cpp
void test_ultrasonicSensor() {
    Serial.println("TEST_001: Ultrasonic Sensor");
    
    // Test at multiple distances
    float testDistances[] = {5, 10, 20, 50, 100, 200};
    
    for (int i = 0; i < 6; i++) {
        Serial.print("Place object at ");
        Serial.print(testDistances[i]);
        Serial.println("cm. Press any key...");
        
        while(!Serial.available()) {}
        Serial.read();
        
        float reading = Sensors::readDistance();
        float error = abs(reading - testDistances[i]);
        
        Serial.print("Expected: ");
        Serial.print(testDistances[i]);
        Serial.print("cm, Got: ");
        Serial.print(reading);
        Serial.print("cm, Error: ");
        Serial.print(error);
        Serial.println("cm");
        
        // Pass/Fail (±2cm tolerance)
        if (error <= 2.0) {
            Serial.println("PASS");
        } else {
            Serial.println("FAIL");
        }
    }
}
```

#### TEST_002: Photoresistor Range
```cpp
void test_photoresistor() {
    Serial.println("TEST_002: Photoresistor Range");
    
    Serial.println("Cover sensor completely...");
    delay(2000);
    uint16_t darkReading = analogRead(PHOTORESISTOR);
    
    Serial.println("Shine bright light on sensor...");
    delay(2000);
    uint16_t brightReading = analogRead(PHOTORESISTOR);
    
    Serial.print("Dark: ");
    Serial.print(darkReading);
    Serial.print(", Bright: ");
    Serial.println(brightReading);
    
    if (darkReading < 100 && brightReading > 900) {
        Serial.println("PASS - Good range");
    } else {
        Serial.println("FAIL - Adjust resistor value");
    }
}
```

### Processing Tests

#### TEST_003: Distance to Blink Rate Conversion
```cpp
void test_blinkRateCalculation() {
    Serial.println("TEST_003: Blink Rate Calculation");
    
    struct TestCase {
        float distance;
        uint16_t expectedRate;
    };
    
    TestCase cases[] = {
        {5.0, 100},    // Very close - fast
        {15.0, 300},   // Warning zone
        {30.0, 600},   // Edge of warning
        {50.0, 1000},  // Safe - slow
        {100.0, 1000}  // Far - max slow
    };
    
    for (int i = 0; i < 5; i++) {
        uint16_t rate = Processing::distanceToBlinkRate(cases[i].distance);
        Serial.print("Distance: ");
        Serial.print(cases[i].distance);
        Serial.print("cm -> Rate: ");
        Serial.print(rate);
        Serial.print("ms (expected: ");
        Serial.print(cases[i].expectedRate);
        Serial.println("ms)");
        
        if (abs(rate - cases[i].expectedRate) < 50) {
            Serial.println("PASS");
        } else {
            Serial.println("FAIL");
        }
    }
}
```

## Integration Tests

### TEST_004: State Machine Transitions
```cpp
void test_stateMachine() {
    Serial.println("TEST_004: State Machine");
    
    // Test Normal -> Shutdown
    systemData.currentState = STATE_NORMAL;
    systemData.currentDistance = 5.0;  // Below trigger
    Control::updateSystemState();
    
    if (systemData.currentState == STATE_SHUTDOWN) {
        Serial.println("PASS - Normal to Shutdown");
    } else {
        Serial.println("FAIL - Should enter shutdown");
    }
    
    // Test Shutdown -> Normal (unlock)
    Control::unlockSystem();
    Control::updateSystemState();
    
    if (systemData.currentState == STATE_NORMAL) {
        Serial.println("PASS - Shutdown to Normal");
    } else {
        Serial.println("FAIL - Should unlock");
    }
}
```

### TEST_005: LCD Screen Navigation
```cpp
void test_lcdNavigation() {
    Serial.println("TEST_005: LCD Navigation");
    
    currentScreen = SCREEN_DISTANCE;
    
    // Simulate UP button
    handleIRCommand(IR_UP);
    if (currentScreen == SCREEN_LUMINOSITY) {
        Serial.println("PASS - UP navigation");
    } else {
        Serial.println("FAIL - UP navigation");
    }
    
    // Test wraparound
    currentScreen = SCREEN_SHUTDOWN;
    handleIRCommand(IR_DOWN);
    if (currentScreen == SCREEN_DISTANCE) {
        Serial.println("PASS - Screen wraparound");
    } else {
        Serial.println("FAIL - Screen wraparound");
    }
}
```

## System Tests

### TEST_006: Full Proximity Response
```cpp
void test_fullProximityResponse() {
    Serial.println("TEST_006: Full Proximity Response");
    Serial.println("Move object from 50cm to 5cm slowly...");
    
    uint32_t testStart = millis();
    bool shutdownTriggered = false;
    
    while(millis() - testStart < 10000) {  // 10 second test
        loop();  // Run main loop
        
        if (systemData.currentState == STATE_SHUTDOWN && !shutdownTriggered) {
            Serial.print("Shutdown triggered at distance: ");
            Serial.println(systemData.currentDistance);
            shutdownTriggered = true;
            
            // Check if LEDs are blinking
            delay(500);
            bool yellowState1 = digitalRead(YELLOW_LED);
            delay(500);
            bool yellowState2 = digitalRead(YELLOW_LED);
            
            if (yellowState1 != yellowState2) {
                Serial.println("PASS - LEDs blinking in shutdown");
            } else {
                Serial.println("FAIL - LEDs should blink");
            }
            break;
        }
    }
    
    if (!shutdownTriggered) {
        Serial.println("FAIL - Shutdown not triggered");
    }
}
```

## Stress Tests

### TEST_007: Timing Accuracy Under Load
```cpp
void test_timingAccuracy() {
    Serial.println("TEST_007: Timing Accuracy");
    
    uint32_t intervals[] = {50, 100, 200};
    const char* names[] = {"Distance", "Luminosity", "LCD"};
    
    for (int i = 0; i < 3; i++) {
        uint32_t expectedInterval = intervals[i];
        uint32_t lastTime = millis();
        uint32_t count = 0;
        uint32_t totalError = 0;
        
        while(count < 20) {  // 20 samples
            if (shouldUpdate(lastTime, expectedInterval)) {
                uint32_t actualInterval = millis() - lastTime;
                totalError += abs(actualInterval - expectedInterval);
                count++;
            }
            
            // Simulate other work
            delay(random(5, 15));
        }
        
        float avgError = totalError / 20.0;
        Serial.print(names[i]);
        Serial.print(" - Avg error: ");
        Serial.print(avgError);
        Serial.println("ms");
        
        if (avgError < 5) {
            Serial.println("PASS");
        } else {
            Serial.println("FAIL - Timing drift too high");
        }
    }
}
```

### TEST_008: Memory Stability
```cpp
void test_memoryStability() {
    Serial.println("TEST_008: Memory Stability");
    
    int startMem = freeMemory();  // Requires MemoryFree library
    
    // Run system for 1000 cycles
    for (int i = 0; i < 1000; i++) {
        loop();
        delay(10);
        
        if (i % 100 == 0) {
            int currentMem = freeMemory();
            Serial.print("Cycle ");
            Serial.print(i);
            Serial.print(" - Free memory: ");
            Serial.println(currentMem);
            
            if (currentMem < startMem - 100) {
                Serial.println("FAIL - Memory leak detected");
                return;
            }
        }
    }
    
    Serial.println("PASS - Memory stable");
}
```

## Test Execution Checklist

### Pre-Test Setup
- [ ] Fresh Arduino upload
- [ ] Serial Monitor at 9600 baud
- [ ] All components connected
- [ ] EEPROM cleared
- [ ] Test environment prepared (objects, light sources)

### Test Sequence
1. [ ] Run unit tests (TEST_001 - TEST_003)
2. [ ] Run integration tests (TEST_004 - TEST_005)
3. [ ] Run system tests (TEST_006)
4. [ ] Run stress tests (TEST_007 - TEST_008)
5. [ ] Document any failures
6. [ ] Retest after fixes

### Acceptance Criteria
- All unit tests: 100% pass
- Integration tests: 100% pass
- System tests: 100% pass
- Stress tests: ≥80% pass
- No memory leaks
- Response time < 100ms for all inputs

## Debugging Helpers

### Serial Debug Macro
```cpp
#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif
```

### State Visualizer
```cpp
void printSystemState() {
    Serial.print("[STATE:");
    switch(systemData.currentState) {
        case STATE_NORMAL: Serial.print("NORMAL"); break;
        case STATE_SHUTDOWN: Serial.print("SHUTDOWN"); break;
        case STATE_RESET_CONFIRM: Serial.print("RESET_CONF"); break;
    }
    Serial.print("] [DIST:");
    Serial.print(systemData.currentDistance);
    Serial.print("cm] [LUX:");
    Serial.print(systemData.currentLuminosity);
    Serial.print("] [SCREEN:");
    Serial.print(currentScreen);
    Serial.println("]");
}
```

---

## How to Use This Documentation

### Before Coding
1. Review **requirements.md** to understand what to build
2. Study **architecture.md** to understand system design
3. Reference **implementation.md** for coding patterns

### During Coding
1. Follow the module structure in **architecture.md**
2. Use code patterns from **implementation.md**
3. Keep **config.h** updated with all constants
4. Run relevant tests from **testing.md** after each module

### After Coding
1. Run full test suite from **testing.md**
2. Update documentation if design changes
3. Document any new patterns discovered

### Quick Reference Cards

#### Function Template
```cpp
/**
 * Brief description
 * @param param1 Description [units]
 * @param param2 Description [units]
 * @return Description [units]
 * @timing Called every Xms
 * @sideEffects Modifies: var1, var2
 */
returnType functionName(type1 param1, type2 param2) {
    // Implementation
}
```

#### Adding New Feature Checklist
- [ ] Update state machine if needed
- [ ] Define new constants in config.h
- [ ] Create/update functions in appropriate module
- [ ] Add timer if time-based
- [ ] Update main loop if needed
- [ ] Create unit test
- [ ] Update integration tests
- [ ] Document in requirements.md