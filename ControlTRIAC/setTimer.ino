/**************************************************************************************************
 * setTimer.ino
 *
 * Created: 12.10.2015
 * Author: Lars Lindner
 *
 * Implementation of Hardware Timer 0 - 2 in Arduino Uno
 * Timer0 prescaler: 1, 8, 64, 256, 1024 + External clock source on T0 pin
 * Timer1 prescaler: 1, 8, 64, 256, 1024 + External clock source on T1 pin
 * Timer2 prescaler: 1, 8, 32, 64, 128, 256, 1024
 *
 * Timer0 compare match register < 256
 * Timer1 compare match register < 65536
 * Timer2 compare match register < 256
 *
 * Attention:	Don't use Timer0, because it's messing with serialEvent(). 
 *				Also Timer0 is used for millis().
 *
***************************************************************************************************/


// Sets 1 of 3 Timers in predefined Mode
void setTimer(int timer_numb)
{
	if (timer_numb == 0)
	{
		//set timer0 interrupt at 100Hz (10ms)

		// Global disable interrupts
		cli();

		// Initialize counter value to 0
		TCNT0  = 0;
		
		// set compare match register for 100Hz increments
		// (16*10^6) / (2 * prescaler * 100) - 1 (must be <256)
		OCR0A = 77.125;			
		// Waveform Generation Mode 2 (CTC Mode, OCR0A TOP)
		TCCR0A = (0 << COM0A1) | (0 << COM0A0) | (0 << COM0B1) | (0 << COM0B0) | (1 << WGM01) | (0 << WGM00);
		// Prescaler = 1024
		TCCR0B = (0 << FOC0A) | (0 << FOC0B) | (0 << WGM02) | (1 << CS02) | (0 << CS01) | (1 << CS00);

		// Interrupt(s) initialization
		TIMSK0 = (0 << OCIE0B) | (1 << OCIE0A) | (0 << TOIE1);

		// Global enable interrupts
		sei();
	}
	else if (timer_numb == 1)
	{
		// Global disable interrupts
		cli();
		TCCR1A = 0;		// set entire TCCR1A register to 0
		TCCR1B = 0;		// same for TCCR1B
		TCNT1  = 0;		//initialize counter value to 0

		// Waveform Generation Mode 14 (Fast PWM, ICR1 TOP)
		// In this mode, the counter counts only up, so for 1kHz PWM frequency ICR1 = 16000 for 16MHz oscillator
		ICR1 = 16000; // actually 15999
		//Defines duty cycle of PWM, can be changed while micro running
		OCR1A = OCR1B = 0;
		TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << COM1B1) | (0 << COM1B0) | (1 << WGM11) | (0 << WGM10);
		// Prescaler = 1
		TCCR1B = (0 << ICNC1) | (0 << ICES1) | (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);

		//// Waveform Generation Mode 10 (Phase Correct PWM Mode, ICR1 TOP)
		//// In this mode, the counter counts up and down, so for 1kHz PWM frequency ICR1 = 8000 for 16MHz oscillator
		//ICR1 = 8000;
		////Defines duty cycle of PWM, can be changed while micro running
		//OCR1A = OCR1B = 4000;
		//TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << COM1B1) | (0 << COM1B0) | (1 << WGM11) | (0 << WGM10);
		//// Prescaler = 1
		//TCCR1B = (0 << ICNC1) | (0 << ICES1) | (1 << WGM13) | (0 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);

		// Timer/Counter 1 Interrupt(s) initialization
		TIMSK1 = (0 << ICIE1) | (0 << OCIE1B) | (0 << OCIE1A) | (1 << TOIE1);

		// Global enable interrupts
		sei();
	}
	else if (timer_numb == 2)
	{
 		//set Timer2 interrupt at 1000Hz (1ms)
 
 		// Global disable interrupts
 		cli();
 
 		// Attention: Timer Register must be set 0 before setting OCR2A!!!
 		TCCR2A = 0;
 		TCCR2B = 0;
 		TCNT2  = 0;
 
 		// set compare match register for 1000Hz increments
 		// Formula not identical with datasheet!!!
 		// (16*10^6) / (prescaler * 1000) - 1 (must be <256)
 		OCR2A = 249;
 
 		// Waveform Generation Mode 2 (CTC Mode, OCR2A TOP)
 		TCCR2A = (0 << COM2A1) | (0 << COM2A0) | (0 << COM2B1) | (0 << COM2B0) | (1 << WGM21) | (0 << WGM20);
 		// Prescaler = 64
 		TCCR2B = (0 << FOC2A) | (0 << FOC2B) | (0 << WGM22) | (1 << CS22) | (0 << CS21) | (0 << CS20);
 
 		// Interrupt(s) initialization
 		TIMSK2 = (0 << OCIE2B) | (1 << OCIE2A) | (0 << TOIE2);
 
 		// Global enable interrupts
 		sei();

	}

};


// Working code for Timer1:

		////set timer1 interrupt at 1000Hz (1ms)
		//
		//// Global disable interrupts
		//cli();
		//
		//// Initialize counter value to 0
		//TCNT1 = 0;
		//
		//// actually sets Timer1 to 1000Hz increments, I don't know why!!!
		//// set compare match register for 100Hz increments
		//// (16*10^6) / (2 * prescaler * 100) - 1 (must be <256)
		//OCR1A = 77.125;
		//// Waveform Generation Mode 4 (CTC Mode, OCR1A TOP)
		//TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0) | (0 << WGM11) | (0 << WGM10);
		//// Prescaler = 1024
		//TCCR1A = (0 << FOC1A) | (0 << FOC1B) | (0 << WGM13) | (1 << WGM12) | (1 << CS12) | (0 << CS11) | (1 << CS10);
		//
		//// Timer/Counter 1 Interrupt(s) initialization
		//TIMSK1 = (0 << ICIE1) | (0 << OCIE1B) | (1 << OCIE1A) | (0 << TOIE1);
		//
		//// Global enable interrupts
		//sei();


// Working code for Timer2:

		////set timer2 interrupt at 1000Hz (1ms)
		//
		//// Global disable interrupts
		//cli();
		//
		//TCCR2A = 0;// set entire TCCR2A register to 0
		//TCCR2B = 0;// same for TCCR2B
		//TCNT2  = 0;//initialize counter value to 0
		//// set compare match register for 1khz increments
		//OCR2A = 249;// = (16*10^6) / (64 * 1000) - 1 (must be <256)
		//// turn on CTC mode
		//TCCR2A |= (1 << WGM21);
		////// Set CS21 bit for 8 prescaler
		////TCCR2B |= (1 << CS21);
		//// Set CS21 bit for 64 prescaler
		//TCCR2B |= (1 << CS22);
		//// enable timer compare interrupt
		//TIMSK2 |= (1 << OCIE2A);
		//
		//// Global enable interrupts
		//sei();
