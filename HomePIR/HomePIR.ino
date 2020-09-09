#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//#include "uart.h"

const uint8_t PIN_LIGHT = 4; // pin 3
const uint8_t PIN_PIR2  = 2; // pin 7
//const uint8_t PIN_PIR2  = 3; // pin 2
const uint8_t PIN_PIR1  = 1; // pin 6
const uint8_t PIN_PIR0  = 0; // pin 5

//#define RX 3  // pin 2
//#define TX 4  // pin 3

ISR(PCINT0_vect) {
}
ISR(PCINT1_vect) {
}
ISR(PCINT2_vect) {
}

void setup()
{
  //uart_puts("Hello World");

  pinMode(PIN_PIR0, INPUT);
  pinMode(PIN_PIR1, INPUT);
  pinMode(PIN_PIR2, INPUT);
  pinMode(PIN_LIGHT, OUTPUT);
  digitalWrite(PIN_LIGHT, LOW);
  delay(500);
}

void sleep() {

    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT0);                   // Use PB0 as interrupt pin
    PCMSK |= _BV(PCINT1);                   // Use PB1 as interrupt pin
    PCMSK |= _BV(PCINT2);                   // Use PB2 as interrupt pin
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


void loop() {
  int pir_in0, pir_in1, pir_in2;

  delay(100);
  pir_in0 = digitalRead(PIN_PIR0);
  pir_in1 = digitalRead(PIN_PIR1);
  pir_in2 = digitalRead(PIN_PIR2);

  if(pir_in0 == LOW && \
     pir_in1 == LOW && \
     pir_in2 == LOW  ) {
	 digitalWrite(PIN_LIGHT, LOW);
  } else {
	 digitalWrite(PIN_LIGHT, HIGH);
  }
  sleep();
}
