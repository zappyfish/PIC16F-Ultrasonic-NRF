
#include "hc_sr04.h"
#include "pins.h"
#include <xc.h>


void configTimer(void) {
    T1CON = 0b01110100; // timer1 is 16Mhz, 1:8 prescale
}


void startTimer(void) {
    T1CON &= 0b11111110; // turn off timer
    TMR1L = 0;
    TMR1H = 0; // reset timer
    T1CON |= 0b00000001; // turn on timer
}

void pulse(void) {
    TRIGGER = 1;
    __delay_us(50);
    TRIGGER = 0;
    // startTimer();
}

void configUltraSonicIO(void) {
    TRIGGER_ENABLE = 0;
    ECHO1_ENABLE = 1;
    ANSELC = 0x00;
    ANSELA = 0x00;
}

void configClock(void) {
    OSCCON = 0b01111011; // 16Mhz, system clock = intosc
}