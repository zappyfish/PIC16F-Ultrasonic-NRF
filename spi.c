

#include "spi.h"
#include <xc.h>
#include "NRF_consts.h"

/* I put these here so it was easier for me to set the TRIS registers
#define SS_PIN LATCbits.LATC3 // this is NRF_CSN
#define SCK LATCbits.LATC0
#define SDO LATCbits.LATC2
#define SDI PORTCbits.RC1
#define SWITCH PORTAbits.RA5 // input
#define LED LATAbits.LATA1
#define NRF_CE LATCbits.LATC4 // chip enable activates RX or TX mode
#define IRQ PORTAbits.RA2 // interrupt request, goes low when valid address

 */

void configIO(void) {
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC1 = 1;
    TRISAbits.TRISA5 = 1;
    TRISAbits.TRISA1 = 0;
    TRISCbits.TRISC4 = 0;
    TRISAbits.TRISA2 = 1;
    OPTION_REGbits.nWPUEN = 0; // enable pull ups
    WPUAbits.WPUA5 = 1; // pull up for switch
    WPUAbits.WPUA2 = 1; // pull up for IRQ
    APFCON = 0b00010000; // sdo on rc2
    ANSELA = 0x00;
    ANSELC = 0x00;
}
void SPI_init(void) {
    SSP1CON1bits.SSPEN = 0;
    configIO();
    SSP1CON1bits.CKP = 0; // clock polarity low
    SSP1STATbits.CKE = 1; // transmit high to low
    SSP1STATbits.SMP = 0; // input data sampled at end of data output time
    SSP1CON1bits.SSPM = 0b0000; // SPI master mode, SCK = FOSC/4
    PIR1bits.SSP1IF = 0;
    SSP1CON1bits.SSPEN = 1; // enable spi
}

uint8_t SPI_write_byte(uint8_t data) {
    SSP1BUF = data;
    while(!PIR1bits.SSP1IF);
    uint8_t ret_data = SSP1BUF;
    PIR1bits.SSP1IF = 0;
    return ret_data;
}

uint8_t SPI_writeArray(uint8_t *data, uint8_t length) {
    SS_PIN = 0;
    uint8_t ret;
    for(uint8_t i = 0; i < length; ++i) {
        ret = SPI_write_byte(data[i]);
    }
    
    SS_PIN = 1;
    return ret; // return the last SPI command
}

uint8_t SPI_read_byte(uint8_t address) {
    SS_PIN = 0;
    SPI_write_byte((address & REGISTER_MASK) | R_MASK);
    uint8_t ret = SPI_write_byte(0xFF); // write dummy byte
    SS_PIN = 1;
    return ret;
}

void resetIRQ(void) {
    uint8_t write[2];
    write[0] = (0x07 & REGISTER_MASK) | W_MASK;
    write[1] = 0b01110000;
    SPI_writeArray(write, 2);
}
