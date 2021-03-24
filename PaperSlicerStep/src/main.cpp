#include <Arduino.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"

const uint8_t PIN_STEP= 2; // pin 7
const uint8_t PIN_DIR = 0; // pin 5
const uint8_t PIN_INT = 1; // pin 6
const uint8_t PIN_DONE = 4; // pin 3
// const uint8_t PIN_INT = 3; // pin 2

#define RX 3
#define TX 4

#define SPEED 1500

void blink() {
    for( int i=0; i<SPEED; i++) {
        __asm__("nop\n\t");
    }
}

void step(int s) {
    for( int i=0; i<s; i++) {
        digitalWrite(PIN_STEP, LOW);
        blink();
        digitalWrite(PIN_STEP, HIGH);
        blink();
    }
}



ISR(PCINT0_vect) {

}


void setup()
{
    uart_puts("Hello World");
    pinMode(PIN_DONE, INPUT);
    pinMode(PIN_INT, INPUT);
    pinMode(PIN_DIR, OUTPUT);
    pinMode(PIN_STEP, OUTPUT);
    digitalWrite(PIN_DIR, LOW);
    digitalWrite(PIN_STEP, HIGH);
    delay(500);
}


void sleep() {

    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT1);                   // Use PB1 as interrupt pin
    ADCSRA &= ~_BV(ADEN);                   // ADC off
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement
    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep
    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT1);                  // Turn off PB1 as interrupt pin
    sleep_disable();                        // Clear SE bit
    ADCSRA |= _BV(ADEN);                    // ADC on
    sei();                                  // Enable interrupts
} // sleep


void loop() {
    // int paper_in=0;
    // paper_in = digitalRead(PIN_INT);
    // if(paper_in == HIGH) {
    //     digitalWrite(PIN_DIR, HIGH);
    //     delay(100);
    //     delay(500);
    //     do {
    //         paper_in = digitalRead(PIN_INT);
    //     } while(paper_in == HIGH);
    // }
    // digitalWrite(PIN_DIR, LOW);
    // sleep();
    while(1) {
        step(500);
    }
}

