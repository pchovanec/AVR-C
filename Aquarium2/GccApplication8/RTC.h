#ifndef RTC_H_
#define RTC_H_

#include <avr/io.h>
#include <math.h>
#include "RTC.h"
#include "MTWI.h"

#define ADDRESS 0x68
#define RTCSEC 0x00
#define RTCMIN 0x01
#define RTCHOUR 0x02
#define RTCWKDAY 0x03
#define RTCDATE 0x04
#define RTCMTH 0x05
#define RTCYEAR 0x06
#define CONTROL 0x07
#define OSCTRIM 0x08
#define ALM0SEC 0x0A
#define ALM0MIN 0x0B
#define ALM0HOUR 0x0C
#define ALM0WKDAY 0x0D
#define ALM0DATE 0x0E
#define ALM0MTH 0x0F
#define ALM1SEC 0x11
#define ALM1MIN 0x12
#define ALM1HOUR 0x13
#define ALM1WKDAY 0x14
#define ALM1DATE 0x15
#define ALM1MTH 0x16
#define PWRDNMIN 0x18
#define PWRDNHOUR 0x19
#define PWRDNDATE 0x1A
#define PWRDNMTH 0x1B
#define PWRUPMIN 0x1C
#define PWRUPHOUR 0x1D
#define PWRUPDATE 0x1E
#define PWRUPMTH 0x1F

void RTCInit(); 

uint8_t RTCGetSeconds();

uint8_t RTCGetMinutes(); 

uint8_t RTCGetHours(); 

uint8_t RTCGetDay(); 

uint8_t RTCGetDate();

uint8_t RTCGetMonth(); 

uint8_t RTCGetYear();

void RTCGetAll(uint8_t *storage);

void RTCSetAll(uint8_t *storage);


#endif /* RTC_H_ */