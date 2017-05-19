/* 
 * File:   hc_sr04.h
 * Author: Liam
 *
 * Created on May 17, 2017, 11:30 PM
 */

#ifndef HC_SR04_H
#define	HC_SR04_H

#include <stdint.h>
#include "pins.h"


#ifdef	__cplusplus
extern "C" {
#endif

#define _XTAL_FREQ 16000000UL
#define TMR_THRESH 60000

void configTimer(void);
void startTimer(void);
void pulse(void);
void configUltraSonicIO(void);
void configClock(void);


#ifdef	__cplusplus
}
#endif

#endif	/* HC_SR04_H */

