/* 
 * =======================================================================
 *                      IR REMOTE MUTE BUTTON
 * ======================================================================
 * 
 * Beware with a low volatage supply this code may result in the LED staying on.
 * 
 * More info about this proect is avalable at https://garrysblog.com/2020/12/05/ir-remote-control-mute-button/
 * 
 * Microcontroller goes into sleep mode while button is not pressed. When
 * pressed mcu wakes up sends codes and goes back to sleep to conserve power.
 * 
 * Button between D2 and GND
 * IR LED to D3. Use transistor and resistor to get best ranges without damaging mcu
 * with too much current https://learn.adafruit.com/using-an-infrared-library/sending-ir-codes
 */

#include <IRremote.h>       // IR remote control library
#include <avr/sleep.h>      // AVR library for controlling the sleep modes

const int buttonPin = 2;    // Mute button pin
const int IRpin = 3;        // To transistor via resistor
const int monitorPin = 13;  // Builtin LED

IRsend irsend;

void setup()
{ 
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(monitorPin, OUTPUT);
  //pinMode(IRpin, OUTPUT);

  ADCSRA = 0;
}

void loop () {
 
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Setting the sleep mode, in this case full sleep
  sleep_enable();                       // Enabling sleep mode
  
  // Using Nick Gammon's info turn off Brown Out Detection https://gammon.com.au/power
  // BODS must be set to one and BODSE must be set to zero within four clock cycles. This sets
  // the MCU Control Register (MCUCR)
  MCUCR = bit (BODS) | bit (BODSE);

  // The BODS bit is automatically cleared after three clock cycles
  MCUCR = bit (BODS);
  
  noInterrupts();                       // Disable interrupts temporarily
  attachInterrupt(digitalPinToInterrupt(buttonPin), button_ISR, LOW);
  interrupts();                         // Allow interrupts again
  sleep_cpu();                          // Enter sleep mode

  // The program will continue from here when it wakes 
    
  // Send mute codes 
  // Edifier computer amp - Mute code only needs to be sent once
  digitalWrite(monitorPin, HIGH);       // Turn on builtin LED for debugging
  irsend.sendNEC(0x8E700FF, 32);        // Edifier mute code
  digitalWrite(monitorPin, LOW);        // Turn off builtin LED
  delay(200);                           // Delay a bit

  // Sony TV - Mute code must be sent at least 3 times
  digitalWrite(monitorPin, HIGH);       // Turn on builtin LED for debugging
  for (int i = 0; i < 3; i++) {
      irsend.sendSony(0x290, 12);       // Sony mute code
      delay(40);
  }
  digitalWrite(monitorPin, LOW);        // Turn off builtin LED

  digitalWrite(IRpin, LOW);             // Ensure IR LED is off. This should not be needed
  delay(700);                           // Delay a bit to prevent from rapidly sending mute and unmute
}

void button_ISR() {
  // Runs on any interrupt. This will wake up the µController
  sleep_disable(); // Disable sleep mode
  detachInterrupt(digitalPinToInterrupt(buttonPin)); // Detach the interrupt to stop it firing
}
 
