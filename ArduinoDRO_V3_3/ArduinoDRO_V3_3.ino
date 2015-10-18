/*
  iGaging/AccuRemote Digital Scales Controller V3.3
  Created 5 July 2014
  Update 15 July 2014

  Copyright (C) 2014 Yuriy Krushelnytskiy, http://www.yuriystoys.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// NOTE: This sketch was designed for Arduino variants that use ATMega 328
// such as Arduino UNO, Nano, etc.
// Refer to the diagram below for wiring information
//
//                                   +-\/-+
//                             PC6  1|    |28  PC5 (AI 5)
//  > -- To BT Tx Pin -- (D 0) PD0  2|    |27  PC4 (AI 4)
//  < -- To BT Rx Pin -- (D 1) PD1  3|    |26  PC3 (AI 3)
//  < --  Scale Clock -- (D 2) PD2  4|    |25  PC2 (AI 2)
//  > -- X Axis Input -- (D 3) PD3  5|    |24  PC1 (AI 1)
//  > -- Y Axis Input -- (D 4) PD4  6|    |23  PC0 (AI 0)
//                             VCC  7|    |22  GND
//                             GND  8|    |21  AREF
//                             PB6  9|    |20  AVCC
//                             PB7 10|    |19  PB5 (D 13) > -- System Tick LED --
//  > -- Z Axis Input -- (D 5) PD5 11|    |18  PB4 (D 12) > -- Heart Beat LED  --
//  > -- W Axis Input -- (D 6) PD6 12|    |17  PB3 (D 11)
//  > -- Tach input A -- (D 7) PD7 13|    |16  PB2 (D 10)
//  > -- Tach input B -- (D 8) PB0 14|    |15  PB1 (D 9)
//                                   +----+
//
//  IMPORTANT: If using a 5V Arduino variant, Scale clock requires a voltage divider
//  NOTE 1: All input lines require a pull-down resistor in 10-47 KOhm range
//  NOTE 2: Tach input B is optional (it's used for direction)

#include <Wire.h> 
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified backpack


void setuplcd()
{
 
lcd.begin (16,2); // for 16 x 2 LCD module
lcd.setBacklightPin(3,POSITIVE);
lcd.setBacklight(HIGH);
}

/* General Settings */

#define UART_BAUD_RATE 9600       //  Set this so it matches the BT module's BAUD rate 
#define UPDATE_FREQUENCY 24       //  Frequency in Hz [must be an even number] (number of timer per second the scales are read and the data is sent to the application)
#define HALF_UPDATE_FREQUENCY 12  //not sure if Arduino optimizes out division by 2, so this sure will :)

//set the values for the axes that should be disabled to 0
//NOTE: X axis needs to be enabled for the app to work correctly
#define Y_ENABLED 1
#define Z_ENABLED 1
#define W_ENABLED 1
#define T_ENABLED 1

/* iGaging Clock Settins (do not change) */

#define SCALE_CLK_PULSES 21       //iGaging and Accuremote sclaes use 21 bit format
#define SCALLE_CLK_FREQUENCY 9000 //iGaging scales run at about 9-10KHz


/* GPIO Definitions */

#define TACH_A_PORT PIND
#define TACH_B_PORT PINB

#define TACH_A_DDR DDRD
#define TACH_B_DDR DDRB

#define TACH_A_PIN 7
#define TACH_B_PIN 0

#define SCALE_CLK_OUTPUT_PORT PORTD
#define SCALE_CLK_DIR_PORT DDRD
#define SCALE_CLK_PIN 2           //Arduino Digital pin 8

#define X_INPUT_PORT PIND
#define Y_INPUT_PORT PIND
#define Z_INPUT_PORT PIND
#define W_INPUT_PORT PIND

#define X_DDR DDRD
#define Y_DDR DDRD
#define Z_DDR DDRD
#define W_DDR DDRD

#define X_PIN 6                   //Arduino Digital pin 9
#define Y_PIN 7                   //Arduino Digital pin 10
#define Z_PIN 8                   //Arduino Digital pin 11
#define W_PIN 9                   //Arduino Digital pin 12

