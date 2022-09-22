
#ifndef BH1750_H_
#define BH1750_H_

#define	 F_CPU 16000000
#include <avr/io.h>
#include "MTWI.h"

#define ADRESS 0x23
#define TURNOFF 0x00
#define TURNON	0x01
#define CHRM	0x10
#define CHRM2	0x11
#define CLRM	0x13
#define OTHRM	0x01
#define OTHRM2	0x10
#define OTLRM	0x07

uint16_t BH1750measure();
void BH1750init();


#endif 