// System
#include <LiquidCrystal.h>
#include <IRremote.h>
#include <EEPROM.h>

enum systemUnits {CM, IN};
int NUMBER_OF_ON_SYSTEM = 3;
enum systemModes {LCD_DISTANCE, LCD_LUMINOSITY, LCD_RESET, LCD_LOCK};
systemUnits systemUnit;
systemModes systemMode = LCD_DISTANCE;

// LEDs
const byte LED_GREEN_PIN = 10; //PWM

const byte LED_YELLOW_PIN = 11; //PWM
const byte LED_RED_PIN = 12;
const byte ledCount = 2;
const byte LED_PINS[ledCount] = {LED_YELLOW_PIN, LED_RED_PIN};

bool systemOnLedIsOn = false;
bool systemLockLedIsOn = false;

unsigned long lastTimeSystemOnLedTrigger = 0;
unsigned long lastTimeSystemLockLedTrigger = 0;

unsigned long systemOnLedDelay;
unsigned long systemOnledDelayRange[2] = {10, 2000};

unsigned long systemLockLedDelay = 500;

// LCD
const byte LCD_RS_PIN = A5;
const byte LCD_E_PIN = A4;
const byte LCD_D4_PIN = 6;
const byte LCD_D5_PIN = 7;
const byte LCD_D6_PIN = 8;
const byte LCD_D7_PIN = 9;
String distanceMessage = "";

LiquidCrystal lcd(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

// IR controller V3
const byte IR_RECEIVE_PIN = 5;
//TinkerCAD values
// const byte IR_BUTTON_OFF = 0;
// const byte IR_BUTTON_FUNCSTOP = 2;
// const byte IR_BUTTON_PLAY = 5;
// const byte IR_BUTTON_DOWN = 8;
// const byte IR_BUTTON_UP = 10;
// const byte IR_BUTTON_UNIT_TOGGLE = 9;
//Actual kit values
const byte IR_BUTTON_OFF = 69;
const byte IR_BUTTON_FUNCSTOP = 71;
const byte IR_BUTTON_PLAY = 64;
const byte IR_BUTTON_DOWN = 7;
const byte IR_BUTTON_UP = 9;
const byte IR_BUTTON_UNIT_TOGGLE = 25;
unsigned long lastTimeIRPressed = 0;
unsigned long irDebounce = 20;

// Ultrasonic sensor
const byte ULTRASONIC_TRIGGER_PIN = 4;
const byte ULTRASONIC_ECHO_PIN = 3; //Interrupt pin
const long ultrasonicTriggerDelay = 100;
unsigned long lastTimeUltrasonicTrigger = 0;
const unsigned long ECHO_MAX = 25000; //us ~431cm minimum
const unsigned long ECHO_MIN = 116; // ~2cm minimum
volatile unsigned long ultrasonicEchoBegin;
volatile unsigned long ultrasonicEchoEnd ;
volatile bool newTimeAvailable = false;
unsigned long ultrasonicEchoDuration;
double ultrasonicEchoDistanceCm;
double ultrasonicEchoDistanceIn;
double ultrasonicDistanceRange[2] = {2.0, 400.0}; //HC-SR04 specs
double lockZoneDistanceCm = 5;
double warningZoneDistanceCm = 30;
bool initialEchoReading = false;

// EEPROM
const byte EEPROM_ADDRESS = 0;

// Photoresistor
const byte 	PHOTORESISTOR_PIN = A3;

int luminosityValue;
unsigned long luminosityReadDelay = 100;
unsigned long lastTimeLuminosityRead = 0;
int maxLuminosity = 1023;
int minLuminosity = 0;

// Button
const byte BUTTON_PIN = 2; //Interrupt pin
volatile bool buttonIsPressed = false;
unsigned long lastTimeButtonPress = 0;
unsigned long buttonPressDebounce = 100; //ms

// #functions

// ##setups
void setUpLeds() {
	for (int i = 0; i < ledCount; i++) {
		pinMode(LED_PINS[i], OUTPUT);
	}
	pinMode(LED_GREEN_PIN, OUTPUT);
}

void setUpPhotoresistor() {
	pinMode(PHOTORESISTOR_PIN, INPUT);
	luminosityValue = analogRead(PHOTORESISTOR_PIN);
}

void setUpUltrasonic() {
	pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
	pinMode(ULTRASONIC_ECHO_PIN, INPUT);
	attachInterrupt(digitalPinToInterrupt(ULTRASONIC_ECHO_PIN), ultrasonicEchoInterrupt, CHANGE);
}

void setUpButton() {
	pinMode(BUTTON_PIN, INPUT);
	attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, RISING);
}