#define ENABLE_LEDS 1

//LEDs are used to provide visual feedback
#if ENABLE_LEDS > 0

#define LED1_PORT PORTB
#define LED1_DDR DDRB
#define LED1_PIN 4

#endif

/* Funstion definitions */

void startTickTimer(int frequency);
void startClkTimer();
void stopClkTimer();

/* Variable definitions */

volatile byte tickTimerFlag = 0;

byte loopCount = 0;

byte bitOffset = 0;
byte clockPinHigh = 0;

volatile long xValue = 0L; //X axis count
volatile long yValue = 0L; //Y axis count
volatile long zValue = 0L; //Z axis count
volatile long wValue = 0L; //W axis count

volatile long tValue = 0L; //Tachometer count

#if ENABLE_LEDS > 0

int elapsedTicks = 0;

#endif

void setup() {
  lcd.begin (16,2); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  cli();      //stop interrupts

  //scale clock shoudl be output
  SCALE_CLK_DIR_PORT |= _BV(SCALE_CLK_PIN);         //set clock pin to output

  //set input pins as such
  X_DDR &= ~_BV(X_PIN);
  Y_DDR &= ~_BV(Y_PIN);
  Z_DDR &= ~_BV(Z_PIN);
  W_DDR &= ~_BV(W_PIN);

  TACH_A_DDR &= ~_BV(TACH_A_PIN);
  TACH_B_DDR &= ~_BV(TACH_B_PIN);


  Serial.begin(UART_BAUD_RATE);                // set up Serial library

  startTickTimer(UPDATE_FREQUENCY);            //init and start the "system tick" timer

  setupClkTimer(SCALLE_CLK_FREQUENCY);         //init the scale clock timer (don't start it yet)

#if T_ENABLED > 0

  loopCount = 0;
  //attachInterrupt(0, readTach, RISING);

  //setup pin change interrupt
  PCICR |= _BV(PCIE2);
  PCMSK2 |= _BV(PCINT23); //Trigger on change of PCINT18 (PD2)


#endif

#if ENABLE_LEDS > 0

  //if the LEDs are enabled, set the pins as outputs
  LED1_DDR |= _BV(LED1_PIN);
  //LED2_DDR |= _BV(LED2_PIN);

  LED1_PORT &= ~_BV(LED1_PIN);
  //LED2_PORT &= ~_BV(LED2_PIN);

#endif

  sei();                                      //allow interrupts
}

void loop() {

  //LED1_PORT ^= _BV(TACH_A_PORT & _BV(TACH_A_PIN) ? 1 : 0);

  if (tickTimerFlag)
  {
    tickTimerFlag = 0;
    //handle system tick



    if (++loopCount == HALF_UPDATE_FREQUENCY) //tachometer is double couinting in the ISR, we we can upadte it twice per second
    {
#if ENABLE_LEDS > 0
      //toggle LED 1 once per second
      LED1_PORT ^= _BV(LED1_PIN);
      elapsedTicks = 0;
#endif

#if T_ENABLED > 0

      //tachometer updates once per second
      //stop the interrupts
      cli();

      long tempT = tValue;
      tValue = 0;

      //re-enable the interrupts
      sei();

      loopCount = 0;

      Serial.print(F("T"));
      Serial.print((long)tempT);
      Serial.print(F(";"));

#endif
    }



    Serial.print(F("X"));
    Serial.print((long)xValue);
    Serial.print(F(";"));
    
    lcd.setCursor(0,0);
    lcd.print("X ");
    lcd.print((long)xValue);

    //send the rest of the positions only if they are enabled
#if Y_ENABLED > 0

    Serial.print(F("Y"));
    Serial.print((long)yValue);
    Serial.print(F(";"));
    
    lcd.setCursor(8,1);
    lcd.print("Y ");
    lcd.print((long)yValue);

#endif


#if Z_ENABLED > 0

    Serial.print(F("Z"));
    Serial.print((long)zValue);
    Serial.print(F(";"));
    
    lcd.setCursor(0,1);
    lcd.print("Z ");
    lcd.print((long)zValue);

#endif


#if W_ENABLED > 0

    Serial.print("W");
    Serial.print((long)wValue);
    Serial.print(";");
    Serial.print("\n");

#endif

    //reset the reader for the next batch (tachometer should not be cleared here)
    bitOffset = 0;

    xValue = 0;

#if Y_ENABLED > 0

    yValue = 0;

#endif

#if Z_ENABLED > 0

    zValue = 0;

#endif


#if W_ENABLED > 0

    wValue = 0;

#endif

    //start reading again
    startClkTimer();

  }
}



