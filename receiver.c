/*
 * File:   receiver.c
 * Author: Liam
 *
 * Created on April 14, 2017, 4:31 PM
 */

#include <stdint.h>
#include <xc.h>
#include "spi.h"
#include "NRF_consts.h"
#include "hc_sr04.h"

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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
// need to set TRIS bits:
/* SDIx must have TRIS bit set
 SDOx must have TRIS bit cleared
 * SCKx (master mode) must have TRIS bit cleared
 * SSx must have TRIS bit set
 
 */



#define _XTAL_FREQ 16000000UL

void configureRX(void);
void blink(int delay);
uint8_t readData(void);
void configLEDTimer(uint16_t);
void setDutyCycle(uint16_t dc);

void main(void) {
    SPI_init();
    configLEDTimer(0x0FF);
    configureRX();
    while(1) {
        NRF_CE = 1;
        while(IRQ); 
        NRF_CE = 0;
        uint8_t data = readData();
        resetIRQ();
        uint16_t dc = (((double)data)/50)*0x3ff;
        setDutyCycle(dc);
    }
}

void blink(int delay) {
    LED = 1;
    for(int i = 0; i < delay; ++i) {
        __delay_ms(5);
    }
    LED = 0;
    for(int i = 0; i < delay; ++i) {
        __delay_ms(5);
    }
}


void configureRX(void) {
    // 1. need to pull PRIM_RX bit low
    // 2. clock in address for receiving node (TX_ADDR) and payload data
    // (TX_PLD) via SPI. Must write TX_PLD continuously while holding CSN low.
    // TX_ADDR does not have to be rewritten if unchanged from last transmit!
    // If PTX device shall receive acknowledge, data pipe 0 has to be configured
    // to receive the acknowledgment
    // 3. A high pulse of CE starts the transmission. Minimum pulse width of
    // 10 microseconds
    NRF_CE = 0;
    __delay_ms(1);
    SS_PIN = 1;
    __delay_ms(10);
    uint8_t write[2];
    write[0] = (CONFIG & REGISTER_MASK) | W_MASK;
    write[1] = 0b00111011;  // config stuff, PRX, power up
    SPI_writeArray(write, 2);
    
   
    write[0] = (EN_AA & REGISTER_MASK) | W_MASK;
    write[1] = 0b00000001;
    SPI_writeArray(write, 2); // enable auto-acknowledgement on pipe 0
    
    write[0] = (EN_RX_ADDR & REGISTER_MASK) | W_MASK;
    write[1] = 0b00000001; // enable data pipe 0
    SPI_writeArray(write, 2);
    
    write[0] = (RX_PW_P[0] & REGISTER_MASK) | W_MASK;
    write[1] = 0b00000001;
    SPI_writeArray(write, 2); // 1 byte in RX payload in data pipe 0
    
    write[0] = (SETUP_AW & REGISTER_MASK) | W_MASK;
    write[1] = 0b0000011;
    SPI_writeArray(write, 2);
    
}


uint8_t readData(void) {
    SS_PIN = 0;
    SPI_write_byte(R_RX_PAYLOAD);
    uint8_t ret = SPI_write_byte(0xFF); // dummy
    SS_PIN = 1;
    return ret;
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