// ##functionalities
// ### systems
void nextMode() {
    systemMode = (systemModes) ((systemMode + 1) % NUMBER_OF_ON_SYSTEM);
}

void prevMode() {
    systemMode = (systemModes) ((systemMode - 1 + NUMBER_OF_ON_SYSTEM) % NUMBER_OF_ON_SYSTEM);
}

// button interrupt
void buttonInterrupt() {
	unsigned long timeNow = millis();
	if (timeNow - lastTimeButtonPress > buttonPressDebounce) {
		buttonIsPressed = true;
		lastTimeButtonPress = timeNow;
	}
}

// ### LED module
void blinkYellowLed() {
	digitalWrite(LED_RED_PIN, LOW);
	systemLockLedIsOn = false;
	if (systemOnLedIsOn) {
		systemOnLedIsOn = false;
		digitalWrite(LED_YELLOW_PIN, LOW);
	}
	else {
		systemOnLedIsOn = true;
		digitalWrite(LED_YELLOW_PIN, HIGH);
	}
}

void blinkRedYellowLed() {
	systemOnLedIsOn = false;
	if (systemLockLedIsOn) {
		systemLockLedIsOn = false;
		for (int i = 0; i < ledCount; i++) {
			digitalWrite(LED_PINS[i], LOW);
		}
	}
	else {
		systemLockLedIsOn = true;
		for (int i = 0; i < ledCount; i++) {
			digitalWrite(LED_PINS[i], HIGH);
		}	
	}
}

// ### Ultrasonic sensor module
void ultrasonicSendTrigger() { //send a 10us trigger pulse
	digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);
	delayMicroseconds(2);
	digitalWrite(ULTRASONIC_TRIGGER_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);
	// Serial.println("Ultrasonic trigger sent!");
}

void ultrasonicEchoInterrupt() { // interrupt function: return echo duration
	if (digitalRead(ULTRASONIC_ECHO_PIN) == HIGH) {
		ultrasonicEchoBegin = micros();
	}
	else {
		ultrasonicEchoEnd = micros();
		newTimeAvailable = true;
		// Serial.println("read value");
	}
}

// ### LCD module
void lcdControlCenter() { // control center given the screen Mode
	switch (systemMode) {
		case LCD_DISTANCE: {
			lcdPrintAllDistance();
			break;
		}
		case LCD_LUMINOSITY: {
			lcdPrintLuminosity();
			break;
		}
		case LCD_RESET: {
			lcdPrintSystemReset();
			break;
		}
		case LCD_LOCK: {
			lcdPrintSystemLock();
			break;
		}
	}
}

void lcdPrintSystemLock() {
	lcd.setCursor(0, 0);
	lcd.print("!!! Obstacle !!!      ");
	lcd.setCursor(0, 1);
	lcd.print("Press to unlock.     ");
}

void lcdPrintLuminosity() {
	lcd.setCursor(0, 0);
	lcd.print("Luminosity: ");
	lcd.print(luminosityValue);
	lcd.print("      ");
	lcd.setCursor(0, 1);
	lcd.print("                           ");
}

void lcdPrintSystemReset() {
	lcd.setCursor(0, 0);
	lcd.print("Press on OFF to   ");
	lcd.setCursor(0, 1);
	lcd.print("reset settings.   ");
}

void lcdPrintAllDistance() {
	// Serial.println(ultrasonicEchoDistanceCm);

	if (ultrasonicEchoDistanceCm < warningZoneDistanceCm) {
		lcdPrintDistance();
		lcd.setCursor(0, 1);
		lcd.print("!! Warning !!   ");
	}
	else {
		lcdPrintDistance();
		lcd.setCursor(0, 1);
		lcd.print("No obstacle.         ");
	}
}

void lcdPrintDistance() {
	// Serial.println(ultrasonicEchoDistanceCm);
	
	double distance = ultrasonicEchoDistanceCm;
	String distanceUnit = " cm    ";
	if (systemUnit == IN) {
		distance = ultrasonicEchoDistanceIn;
		distanceUnit = " in    ";
	}

	int spaceToAdd = 0;
	if (distance < 10) {
		spaceToAdd = 2;
	}
	else if (distance < 100) {
		spaceToAdd = 1;
	}

	distanceMessage = "Dist: ";
	for (int i = 0; i < spaceToAdd; i++) {
		distanceMessage += " ";
	}
	distanceMessage += String(distance, 2);
	distanceMessage += distanceUnit;

	// Serial.println(distanceMessage);

	lcd.setCursor(0, 0);
	lcd.print(distanceMessage);
}

// #Main ==================================================================================================

