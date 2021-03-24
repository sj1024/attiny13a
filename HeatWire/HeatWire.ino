#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
//#include "uart.h"

const uint8_t PIN_LED   = 1; // pin 6
const uint8_t PIN_LIGHT = 4; // pin 3
const uint8_t PIN_TEMP  = 0; // pin 5

int gCounter = 0;
int gMachineState=0;
//#define RX 3  // pin 2
//#define TX 4  // pin 3

ISR(PCINT0_vect) {}
ISR(PCINT1_vect) {}
ISR(PCINT2_vect) {}
ISR(WDT_vect) {
}

void watchdog_setup() {
  // set timer to 0.5s
  WDTCR |= (0<<WDP3) | (1<<WDP2) | (0<<WDP1) | (1<<WDP0);
  // set timer to 1 sec
  // WDTCR |= (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (0<<WDP0);
  // set timer to 4s
  // WDTCR |= (1<<WDP3);
  // set timer to 8s
  // WDTCR |= (1<<WDP3) | (1<<WDP0);

  // Set watchdog timer in interrupt mode
  WDTCR |= (1<<WDTIE);
  WDTCR |= (0<<WDE);
}


void sleep() {

    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    //PCMSK |= _BV(PCINT0);                   // Use PB0 as interrupt pin
    //PCMSK |= _BV(PCINT1);                   // Use PB1 as interrupt pin
    //PCMSK |= _BV(PCINT2);                   // Use PB2 as interrupt pin
    ADCSRA &= ~_BV(ADEN);                   // ADC off
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep

    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT0);                  // Turn off PB0 as interrupt pin
    PCMSK &= ~_BV(PCINT1);                  // Turn off PB1 as interrupt pin
    PCMSK &= ~_BV(PCINT2);                  // Turn off PB2 as interrupt pin
    sleep_disable();                        // Clear SE bit
    ADCSRA |= _BV(ADEN);                    // ADC on

    sei();                                  // Enable interrupts
} // sleep


void setup() {
  //uart_puts("Hello World");

  pinMode(PIN_TEMP, INPUT);
  pinMode(PIN_LIGHT, OUTPUT);
  digitalWrite(PIN_LIGHT, HIGH); // OFF
  pinMode(PIN_LED, OUTPUT);
  watchdog_setup();
  delay(10);
  gMachineState = 0;

}

// 5*60 = 300 -> 600 
void each_5min() {
	int temp = digitalRead(PIN_TEMP);
	if(HIGH == temp) {
		gMachineState = 1;
		digitalWrite(PIN_LIGHT, LOW);
	}
	
}

// 10*60  = 600 -> 1200
void each_10min() {
	digitalWrite(PIN_LIGHT, HIGH);
	gMachineState = 0;
}

void loop() {
	static int blink=0;
	digitalWrite(PIN_LED, blink);
	
	switch(gMachineState) {
		case 0:
			if(gCounter > 600) {
				each_5min();
				gCounter = 0;
			}
		break;
		case 1:
			if(gCounter > 1200) {
				each_10min();
				gCounter = 0;
			}
		break;
	}
	blink ^= 1;
	gCounter++;
	sleep();
}
