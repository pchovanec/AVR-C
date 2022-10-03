#define F_CPU 16000000

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void SerialInit(unsigned long baud) {
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);   // Zapnutí serial rx a tx
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); // 8bitový formát s 1 stop-bitem
	UBRR0H = ((((F_CPU / 16 / baud)) - 1) >> 8); // Zapsání UBBR do horn?jšího registru
	UBRR0L = (((F_CPU / 16 / baud)) - 1); // Zapsání UBBR do spodn?jšího registru
}

void SerialEnable(void) {
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);	// Zapnutí serial rx a tx
}

void SerialDisable(void) {
	UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0));	// Vypnutí serial rx a tx
}

void SerialSendChar(char a) {
	while (!(UCSR0A & (1 << UDRE0))) {}	// ?ekání, než bude registr na data volný
	UDR0 = a;	// Zapsání charu do data registru
}

char SerialRecieveChar(void) {
	while (!(UCSR0A & (1 << RXC0))) {}	// ?ekání, než bude registr na data zapln?ný
	return UDR0;	// Funkce vrátí to, co je v registru na data
}

void SerialSendString(const char * c) {
	for (uint32_t i=0;i<strlen(c);i++)	//Cyklus na rozd?lení stringu do char?
	SerialSendChar(c[i]);	// Posílání jednotlivých char?
}

void SerialSendDec(long int c) {
	char b[20];
	sprintf(b, "%ld", c);
	SerialSendString(b);
}

void SerialSendNumber(uint16_t num, unsigned char system) { //do "system" psát pouze: 2-binární soustava=BIN, 8-osmi?ková=OCT, 10-dekadická=default, 16-hexadecimální=HEX
	char buffer[20];
	itoa(num,buffer,system);
	SerialSendString(buffer);
	//SerialSendString("\r\n");
}

char SerialAvailable() {
	if (UCSR0A & (1 << RXC0)) return 1;	// Je registr volný?
	else return 0;	// Není.
}
