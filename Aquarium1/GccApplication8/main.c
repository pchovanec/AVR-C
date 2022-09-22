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

#define on				1
#define off				0

volatile int timeCount = 0;
volatile int enableRTC = 1;
volatile int timerOverflow = 0;
unsigned int firstCycle=1;
unsigned int minutes;
unsigned int hours;
unsigned int currentMarker=0;
void watchdogSetup(void);
void sunrise(void);
void morning(void);
void midday(void);
void afternoon(void);
void evening(void);
void sunset(void);
void night(void);
void verifyTime(void);
void (*prog)(void) = sunrise;
void rutine(int timer, int pwm, int init);

struct trigger{
	int H;
	int M;
	int PWM;
	void (*prog)(void);
	}event[7] = {
	{8, 45, on,sunrise},
	{10, 4, off,morning},
	{12, 1, on,midday},
	{16, 38, on,afternoon},
	{17, 10, on,evening},
	{18, 45, on,sunset},
	{20, 10, on,night}
};

int main(void){
	SerialInit(9600);
	MTWIInit(100000);
	ICR1=65055;
	OCR1A = 65055;
	sei();

	while (1){
		_delay_ms(200);
		if(enableRTC){
			verifyTime();
			(*prog)();
		}
		if(event[currentMarker].PWM){
			cli();
			TIMSK1 |=  (1 << OCIE1A); //Pøeteèení èítaèe1 = pøerušení a probuzení
			TCCR1B |= (1 << CS10) | (1 << CS12) | (1 << WGM12) | (1 << WGM13) ; //timer1 jako èasovaè
			sei();
			set_sleep_mode(SLEEP_MODE_IDLE);
			sleep_mode();
			cli();
			TIMSK1 &= ~(1 << OCIE1A);
			TCCR1B &= ~(1 << CS10) | (1 << CS12) | (1 << WGM12) | (1 << WGM13) ;
			sei();
		}
		else{
			(*prog)();
			_delay_ms(200);
			watchdogSetup();
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_mode();
			sleep_disable();
			power_all_enable();
			WDTCSR = 0;
			_delay_ms(300);
		}
	}
}

void verifyTime(){
	hours = RTCGetHours();
	minutes = RTCGetMinutes();
	enableRTC = 0;
	for(int i = 6;i > 0;i--){
		if((hours == event[i].H && minutes >= event[i].M) || hours > event[i].H){
			if(i != currentMarker){
				currentMarker = i;
				firstCycle= 1;
				prog = event[i].prog;
			}
			break;
		}
	}
}

void watchdogSetup(void){
	cli();
	MCUSR &= ~(1<<WDRF);
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR = (1<<WDP0) | (1<<WDP3); // probuzení po 8s
	WDTCSR |= _BV(WDIE);
	sei();
}

ISR(TIMER1_COMPA_vect){
	if(timerOverflow < 3)timerOverflow++;
	else{
		timerOverflow = 0;
		enableRTC = 1;
	}
}

ISR(WDT_vect){
	timeCount++;
	if(timeCount >= 2){
		timeCount=0;
		enableRTC=1;
	}
	else {
		watchdogSetup();
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();
	}
}

void sunrise(){
	if(firstCycle){
		DDRD |= (1 << PIND5);
		PORTD &= ~(1 << PIND6) | (1 << PIND3);
		TCCR0A |= (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);
		TCCR0B |= (1 << CS00);
		firstCycle = 0;
	}
	if(predniOsvetleni<244)predniOsvetleni++;
};

void morning(){
	if(firstCycle){
		TCCR0B = 0;
		TCCR0A = 0;
		DDRD |= (1 << PIND5);
		PORTD |= (1 << PIND5);
		firstCycle = 0;
	}
};

void midday(){
	if(firstCycle){
		DDRD |= (1 << PIND6) | (1 << PIND5);
		PORTD &= ~(1 << PIND6) | (1 << PIND5) | (1 << PIND6);
		TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);
		TCCR0B |= (1 << CS00);
		hlavniOsvetleni = 0;
		predniOsvetleni = 254;
		firstCycle = 0;
	}
	if(predniOsvetleni>60)predniOsvetleni--;
	if(hlavniOsvetleni<254)hlavniOsvetleni++;
};

void afternoon(){
	if(firstCycle){
		DDRD |= (1 << PIND6) | (1 << PIND5);
		PORTD &= ~(1 << PIND6) | (1 << PIND5) | (1 << PIND6);
		predniOsvetleni = 60;
		hlavniOsvetleni = 254;
		TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);
		TCCR0B |= (1 << CS00);
		firstCycle = 0;
	}
	if(predniOsvetleni>0)predniOsvetleni--;
	PORTD |= (1 << PIND6);
};

void evening(){
	if(firstCycle){
		DDRD |= (1 << PIND6) | (1 << PIND3);
		DDRD &= ~ (1 << PIND5);
		PORTD &= ~(1 << PIND6) | (1 << PIND5) | (1 << PIND3);
		TCCR2A |= (1 << COM2B1) | (1 << WGM20) | (1 << WGM21);
		TCCR2B |= (1 << CS20);
		TCCR0A |= (1 << COM0A1) | (1 << WGM00) | (1 << WGM01);
		TCCR0A &= ~ (1 << COM0B1);
		TCCR0B |= (1 << CS00);
		OCR0A = 254;
		OCR2B = 0;
		firstCycle = 0;
	}
	if(nocniOsvetleni < 80)nocniOsvetleni++;
	if(hlavniOsvetleni > 180)nocniOsvetleni--;
};

void sunset(){
	if(firstCycle){
		DDRD |= (1 << PIND6) | (1 << PIND3);
		DDRD &= ~(1 << PIND5);
		PORTD &= ~(1 << PIND6) | (1 << PIND5) | (1 << PIND3);
		TCCR2A |= (1 << COM2B1) | (1 << WGM20) | (1 << WGM21);
		TCCR2B |= (1 << CS20);
		TCCR0A |= (1 << COM0A1) | (1 << WGM00) | (1 << WGM01);
		TCCR0B |= (1 << CS00);
		OCR0A = 150;
		OCR2B = 50;
		firstCycle = 0;
	}
	if(hlavniOsvetleni>100)hlavniOsvetleni--;
	if(nocniOsvetleni<170)nocniOsvetleni++;
};

void night(){
	if(hlavniOsvetleni>0)hlavniOsvetleni--;
	if(nocniOsvetleni>0)nocniOsvetleni--;
};