void startTickTimer(int frequency)
{
  //set timer1 interrupt
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0

  // set compare match register
  OCR1A = F_CPU / (1024 * frequency) - 1; //must be <65536

  // turn on CTC mode
  TCCR1B |= (1 << WGM12);

  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);

  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
}

//initializes the timer for the scale clock but does not start it
void setupClkTimer(int frequency)
{
  bitOffset = 0;

  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B

  // set compare match register
  OCR2A = F_CPU / (8 * frequency) - 1; // 160 - 1;

  // turn on CTC mode
  TCCR2A |= (1 << WGM21);

  // Set CS21 bit for 8 prescaler //CS20 for no prescaler
  TCCR2B |= (1 << CS21);
}

//starts scale clock timer
void startClkTimer()
{
  //initialize counter value to 0
  TCNT2  = 0;
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
}

//stops scale clock timer
void stopClkTimer()
{
  // disable timer compare interrupt
  TIMSK2 &= ~(1 << OCIE2A);
}


/* Interrupt Service Routines */


//timer 1 compare interrupt service routine (provides system tick)
ISR(TIMER1_COMPA_vect) {

  tickTimerFlag = 1;

}

//Timer 2 interrupt (scale clock driver)
ISR(TIMER2_COMPA_vect) {
  //scale reading happens here
  if (!clockPinHigh)
  {
    SCALE_CLK_OUTPUT_PORT |= _BV(SCALE_CLK_PIN);
    clockPinHigh = 1;
  }
  else
  {

    clockPinHigh = 0;

    SCALE_CLK_OUTPUT_PORT &= ~_BV(SCALE_CLK_PIN);

    //read the pin state and shif it into the appropriate variables
    xValue |= ((long)(X_INPUT_PORT & _BV(X_PIN) ? 1 : 0) << bitOffset);

    //read the rest of the scales only if they are enabled

#if Y_ENABLED > 0

    yValue |= ((long)(Y_INPUT_PORT & _BV(Y_PIN) ? 1 : 0) << bitOffset);

#endif


#if Z_ENABLED > 0

    zValue |= ((long)(Z_INPUT_PORT & _BV(Z_PIN) ? 1 : 0) << bitOffset);

#endif


#if W_ENABLED > 0

    wValue |= ((long)(W_INPUT_PORT & _BV(W_PIN) ? 1 : 0) << bitOffset);

#endif

    //increment the bit offset
    bitOffset++;

    if (bitOffset >= SCALE_CLK_PULSES)
    {
      //stop the timer after the predefined number of pulses
      stopClkTimer();

      if (X_INPUT_PORT & _BV(X_PIN))
        xValue |= ((long)0x7ff << 21);


#if Y_ENABLED > 0

      if (Y_INPUT_PORT & _BV(Y_PIN))
        yValue |= ((long)0x7ff << 21);

#endif

#if Z_ENABLED > 0

      if (Z_INPUT_PORT & _BV(Z_PIN))
        zValue |= ((long)0x7ff << 21);

#endif

#if W_ENABLED > 0

      if (W_INPUT_PORT & _BV(W_PIN))
        wValue |= ((long)0x7ff << 21);

#endif
    }
  }
}

ISR(PCINT2_vect)
{
  if (TACH_B_PORT & _BV(TACH_B_PIN))
  {
    tValue++;
  }
  else
  {
    tValue--;
  }
}
