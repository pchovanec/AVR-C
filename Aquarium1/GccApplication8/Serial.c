#define F_CPU 16000000

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void SerialInit(unsigned long baud) {
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);   // Zapnut� serial rx a tx
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); // 8bitov� form�t s 1 stop-bitem
	UBRR0H = ((((F_CPU / 16 / baud)) - 1) >> 8); // Zaps�n� UBBR do horn?j��ho registru
	UBRR0L = (((F_CPU / 16 / baud)) - 1); // Zaps�n� UBBR do spodn?j��ho registru
}

void SerialAdvancedInit(unsigned long baud) {
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);   // Zapnut� serial rx a tx
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); // 8bitov� form�t s 1 stop-bitem
	UBRR0H = ((((F_CPU / 16 / baud)) - 1) >> 8); // Zaps�n� UBBR do horn?j��ho registru
	UBRR0L = (((F_CPU / 16 / baud)) - 1); // Zaps�n� UBBR do spodn?j��ho registru
}

void SerialEnable(void) {
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);	// Zapnut� serial rx a tx
}

void SerialDisable(void) {
	UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0));	// Vypnut� serial rx a tx
}

void SerialSendChar(char a) {
	while (!(UCSR0A & (1 << UDRE0))) {}	// ?ek�n�, ne� bude registr na data voln�
	UDR0 = a;	// Zaps�n� charu do data registru
}

char SerialRecieveChar(void) {
	while (!(UCSR0A & (1 << RXC0))) {}	// ?ek�n�, ne� bude registr na data zapln?n�
	return UDR0;	// Funkce vr�t� to, co je v registru na data
}

void SerialSendString(const char * c) {
	for (uint32_t i=0;i<strlen(c);i++)	//Cyklus na rozd?len� stringu do char?
	SerialSendChar(c[i]);	// Pos�l�n� jednotliv�ch char?
}

void SerialSendDec(long int c) {
	char b[20];
	sprintf(b, "%ld", c);
	SerialSendString(b);
}

void SerialSendNumber(uint16_t num, unsigned char system) { //do "system" ps�t pouze: 2-bin�rn� soustava=BIN, 8-osmi?kov�=OCT, 10-dekadick�=default, 16-hexadecim�ln�=HEX
	char buffer[20];
	itoa(num,buffer,system);
	SerialSendString(buffer);
	//SerialSendString("\r\n");
}

char SerialAvailable() {
	if (UCSR0A & (1 << RXC0)) return 1;	// Je registr voln�?
	else return 0;	// Nen�.
}
