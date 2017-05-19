/* 
 * File:   pins.h
 * Author: Liam
 *
 * Created on May 18, 2017, 2:07 PM
 */

#ifndef PINS_H
#define	PINS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define TRIGGER LATAbits.LATA4
#define TRIGGER_ENABLE TRISAbits.TRISA4
#define ECHO1 PORTAbits.RA0
#define ECHO1_ENABLE TRISAbits.TRISA0
#define LED_ENABLE TRISAbits.TRISA2
#define LED LATAbits.LATA2


#ifdef	__cplusplus
}
#endif

#endif	/* PINS_H */