void setup() {
	Serial.begin(115200);
	
	setUpLeds();
	setUpUltrasonic();
	setUpPhotoresistor();
	setUpButton();
	
	lcd.begin(16, 2);

	IrReceiver.begin(IR_RECEIVE_PIN);
	
	byte eepromRead = EEPROM.read(EEPROM_ADDRESS);
	if (eepromRead == 255) {//the default EEPROM value
		systemUnit = CM;
	}
	else {
		systemUnit = (systemUnits)(eepromRead);
	}
}

void loop() {
	// read ultrasonic
	unsigned long timeNow = millis();
	int commandValue = 100;

	// get IR controller signal & change system mode
	if (timeNow - lastTimeIRPressed >= irDebounce) {
		if (IrReceiver.decode()) {
			IrReceiver.resume();
			// Serial.println(IrReceiver.decodedIRData.command);
			commandValue = IrReceiver.decodedIRData.command;
		}
		lastTimeIRPressed = timeNow;
	}
		
	// change system mode
	if (systemMode == LCD_LOCK) {// in system lock 
		if ((commandValue == IR_BUTTON_PLAY) || buttonIsPressed) {
			systemMode = LCD_DISTANCE;
		}
	}
	else {
		if ((systemMode == LCD_RESET) && (commandValue == IR_BUTTON_OFF)) {
			systemMode = LCD_DISTANCE;
			systemUnit = CM;
			EEPROM.write(EEPROM_ADDRESS, (byte)systemUnit);
		}
		switch (commandValue) {
			case IR_BUTTON_FUNCSTOP: {
				systemMode = LCD_RESET;
				break;
			}
			case IR_BUTTON_PLAY: {
				systemMode = LCD_DISTANCE;
				break;
			}
			case IR_BUTTON_UP: {
				nextMode();
				break;
			}
			case IR_BUTTON_DOWN: {
				prevMode();
				break;
			}
			case IR_BUTTON_UNIT_TOGGLE: {
				systemUnit = (systemUnit == CM) ? IN : CM;  // Toggle between CM and IN
				EEPROM.write(EEPROM_ADDRESS, (byte)systemUnit);
				break;
			}
			default: {
				//no button press on IR
			}
		}
	}
	
	buttonIsPressed = false; //reset button
	
	if (systemMode == LCD_DISTANCE) {
		//Ultrasonic 
		if (timeNow - lastTimeUltrasonicTrigger >= ultrasonicTriggerDelay) {
			ultrasonicSendTrigger();
			lastTimeUltrasonicTrigger += ultrasonicTriggerDelay;
			if (newTimeAvailable) {
				newTimeAvailable = false;
				ultrasonicEchoDuration = ultrasonicEchoEnd - ultrasonicEchoBegin;
				ultrasonicEchoDuration = constrain(ultrasonicEchoDuration, ECHO_MIN, ECHO_MAX); // limit the lower and upper bounds based on HC-SR04 specs
				initialEchoReading = true;
				ultrasonicEchoDistanceCm = (ultrasonicEchoDuration / 2.0) * 0.0343;
				ultrasonicEchoDistanceIn = ultrasonicEchoDistanceCm * 0.393701;

				systemOnLedDelay = map(ultrasonicEchoDistanceCm, 
								ultrasonicDistanceRange[0], ultrasonicDistanceRange[1], 
								systemOnledDelayRange[0], systemOnledDelayRange[1]);
			}
		}

		// change to LCD_LOCK when it gets too close
		if (initialEchoReading && ultrasonicEchoDistanceCm < lockZoneDistanceCm) {
			systemMode = LCD_LOCK;
		}
		
		// blink yellow LED
		if (timeNow - lastTimeSystemOnLedTrigger > systemOnLedDelay) {
			blinkYellowLed();
			lastTimeSystemOnLedTrigger = timeNow;
		}
	}
	else if (systemMode == LCD_LOCK) { // when system is locked
		if (timeNow - lastTimeSystemLockLedTrigger > systemLockLedDelay) {
			blinkRedYellowLed();
			lastTimeSystemLockLedTrigger = timeNow;
		}
		
	}
	
	// get luminosity value & change Green LED brightness
	if (timeNow - lastTimeLuminosityRead > luminosityReadDelay) {
		luminosityValue = analogRead(PHOTORESISTOR_PIN);
		lastTimeLuminosityRead += luminosityReadDelay;
		int greenLedDuty = map(luminosityValue, minLuminosity, maxLuminosity, 255, 0);
		analogWrite(LED_GREEN_PIN, greenLedDuty);
	}

	// call control center
	lcdControlCenter();
}