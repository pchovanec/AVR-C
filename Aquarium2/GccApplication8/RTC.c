#include "RTC.h"
#include "MTWI.h"

void RTCInit() {
	uint8_t sec, hr;
	sec = MTWIReadRegister(ADDRESS, RTCSEC);
	hr = MTWIReadRegister(ADDRESS, RTCHOUR); 
	MTWIWriteRegister(ADDRESS, RTCSEC, (sec |= 0b10000000)); //zapnuti
	MTWIWriteRegister(ADDRESS, RTCHOUR, (hr &= ~(0b01000000))); //nastaveni 24h formatu
	MTWIWriteRegister(ADDRESS, CONTROL, 0);
}

uint8_t RTCGetSeconds() {
	uint8_t sec = MTWIReadRegister(ADDRESS, RTCSEC);
	return ((sec&0b00001111)+((sec&0b01110000)>>4)*10);
}

uint8_t RTCGetMinutes() {
	uint8_t min = MTWIReadRegister(ADDRESS, RTCMIN);
	return ((min&0b00001111)+((min&0b01110000)>>4)*10);
}

uint8_t RTCGetHours() {
	uint8_t hr = MTWIReadRegister(ADDRESS, RTCHOUR);
	return ((hr&0b00001111)+((hr&0b00110000)>>4)*10);
}

uint8_t RTCGetDay() {
	uint8_t day = MTWIReadRegister(ADDRESS, RTCWKDAY);
	return (day&0b00000111);
}

uint8_t RTCGetDate() {
	uint8_t dat = MTWIReadRegister(ADDRESS, RTCDATE);
	return ((dat&0b00001111)+((dat&0b00110000)>>4)*10);
}

uint8_t RTCGetMonth() {
	uint8_t mon = MTWIReadRegister(ADDRESS, RTCMTH);
	return ((mon&0b00001111)+((mon&0b00010000)>>4)*10);
}

uint8_t RTCGetYear() {
	uint8_t yer = MTWIReadRegister(ADDRESS, RTCYEAR);
	return ((yer&0b00001111)+((yer&0b11110000)>>4)*10);
}

void RTCGetAll(uint8_t *storage) {//0-Rok, 1-Mesic, 2-Datum, 3-Den, 4-Hodiny, 5-Minuty, 6-Sekundy
	*storage=RTCGetYear();
	*(storage+1)=RTCGetMonth();
	*(storage+2)=RTCGetDate();
	*(storage+3)=RTCGetDay();
	*(storage+4)=RTCGetHours();
	*(storage+5)=RTCGetMinutes();
	*(storage+6)=RTCGetSeconds();
}

void RTCSetAll(uint8_t *storage) {//0-Rok, 1-Mesic, 2-Datum, 3-Den, 4-Hodiny, 5-Minuty, 6-Sekundy
	uint8_t reg=0, ten, one;
	//year
	ten = floor((*storage)/10);
	one = *storage-(ten*10);
	reg |= (ten<<4) | (one);
	MTWIWriteRegister(ADDRESS, RTCYEAR, reg);
	
	//month
	ten = floor((*(storage+1))/10);
	one = (*(storage+1))-(ten*10);
	reg = MTWIReadRegister(ADDRESS, RTCMTH);
	reg &= ~(0b00011111);
	reg |= (ten<<4) | (one);
	MTWIWriteRegister(ADDRESS, RTCMTH, reg);
	
	//date
	ten = floor((*(storage+2))/10);
	one = (*(storage+2))-(ten*10);
	reg = MTWIReadRegister(ADDRESS, RTCDATE);
	reg &= ~(0b00111111);
	reg |= (ten<<4) | (one);
	MTWIWriteRegister(ADDRESS, RTCDATE, reg);
	
	//day
	ten = floor((*(storage+3))/10);
	one = (*(storage+3))-(ten*10);
	reg = MTWIReadRegister(ADDRESS, RTCWKDAY);
	reg &= ~(0b00000111);
	reg |= (ten<<4) | (one);
	MTWIWriteRegister(ADDRESS, RTCWKDAY, reg);
	
	//hours
	ten = floor((*(storage+4))/10);
	one = (*(storage+4))-(ten*10);
	reg = MTWIReadRegister(ADDRESS, RTCHOUR);
	reg &= ~(0b00111111);
	reg |= (ten<<4) | (one);
	MTWIWriteRegister(ADDRESS, RTCHOUR, reg);
	
	//minutes
	ten = floor((*(storage+5))/10);
	one = (*(storage+5))-(ten*10);
	reg = MTWIReadRegister(ADDRESS, RTCMIN);
	reg &= ~(0b01111111);
	reg |= (ten<<4) | (one);
	MTWIWriteRegister(ADDRESS, RTCMIN, reg);
	
	//seconds
	ten = floor((*(storage+6))/10);
	one = (*(storage+6))-(ten*10);
	reg = MTWIReadRegister(ADDRESS, RTCSEC);
	reg &= ~(0b01111111);
	reg |= (ten<<4) | (one);
	MTWIWriteRegister(ADDRESS, RTCSEC, reg);
}
