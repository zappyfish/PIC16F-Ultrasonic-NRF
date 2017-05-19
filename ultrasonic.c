/*
 * File:   newmain.c
 * Author: Liam
 *
 * Created on May 17, 2017, 11:31 PM
 */


#include "hc_sr04.h"
#include <xc.h>
#include "pins.h"

// PIC16F1503 Configuration Bit Settings

// 'C' source line config statements

 // CONFIG1 
 #pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin) 
 #pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled) 
 #pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled) 
 #pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR) 
 #pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled) 
 #pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled) 
 #pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin) 
 
 // CONFIG2 
 #pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off) 
 #pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset) 
 #pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.) 
 #pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled) 
 #pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming) 
 

// Pwm on timer 2, pwm3
void configLEDTimer(uint16_t);
void setDutyCycle(uint16_t);
void blink1Sec(void);

const uint16_t TMR_THRESH = 60000; // (freq (tics/sec) * .03 secs)/8 prescale

int main(void) {
    //configLEDTimer(0x3cf);
    OSCCON = 0b01111011; // 16Mhz, system clock = intosc
    //LED_ENABLE = 0;
    configUltraSonicIO();
    configTimer();
    configLEDTimer(0);
    //blink1Sec();
    while(1) {
        pulse();
        while(!ECHO1 && TMR1 < TMR_THRESH){ }
        startTimer();
        while(ECHO1 && TMR1 < TMR_THRESH){}
        uint16_t ticks = TMR1;
        double timeuS = ((double)ticks)/2; // 16 MHz, prescale 8
        double cm = timeuS/58.82;
        if(cm > 50) {
            cm = 500;
        }
        else {
            cm = cm * 10;
        }
        uint16_t dc = (cm/500) * 0x3FF;
        setDutyCycle(dc);
        __delay_ms(20);
    }
    return 0;
}



void configLEDTimer(uint16_t dc) {
    // disable pwmx pin output drivers by setting tris bits
    LED_ENABLE = 1; // disable output
    // clear pwmxcon
    PWM3CON = 0;
    // load pr2 register with pwm period value
    PR2 = 0x7F;
    // clear pwmxdch reigsters and bits <7:6> of pwmxdcl register
    setDutyCycle(dc);
    // configure and start timer2: clear tmr2if flag of pir1 register,
    PIR1bits.TMR2IF = 0;
    T2CON = 0b00000100; // prescale = 1, timer on
    // configure t2ckps bitsd of t2con register with timer2 prescale
    // enable pwm output pin and wait until timer2 overflows (tmr2if
    // of pir1 is set)
    while(!PIR1bits.TMR2IF);
    LED_ENABLE = 0;
    PWM3CON = 0b11010000;
    // enable pwmx output drivers by clearing TRIS bits
    
    // set the pwmxOE bit of PwmxCon reigster
    // configure pwm module w/ PWMxCON
    
}

void setDutyCycle(uint16_t dc) {
    uint16_t masked = dc & 0x3FF;
    PWM3DCH = (masked >> 2); // get 8 Msbs
    PWM3DCL = (masked & 0x003); // get 2 Lsbs
}

void blink1Sec(void) {
    LED = 1;
    for(int i = 0; i < 1000; ++i) {
        __delay_ms(1);
    }
    LED = 0;
    for(int i = 0; i < 1000; ++i) {
        __delay_ms(1);
    }
}
