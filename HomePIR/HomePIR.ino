#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
//#include "uart.h"

const uint8_t PIN_LIGHT = 4; // pin 3
const uint8_t PIN_PIR2  = 2; // pin 7
const uint8_t PIN_PIR1  = 1; // pin 6
const uint8_t PIN_PIR0  = 0; // pin 5

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
  //set timer to 1 sec
  //WDTCR |= (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (0<<WDP0);
  // set timer to 4s
  //WDTCR |= (1<<WDP3);
  // set timer to 8s
  //WDTCR |= (1<<WDP3) | (1<<WDP0);

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

  pinMode(PIN_PIR0, INPUT);
  pinMode(PIN_PIR1, INPUT);
  pinMode(PIN_PIR2, INPUT);
  pinMode(PIN_LIGHT, OUTPUT);
  watchdog_setup();
  delay(10);

}


void loop() {
  int pir_in0 = digitalRead(PIN_PIR0);
  int pir_in1 = digitalRead(PIN_PIR1);
  int pir_in2 = digitalRead(PIN_PIR2);

  if(pir_in0 == LOW && \
     pir_in1 == LOW && \
     pir_in2 == LOW  ) {
     digitalWrite(PIN_LIGHT, LOW);
  } else {
    digitalWrite(PIN_LIGHT, HIGH);
  }
  sleep();
}
