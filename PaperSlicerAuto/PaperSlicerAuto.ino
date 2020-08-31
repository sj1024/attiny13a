00000#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"

const uint8_t SERVO_PIN = 2; // pin 7
const uint8_t INT_PIN = 1; // pin 6
const uint8_t PWREN_PIN= 0; // pin 5

#define NEWMODEL 0
#define OLDMODEL 1
#define PWRMODEL 2
#define MODEL -1

//#define MODEL OLDMODEL
//#define MODEL NEWMODEL
#define MODEL PWRMODEL

#define RX 3
#define TX 4

#if(MODEL==OLDMODEL)
#define DEG0 (260)
#define DEG180 (3200)
#elif(MODEL==NEWMODEL)
#define DEG0 (260)
#define DEG180 (3000)
#else
//#define TIME 40
#define TIME 50
#define HOME   angle(400, TIME)
//#define CUT   angle(1800, TIME)
#define CUT   angle(1750, TIME)
#endif


#define sleep_mode() \
do {  \
sleep_enable(); \
sleep_cpu();  \
sleep_disable();  \
}while(0)


void angle(volatile  int __angle, volatile  int rev){
  int peri = 15000;
  
  int low = peri - __angle;
  int high = __angle;
  
  for ( int i=0;i<rev;i++) {
    digitalWrite(SERVO_PIN, LOW);
    for( int k=0; k<low; k++) {
        __asm__("nop\n\t");
    }
    digitalWrite(SERVO_PIN, HIGH);
    for( int j=0; j<high; j++) {
        __asm__("nop\n\t");
    }
  }
}

ISR(PCINT0_vect) {
  
}

void setup() 
{
  char c;

  uart_puts("Hello World");
  
  pinMode(PWREN_PIN, OUTPUT);
  pinMode(INT_PIN, INPUT);
  pinMode(SERVO_PIN, OUTPUT);
  digitalWrite(PWREN_PIN, HIGH); 
  delay(500);
  //pinMode(BAT_PIN, INPUT);
#if(MODEL==OLDMODEL)
  angle(DEG0, 25);
#elif(MODEL==NEWMODEL)
  angle(DEG180*5, 30);
#elif(MODEL==PWRMODEL)
  HOME;
  CUT;
  HOME;
#else
#error("YOU SHOULD CHOOSE A MODEL")
#endif
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
    digitalWrite(PWREN_PIN, HIGH); 
    delay(100);
    //digitalWrite(, HIGH);
#if(MODEL==OLDMODEL)    
    angle(DEG180, 30);
    angle(DEG0, 25);
#elif(MODEL==NEWMODEL)
    angle(DEG0, 30);
    angle(DEG180, 25);
#elif(MODEL==PWRMODEL)
    CUT;
    HOME;
#else
#error("YOU SHOULD CHOOSE A MODEL")
#endif
    delay(500);
    do {
      paper_in = digitalRead(INT_PIN); 
    } while(paper_in == HIGH);

  } else {

  }
  digitalWrite(PWREN_PIN, LOW); 
  //sleep_mode();
  sleep();

}
