/*
 * Serial.h
 *
 * Created: 15. 8. 2016 18:44:16
 *  Author: Kamil
 */ 
#ifndef SERIAL_H_
#define SERIAL_H_
void SerialInit(unsigned long baud);

void SerialAdvancedInit(unsigned long baud);

void SerialEnable(void);

void SerialDisable(void);

void SerialSendChar(char a);

char SerialRecieveChar(void);

void SerialSendDebug(const char * c);

void SerialSendString(const char * c);

void SerialSendDec(long int c);

void SerialSendNumber(uint16_t num, unsigned char system);

char SerialAvailabel(void);

#endif /* SERIAL_H_ */