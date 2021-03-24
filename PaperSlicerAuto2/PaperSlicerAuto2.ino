#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"

const uint8_t SERVO_PIN = 2; // pin 7
const uint8_t INT_PIN   = 1; // pin 6
const uint8_t PWREN_PIN = 0; // pin 5
const uint8_t DIR_PIN   = 4; // pin 3
//const uint8_t xxx_PIN = 3; // pin 2

#define RX 3
#define TX 4

#define TIME 50

#define ROTATION 200
#define SPEED    2500

ISR(PCINT0_vect) {
}

void delay() {
  for(int i=0;i<SPEED;i++) {
    __asm__("nop\n\t");
  }
}

void cut() {
  digitalWrite(DIR_PIN, LOW);
  for(int i=0;i<ROTATION;i++) {
    digitalWrite(SERVO_PIN, LOW);
    delay();
    digitalWrite(SERVO_PIN, HIGH);
    delay();
  }
}


void setup()
{
  char c;

  uart_puts("Hello World");
  pinMode(PWREN_PIN, OUTPUT);
  pinMode(INT_PIN, INPUT);
  pinMode(SERVO_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(PWREN_PIN, HIGH);
  delay(500);
  cut();
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
  int paper_in=0;

  paper_in = digitalRead(INT_PIN);

  if(paper_in == HIGH) {

    cut();
    delay(500);
    do {
      paper_in = digitalRead(INT_PIN);
    } while(paper_in == HIGH);

  }
  digitalWrite(PWREN_PIN, LOW);
  sleep();
}
