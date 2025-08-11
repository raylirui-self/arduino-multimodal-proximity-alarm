# Multi-Modal Proximity Alarm (Arduino) Notes

## Pain Points

1. unclear what functions I should have
2. unclear what functionality each function should have
3. unclear the the input and output for the functions and code blocks (the parallel blocks with separate time delays in loop())
4. unclear what should be put into the loop()
5. conflicting/overlapping thoughts/variables/functionality
6. lack of focus when working on a block of code. suddenly thought of or found out an error in another block of code, disrupting the thought process of the current code block and causing basic errors

## Questions to be Addressed

1. how to code these kinds of projects?
2. how can my current method be improved
3. how to list out and organize customer requirements (feature list)
4. how to translate customer requirements into technical requirements that I can use to guide me coding by modules

## Learnings

### 1. Function and Variable Order
need to put all my custom functions after all the variable initiation as the compiler needs to generate the variables first
- however, the order of the functions does not matter

### 2. Just-In-Time Coding
Just-In-Time coding: only initiate the variables when I need them. Avoid just brain dumping all variables that I remember that I would need, causing overwhelming sensation and confusion.

### 3. Debugging with Serial
Leverage `Serial.println()` to debug

### 4. Feature Scope Management
realized my tendency to gold plate the feature list (customer requirements)
- even though I was just thinking to practice something more to challenge myself
- I should document those thoughts separately as "extra challenges" and keep the original scope

### 5. Testing and Code Validation
just keep "playing around with" and "testing" your code
- you might come across some loop holes or unintended/unrealized errors in the code
- easier to fix and debug
  - if the code was working before adding another chunk in, then it is more likely issues with the new chunk

### 6. Simulation vs Real-World Differences
Experienced differences between simulation (TinkerCAD) and real-world implementation
- button & remote controller debounce
- obstructions to IR receiver, causing readings of 0s
- angle of ultrasonic sensor pointing downward too much

### 7. Data Type Byte Sizes

