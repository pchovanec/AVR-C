
#include "BH1750.h"

void BH1750init(int mode) {
	MTWIStart(ADRESS,WRITE);
	MTWIWrite(mode);
	MTWIStop();
}

uint16_t BH1750measure(){
	uint16_t measureCache = 0;
		MTWIStart(ADRESS,WRITE);
		MTWIWrite(CHRM2);
		MTWIStop();
		MTWIStart(ADRESS,READ);
		measureCache = MTWIRead(ACK);
		measureCache <<= 8;
		measureCache = MTWIRead(NACK);
		MTWIStop();
		measureCache = measureCache/1.2;
		return measureCache;
}



