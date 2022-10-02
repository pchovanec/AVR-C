#define	 F_CPU 16000000
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "BH1750.h"
#include "MTWI.h"
#include "RTC.h"
#include "Serial.h"

#define hlavniOsvetleni	OCR0A
#define predniOsvetleni	OCR0B
#define nocniOsvetleni	OCR2B

volatile int timeCount = 0;
volatile int enableRTC = 1;
volatile int timerOverflow = 0;
unsigned int minutes;
unsigned int hours;
int mainLight;
int frontLight;
int blueLight;
void watchdogSetup(void);
void ligtingRutine(void);
void timerControl(volatile uint8_t *timer, uint8_t init);
void initTimer(int i);

struct trigger{
	uint8_t H;
	uint8_t M;
	uint8_t limit;
	volatile uint8_t *timer; //PWM 
	}event[] = {
	{9, 45, 200, &hlavniOsvetleni},
	{9, 55, 200, &predniOsvetleni},
	{12, 0, 250, &hlavniOsvetleni},
	{12, 0, 220, &predniOsvetleni},
	{13, 30, 200, &hlavniOsvetleni},
	{13, 30, 0, &predniOsvetleni},
	{16, 55, 120, &hlavniOsvetleni},
	{16, 55, 200, &nocniOsvetleni},
	{19, 5, 0, &hlavniOsvetleni},
	{19, 5, 0, &nocniOsvetleni}
};

int main(void){
	SerialInit(9600);
	MTWIInit(100000);
	ICR1=65055;
	OCR1A = 65055;
	DDRB |= (1 << PINB5);
	TIMSK1 |=  (1 << OCIE1A); //Přetečení čítače1 = přerušení a probuzení
	TCCR1B |= (1 << CS10) | (1 << CS12) | (1 << WGM12) | (1 << WGM13) ;
	sei();
	
	while (1){
		_delay_ms(500);
		if(enableRTC)ligtingRutine();
	}
}

void ligtingRutine(){
	int mainLightIncremented = 0;
	int frontLightIncremented = 0;
	int blueLightIncremented = 0;
	hours = RTCGetHours();
	minutes = RTCGetMinutes();
	enableRTC = 0;

	for(int i = 9;i > 0;i--){	
		if((hours == event[i].H && minutes >= event[i].M) || hours > event[i].H){
		
			//Inicializace PWM pro danou udalost
			if(event[i].timer == &hlavniOsvetleni && !mainLight){
				initTimer(i);
				mainLight = 1;
			}
			else if(event[i].timer == &predniOsvetleni && !frontLight){
				initTimer(i);
				frontLight = 1;
			}
			else if(event[i].timer == &nocniOsvetleni && !blueLight){
				initTimer(i);
				blueLight = 1;
			}
		
			//Pricteni jednicky k jednotlivym citacum	
			if(event[i].timer == &hlavniOsvetleni && !mainLightIncremented){
				if(*event[i].timer < event[i].limit){
					hlavniOsvetleni++;
					mainLightIncremented = 1;
				}
				else if (*event[i].timer > event[i].limit){
					hlavniOsvetleni--;
					mainLightIncremented = 1;
					if(*event[i].timer == 0)initTimer(i);
				}
			}
			
			else if(event[i].timer == &predniOsvetleni && !frontLightIncremented){
				if(*event[i].timer < event[i].limit){
					predniOsvetleni++;
					frontLightIncremented = 1;
				}
				else if (*event[i].timer > event[i].limit){
					predniOsvetleni--;
					frontLightIncremented = 1;
					if(*event[i].timer == 0)initTimer(i);
				}
			}
			
			else if(event[i].timer == &nocniOsvetleni && !blueLightIncremented){
				if(*event[i].timer < event[i].limit){
					nocniOsvetleni++;
					blueLightIncremented = 1;
				}
				else if (*event[i].timer > event[i].limit){
					nocniOsvetleni--;
					blueLightIncremented = 1;
					if(*event[i].timer == 0)initTimer(i);
				}
			}
			
		}
		
	}		
}

void initTimer(int i){
	if(*event[i].timer != event[i].limit){
		timerControl(event[i].timer,1);
	}
	else if(*event[i].timer == event[i].limit){
		timerControl(event[i].timer,0);
	}
}

void timerControl(volatile uint8_t *timer, uint8_t init){
	if (init){
		if(timer == &hlavniOsvetleni){
			DDRD |= (1 << PIND6);
			TCCR0A |= (1 << COM0A1) | (1 << WGM00) | (1 << WGM01);
			TCCR0B |= (1 << CS00);
		}
		else if(timer == &predniOsvetleni){
			DDRD |= (1 << PIND5);
			TCCR0A |= (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);
			TCCR0B |= (1 << CS00);
		}
		else if(timer == &nocniOsvetleni){
			DDRD |= (1 << PIND3);
			TCCR2A |= (1 << COM2B1) | (1 << WGM20) | (1 << WGM21);
			TCCR2B |= (1 << CS20);
		}
	}
	else if (!init){
		if(timer == &hlavniOsvetleni){
			DDRD &= ~(1 << PIND6);
			TCCR0A = 0;
			TCCR0B = 0;
		}
		else if(timer == &predniOsvetleni){
			DDRD &= ~(1 << PIND5);
			TCCR0A = 0;
			TCCR0B = 0;
		}
		else if(timer == &nocniOsvetleni){
			DDRD &= ~(1 << PIND3);
			TCCR2A = 0;
			TCCR2B = 0;
		}
	}
}

ISR(TIMER1_COMPA_vect){
	if(timerOverflow < 3)timerOverflow++;
	else{
		timerOverflow = 0;
		enableRTC = 1;
	}
}