| **Data Type** | **Size (bytes)** | **Range (most Arduino boards)** | **Description** |
| --- | --- | --- | --- |
| **`boolean` (or `bool`)** | **1** | **`true` or `false`** | **Simple logical value.** |
| **`byte`** | **1** | **0 to 255** | **Unsigned 8-bit number.** |
| **`char`** | **1** | **-128 to 127** | **Signed 8-bit number, also used for single characters (ASCII values). [according to Seeed Studio](https://www.seeedstudio.com/blog/2020/05/29/getting-started-with-arduino-data-types/)** |
| **`unsigned char`** | **1** | **0 to 255** | **Unsigned 8-bit number, similar to `byte`. `byte` is recommended for clarity.** |
| **`int`** | **2 (4 on some boards)** | **-32,768 to 32,767 (or -2,147,483,648 to 2,147,483,647)** | **Primary data type for number storage. Size varies by board type (e.g., 2 bytes on Uno, 4 on Due).** |
| **`unsigned int`** | **2 (4 on some boards)** | **0 to 65,535 (or 0 to 4,294,967,295)** | **Same as `int` but only stores positive values. Size also varies by board type.** |
| **`word`** | **2 (4 on some boards)** | **0 to 65,535 (or 0 to 4,294,967,295)** | **On ATmega based boards, stores a 16-bit unsigned number. [On the Due and Zero, it stores a 32-bit unsigned number](https://learn.sparkfun.com/tutorials/data-types-in-arduino/all). `unsigned int` can be used interchangeably on many boards.** |
| **`long`** | **4** | **-2,147,483,648 to 2,147,483,647** | **Extended size variable for number storage.** |
| **`unsigned long`** | **4** | **0 to 4,294,967,295** | **Extended size for positive numbers. Often used with `millis()`.** |
| **`float`** | **4** | **Approximately -3.4028235E+38 to 3.4028235E+38** | **Stores floating-point numbers (numbers with decimal points). according to OYOclass.com. Calculations can be slower than integer operations.** |
| **`double`** | **4 (8 on Arduino Due)** | **Approximately -3.4028235E+38 to 3.4028235E+38 (more precision on Due)** | **Floating-point number type. Same precision as `float` on most boards, higher precision on the Arduino Due.** |

### 8. String vs Character Arrays
Prioritize `char[]` over `String`

**Reason:** On a microcontroller with limited RAM, repeated `String` concatenations lead to heap fragmentation, which will eventually cause random crashes.

Heap fragmentation is a common issue encountered in Arduino programming, especially when dealing with limited memory resources. It occurs when dynamic memory allocation and deallocation (using functions like `malloc()`, `calloc()`, `realloc()`, `free()`, or the `new` and `delete` operators) creates small, non-contiguous blocks of free memory within the heap.

**How it happens**
- **Dynamic memory allocation:** In Arduino, the heap is the area of RAM where dynamic memory allocation takes place. When you request a block of memory (e.g., for a `String` object, or using `malloc`), it's taken from the heap.
- **Deallocation creates "holes":** When you free a block of memory, a "hole" or unused space is created in the heap.
- **Varying sizes exacerbate fragmentation:** If you repeatedly allocate and deallocate blocks of different sizes, these "holes" become scattered and irregular, making it difficult for the memory allocator to find contiguous blocks large enough to satisfy new requests, even if the total available memory is sufficient.

**Why it's a problem**
- **Memory allocation failures:** Even with ample free memory, a program might fail to allocate a required block if there's no single contiguous chunk large enough, leading to crashes or unexpected behavior.
- **Degraded performance:** The memory allocator takes longer to find suitable free blocks in a fragmented heap, increasing CPU workload and slowing down program execution.
- **Unreliable programs:** Programs become unreliable if they cannot consistently allocate the memory they need, resulting in inconsistent operation.

**Strategies to reduce heap fragmentation in Arduino**

1. **Avoid dynamic memory allocation where possible:**
   - Favor using static or global variables instead of dynamic ones, as they reside in non-fragmented memory areas (static and global memory sections, respectively).
   - If possible, use character arrays (`char[]`) instead of `String` objects, especially for managing text data, as `String` objects can contribute to fragmentation due to their dynamic resizing behavior.
2. **Short object lifetimes:**
   - Objects with short lifetimes (e.g., local variables within a function) are less likely to cause fragmentation as they are allocated and deallocated quickly, returning the memory to the heap for reuse.
   - Long-lived objects (especially if their sizes fluctuate) contribute more significantly to fragmentation, reserving large blocks of memory for extended periods.
3. **Constant allocation:**
   - If you must use dynamic memory, try to ensure that objects of the same type are allocated with the same size consistently.
   - For `String` objects, pre-allocate sufficient memory using `reserve()` to prevent repeated reallocations, which can lead to fragmentation. For example, `myString.reserve(100);` reserves 100 characters for the string.

### 9. Understanding `const` with Pointers vs Simple Types
1. **Simple types** (`const byte PIN = 10`): `const` makes the value unchangeable - straightforward!
2. **Pointers** (`const char*`): The position of `const` matters:
   - `const char* str` = pointer TO const data (can change where it points, can't modify the data)
   - `char* const str` = CONST pointer (can't change where it points, can modify the data)
   - `const char* const str` = both pointer AND data are const
3. **Memory tip**: Read declarations right-to-left:
   - `const char*` → pointer to char const → "pointer that points to constant characters"
4. **Practical use**:
   - Pin assignments: Use `const byte` (simple type) ✅
   - String literals: Use `const char*` (lets you switch between "cm"/"in" without copying) ✅
     - more memory-efficient since it just points to existing string literals rather than copying them!
   - LCD buffers: Use `char array[size]` (fixed memory, no fragmentation) ✅
5. **Key insight**: String literals ("hello") live in read-only memory, so `const char*` matches their nature and prevents crashes from modification attempts

### 10. EEPROM Write Operations
Prioritize eeprom.update() over eeprom.write()

1. **Reason:** EEPROM HW has limited write cycles (around 100,000 per address)
2. **Purpose:** to decrease the number of times we write to EEPROM
3. **Exception:**
   - **Initial Writes:** When initializing EEPROM with known values for the first time, `write()` might be used, as the values are guaranteed to be different from the default (often 0xFF) state.

## Mistakes

### 1. LCD Print: Did Not Add "    " to Clear Out Trailing Prints When Contents Shrink

**Impact:** misleading & wrong value display on LCD

**Example:** when displaying luminosity, the lower end displayed 549 instead of 54 because the 9 was not cleared

### 2. Incorrect Time Delay Setup: Putting the lastTimeTrigger Outside the if (timeNow - lastTimeTrigger > delay) {}

**Impact:** the abnormal LED blinking

```arduino
//Wrong
if (timeNow - lastTimeSystemLockLedTrigger > systemLockLedDelay) {
    blinkRedYellowLed();
}
lastTimeSystemLockLedTrigger += systemLockLedDelay;

//Corrected
if (timeNow - lastTimeSystemLockLedTrigger > systemLockLedDelay) {
    blinkRedYellowLed();
    lastTimeSystemLockLedTrigger += systemLockLedDelay;
}
```

### 3. Preallocate Values to Variables That Will Be Assigned Value in the loop() Based on Measurements

The values are also used for judgements

```arduino
unsigned long ultrasonicEchoDuration = 0;
double ultrasonicEchoDistanceCm = 0;
double ultrasonicEchoDistanceIn = 0;
```

**Impact:** causing the system to lock immediately before the ultrasonic sensor (set to sense every 100ms) is able to retrieve values

**Learnings:**
- for variables that will be assigned with values based on inputs (sensor, user…), do not initiate the global variable with a value
  - unless I need a default state

**Question:** what happens during the time that the variables are empty?

When uninitialized global variables are declared without values in Arduino:
- Numeric types (`int`, `long`, `double`, etc.) are initialized to 0
- Pointers are initialized to NULL
- This is why your ultrasonic distance started at 0, causing the minimum LED delay (very fast blinking)
- Best practice: Always initialize with sensible defaults for your application (e.g., `double ultrasonicEchoDistanceCm = 200.0;` for "no obstacle detected")

### 4. Type Mismatch & Casting

When working with `byte` type stored in EEPROM & `enum` (systemUnits; a custom data type), tried to assigned EEPROM.read() directly to my custom data Type

**Impact:**
- Error: `invalid header file`

```arduino
enum systemUnits {CM, IN};
void setup() {
    systemUnit = EEPROM.read(EEPROM_ADDRESS); //ERROR LINE: assigning byte to systemUnits type
    if (systemUnit == 255) {
        systemUnit = CM;
    }
    else {
        systemUnit = (systemUnits)(systemUnit); // this is actually meaningless
    }
}
```

**Fix:** assigned EEPROM.read() to a byte first, then convert

```arduino
enum systemUnits {CM, IN};
void setup() {
    byte eepromRead = EEPROM.read(EEPROM_ADDRESS); // FIXED
    if (systemUnit == 255) {
        systemUnit = CM;
    }
    else {
        systemUnit = (systemUnits)(eepromRead); // byte -> systemUnits type
    }
}
```

### 5. Timing Accumulation Error: Using `+=` Instead of `=` for Timer Updates

**Impact:** Yellow LED blinking extremely fast on real hardware (but not in simulation)

**Root cause:** Using `+=` accumulates timing drift when loop execution varies

**Example:**

```cpp
// Wrong - accumulates timing drift
if (timeNow - lastTimeSystemOnLedTrigger > systemOnLedDelay) {
    blinkYellowLed();
    lastTimeSystemOnLedTrigger += systemOnLedDelay;  // WRONG: adds delay to old value
}

// Corrected - resets to current time
if (timeNow - lastTimeSystemOnLedTrigger > systemOnLedDelay) {
    blinkYellowLed();
    lastTimeSystemOnLedTrigger = timeNow;  // CORRECT: sets to current time
}
```

**Why this happens:**
- With `+=`: If loop takes 150ms but delay is 100ms, next trigger time is already 50ms in the past
- This causes immediate re-triggering, creating rapid blinking
- Simulation doesn't show this because it runs in ideal conditions

**Learnings:**
1. Use `= timeNow` for non-critical timing that should reset each trigger
2. Only use `+=` when you need precise intervals (e.g., music/tone generation)
3. Real hardware has variable loop execution time due to:
   - Interrupt processing
   - Serial communication
   - Sensor reading delays
   - Power fluctuations
4. Always test on real hardware - simulations don't show timing drift issues

### 6. Omitted Lower and Upper Bounds for Echo Duration for Ultrasonic Sensor

**Impact:**
- returns 805cm reading when no object is in view at all
- Briefly incorrect distance displays before the lock engages
- Potential edge cases with the distance calculation

**Fix:** using `constrain(varName, minValue, maxValue)` to give lower and upper bounds for echo duration

### 7. Value Corruption for 4-byte Data `unsigned long`

**Cause:** `unsigned long` is a 4-byte value, requiring multiple CPU instructions to read through byte by byte

**Impact:** `interrupt()` changing the value mid-air while reading

```cpp
// Let's say ultrasonicEchoBegin currently = 0x12345678

// Main loop starts reading the value:
1. Read byte 1: 0x12
2. Read byte 2: 0x34
// INTERRUPT OCCURS HERE! ISR changes ultrasonicEchoBegin to 0xABCDEF00
3. Read byte 3: 0xEF  // This is from the NEW value!
4. Read byte 4: 0x00  // This is from the NEW value!

// Result: We read 0x1234EF00 - a corrupted mix of old and new values!
```

**Fix:** disabling `interrupt()` during the reading process

```cpp
noInterrupts();  // Disable interrupts temporarily
unsigned long echoBegin = ultrasonicEchoBegin;  // Copy takes ~4 instructions
unsigned long echoEnd = ultrasonicEchoEnd;      // Copy takes ~4 instructions  
interrupts();    // Re-enable interrupts (only disabled for ~8 CPU cycles)
```