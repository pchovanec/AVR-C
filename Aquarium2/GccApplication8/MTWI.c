#define F_CPU 16000000
#include <avr/io.h>
#include <util/twi.h>
#include "MTWI.h"

void MTWIInit(uint32_t freq) {
	TWBR = ((F_CPU)/(2*freq)-8);
}

//Zahajeni zapisovani: 0-uspech, 1-selhani
uint8_t MTWIStart(uint8_t addr, uint8_t mode) { 
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTA); //poslani start sekvence
	while(!(TWCR & (1<<TWINT))); //cekat na dokonceni startu
	if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START)) return 1; //kontrola stavu
	if (mode==WRITE) addr = (addr<<1);
	if (mode==READ) addr = (addr<<1) | 1;
	TWDR = addr; //zapsani adresy s write modem
	TWCR = (1<<TWINT) | (1<<TWEN); //odeslani
	while(!(TWCR & (1<<TWINT))); //cekat na dokonceni prenosu adresy
	if ((TW_STATUS != TW_MT_SLA_ACK) && (TW_STATUS != TW_MR_SLA_ACK)) return 1; //pokud slave neodpovedel - selhani
	return 0;
}

//Zapis dat: 0-uspech, 1-selhani
uint8_t MTWIWrite(uint8_t data) {
	TWDR = data; //nacteni dat
	TWCR = (1<<TWINT) | (1<<TWEN); //poslani dat
	while(!(TWCR & (1<<TWINT))); //cekat na dokonceni prenosu
	if (TW_STATUS != TW_MT_DATA_ACK) return 1; //pokud slave neodpovedel - selhani
	return 0;
}

uint8_t MTWIRead(uint8_t mode) {
	if (mode==ACK) TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA); //nastaveni posilani ack
	if (mode==NACK) TWCR = (1<<TWINT) | (1<<TWEN); //nenastavovat ack
	while(!(TWCR & (1<<TWINT))); //cekani na dokonceni prenosu
	if (mode==ACK) if (TW_STATUS != TW_MR_DATA_ACK) return 1; //pokud master neodpovedel - selhani
	if (mode==NACK) if (TW_STATUS != TW_MR_DATA_NACK) return 1; //pokud master neodpovedel - selhani
	return TWDR; 
}

void MTWIStop() {
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);//poslani stop sekvence
	while(TWCR & (1<<TWSTO)); //cekat nez se stop sekvence posle
}

uint8_t MTWIReadRegister(uint8_t slave_addr, uint8_t reg_addr) {
	MTWIStart(slave_addr, WRITE);
	MTWIWrite(reg_addr);
	MTWIStop();
	MTWIStart(slave_addr, READ);
	uint8_t response = MTWIRead(NACK);
	MTWIStop();
	return response;
}

void MTWIWriteRegister(uint8_t slave_addr, uint8_t reg_addr, uint8_t value) {
	MTWIStart(slave_addr, WRITE);
	MTWIWrite(reg_addr);
	MTWIWrite(value);
	MTWIStop();
}

void MTWISendCommand(uint8_t slave_addr, uint8_t inst) {
	MTWIStart(slave_addr, WRITE);
	MTWIWrite(inst);
	MTWIStop();
}
