/**************************************************************************************************
 * ControlTRIAC.ino
 *
 * Created: 18/04/2020
 * Author: Lars Lindner
 *
 * Power control of a resistive load using phase-fired control (https://en.wikipedia.org/wiki/Phase-fired_controller)
 * The moment of zero-cross of the mains signal is detected
 * The moment of firing of the Triac is controlled using delays
 * The delay for firing moment is received by serial over ESP8266 or using a potentiometer
 * loop() function must not be overloaded with tasks (serial read, analog read, etc.), otherwise the delay between zero-cross and firing moment will be too large!
 * Therefore, only read either serial data or potentiometer in loop()!!
 * (may be this behavior can be avoided using interrupts for serial and analog read)
 *
***************************************************************************************************/

#include<SoftwareSerial.h>

#define optoemitter_pin		2			// Optocoupler output to detect zero cross
#define moc_pin				8			// Solid State Relay input to control AC load
const int T2_ms = 1000;					// Execution period for Serial Communication [ms]
const byte numChars = 32;				// Max number of bytes of the received string
const int delay_min = 1200;				// Min delay for max power [us]
const int delay_max = 8000;				// Max delay for min power [us]
int delay_int = delay_min;				// 60Hz = 16666.66us -> 1/4 Halfwave = 4167us (halfwave, because we are rectifying)

volatile long count_zerocross = 0;
volatile long count_T2 = 0L;			// Count variable for Timer2
volatile bool flag_zerocross = false;
volatile bool flag_led = false;

SoftwareSerial mySerial(6,7);

char receivedChars[numChars];			// Array to store the received data
String rx_str = "";						// Store the received char array as an string
String cmd_str = "";					// Received command
int value_int = 0;						// Received command value
bool flag_newSerialData = false;		// When new Serial Data arrived
bool flag_newPotiData = false;			// When new Poti value arrived
int potval_int = 0;						// Old value of potentiometer

// volatile unsigned long prevMicros_long = 0;		// will store last time
// int delaycase = 1;								// switches between delays



// Setup function
void setup() 
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(optoemitter_pin, INPUT_PULLUP);
	pinMode(moc_pin, OUTPUT);
	attachInterrupt(digitalPinToInterrupt(optoemitter_pin), ISR_zerocross, FALLING);
	
	setTimer(2);
	Serial.begin(9600);
	mySerial.begin(9600);
	Serial.println("Hi user, everything connected...");
	Serial.println();
	
}


// Loop function
void loop() 
{
	// Only use one of this both functions (see code description)!
	recvSerialData();
	updatePotData();
	
	// Decide whether serialData is taken for delay_int
	if (flag_newSerialData)
	{
		rx_str = receivedChars;
		cmd_str = rx_str.substring(0,rx_str.indexOf('_'));
		value_int = rx_str.substring(rx_str.indexOf('_') + 1).toInt();
		delay_int = map(value_int,100,0,delay_min,delay_max);
				
		if (value_int == 0)			// A defined level for turning OFF
		{
			delay_int = delay_max;
		} 
		else if (value_int == 100)	// A defined level for turning ON
		{
			delay_int = delay_min;
		}

		flag_newSerialData = false;
	}
	// or PotiData
	else if (flag_newPotiData)
	{
		delay_int = map(potval_int,0,1024,delay_min,delay_max);
		flag_newPotiData = false;
	}

 	// Part which control the firing moment
	if(flag_zerocross)
	{
		delayMicroseconds(delay_int);
		digitalWrite(moc_pin, HIGH);
		delayMicroseconds(100);
		digitalWrite(moc_pin, LOW);
		flag_zerocross = false;
	}


//  	// Part which control the firing moment without blocking behavior (does work, but has another behavior, lamp is still flickering)
// 	if(flag_zerocross)
// 	{
// 		unsigned long actMicros_long = micros();
// 		if ((actMicros_long - prevMicros_long >= delay_int) && (delaycase == 1)) 
// 		{
// 			prevMicros_long = actMicros_long;
// 			delaycase = 2;
// 			digitalWrite(moc_pin, HIGH);
// 		}
// 
// 		if ((actMicros_long - prevMicros_long >= 100) && (delaycase == 2))
// 		{
// 			delaycase = 1;
// 			flag_zerocross = false;
// 			digitalWrite(moc_pin, LOW);
// 		}
// 	}
	
}




// ISR for Zero-Cross detection
void ISR_zerocross() 
{
// 	count_zerocross++;
// 	prevMicros_long = 0;
// 	reset_millis();
	flag_zerocross = true;
}


// Reset the millis and micro counter
void reset_millis()
{
	extern volatile unsigned long timer0_millis, timer0_overflow_count;
	noInterrupts();
	timer0_millis = timer0_overflow_count = 0;
	interrupts();
}


// ISR for Serial Print
ISR(TIMER2_COMPA_vect)
{
	count_T2++;
	if (count_T2 > T2_ms)
	{
// 		Serial.print("Mains freq: ");
// 		Serial.println(count_zerocross/2, DEC);
// 		Serial.print("Delay Value: ");
// 		Serial.println(delay_int);
		digitalWrite(LED_BUILTIN, flag_led);
		count_T2 = 0;
		flag_led = !flag_led;
		count_zerocross = 0;

	}
}


// For receiving serial data from Software Serial
void recvSerialData()
{
	static byte ndx = 0;
	char endMarker = '\n';
	char rc;
	
	while (mySerial.available() > 0 && flag_newSerialData == false)
	{
		rc = mySerial.read();

		if (rc != endMarker)
		{
			receivedChars[ndx] = rc;
			ndx++;
			if (ndx >= numChars)
			{
				ndx = numChars - 1;
			}
		}
		else
		{
			// terminate the string with the null character (see: https://www.arduino.cc/reference/en/language/variables/data-types/string/)
			receivedChars[ndx] = '\0';
			ndx = 0;
			flag_newSerialData = true;
		}
	}
}


// Checking, if the Poti gives a new value
void updatePotData()
{
	int potnewval_int = analogRead(A0);
	if (potnewval_int > (potval_int + 3) || potnewval_int < (potval_int - 3))
	{
		flag_newPotiData = true;
		potval_int = potnewval_int;
	} 
	else
	{
		flag_newPotiData = false;
	}
	
}