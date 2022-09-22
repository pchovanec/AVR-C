#ifndef MTWI_H_
#define MTWI_H_

#define WRITE 0
#define READ 1
#define ACK 0
#define NACK 1

void MTWIInit(uint32_t freq);

uint8_t MTWIStart(uint8_t addr, uint8_t mode);

uint8_t MTWIWrite(uint8_t data);

uint8_t MTWIRead(uint8_t mode);

void MTWIStop();

uint8_t MTWIReadRegister(uint8_t slave_addr, uint8_t reg_addr);

void MTWIWriteRegister(uint8_t slave_addr, uint8_t reg_addr, uint8_t value);

void MTWISendCommand(uint8_t slave_addr, uint8_t inst);
#endif