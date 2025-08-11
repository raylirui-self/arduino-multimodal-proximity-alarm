# Arduino final project
## feature list
[x] object distance detection
	[x] **Unit Test:** basic distance detection
	[x] yellow LED blink frequency relates to proximity of an object
		[x] *LCD* display distance
		[x] *LCD* display message based on distance value
	[x] toggle yellow & red LEDs after object getting too close. 
		[x] shuts down functionalities until a button press
			[x]  remote controller press to un lock 
			[] button press press to unlock 
		[x] *LCD* display message
[x] green LED inversely relates to luminosity
	[x] potentiometer to set max LED brightness
[x] remote controller
	[x] remote controller press to change unit of distance
	[x] unlock system
	[x] toggle *LCD* screens
		[x] distance
		[x] luminsity
		[x] system reset
			[x] additional button-push confirmation to reset system
				- return to distance in cm
				- return to display distance
[] default distance unit in EEPROM

## technical requirements
1. LED
	[x] set up LEDs
	[x] yellow LED blink frequency proportional to Ultrasonic distance
	[x] yellow & red LED blink in *System Shutdown*
	[x] green LED inversely proportional to luminosity
		[x] get value from photoresistor
2. Ultrasonic Sensor+
	[x] set up Ultrasonic
	[x] return Echo pulse duration
		[x] use Interrupt to avoid system delay
	[x] return distance in cm and in
3. LCD
	[x] *Distance* screen
		[x] Distance in cm
		[x] Distance in in
	[x] *Luminosity* screen
	[x] *System Reset* screen
		[x] "Power On/Off" button push for reset
			[x] Distance in cm
			[] EEPROM in cm
	[x] *System Shutdown* screen
4. IR Remote Controller
	[x] "Play": Re-enable system
	[x] "^v": toggle LCD screens
	[x] "Power On/Off": reset system in the *System Reset*
5. Button
	[] Re-enable system
6. EEPROM
	[] stores the current Distance unit (cm | in)
7. Photoresistor
	[x] get value every 100ms

## To-do's in HW
[x] IR Remote controller
	[x] get button values
[x] Photoresister
	[x] upper & lower bound tuning

## requirements:
- code modularity
	- able to test each component/function separately
- code scalability

## knowledge included:
1. interrupt
2. independent time control (without delay)
3. Array
4. switch
5. importing library
6. Serial monitor
7. Scope
8. condition
9. for loop
10. function
11. debounce
12. EEPROM
13. PWM
14. Type casting

## components included
1. LED
2. button
3. LCD
4. IR remote controller
5. Photoresistor
6. resistor
7. ultrasonic sensor
8. breadboard
9. Arduino UNO
10. potentiometer

## skills
1. MVP
2. JIT
3. WBS
4. Iterative testing & development

## additional challenge
- OOP