// TODO: Binäruhr mit Atmega mController realisieren
// Aufbau nach selbstentworfenem Schaltplan mit selbst erstellter Platine

// Binäruhr auf exterenen uhrenquarz als taktgeber 
// Zählen per "Taktinterrupt" ansonsten sleepmode um Energie zu sparen
// Zeitkorrektur nach messung im Labor

// Includes
#define F_CPU 1000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

// globale volatile Variablen f�r Steuerung der Interrupts
// bspw. zum Entprellen der Schalter, ...
volatile uint8_t s = 0;
volatile uint8_t min = 0;
volatile uint8_t h = 0;

volatile uint8_t prell = 0;
volatile uint8_t show = 0;
volatile uint8_t brightness = 100;

/** TODO: Zeitbasis generieren
 * Uhrenquarz = 32.768Hz
 * Prescaler und Timerwidth = 128 und 256
 * damit bekommt man Zeitbasis 1s
*/
void initTimebase(){
    ASSR |= (1 << AS2);

    TIMSK2 |= (1 << TOIE2); // Overflow Interrupt, 8Bit register -> 256
    TCCR2B = (1 << CS22) | (1 << CS20);  // Prescaler 128
    TCCR2A = 0; // normaler modus, kein CTC, kein Compare
}

/**
 * LED Register setzen
*/
void updateLEDRegister(){
    PORTC = (PORTC & 0b11000000) | (min & 0b00111111); 
    PORTD = (PORTD & 0b00011100) | ((h & 0b00000001) << 7) | ((h & 0b00000010) << 5) | ((h & 0b00000100) << 3) | ((h & 0b00001000) >> 2) | ((h & 0b000010000) >> 4);
	DDRB |= 0b00000110; 
}

/** TODO: Pin Initalisierung
 * PC0 - PC5 Minuten Ausgang
 * PD0, PD1 und PD5-PD7 Stunden Ausgang
 * PD2-PD4 Buttons Eingang
 * B1 und B2 Ausgang gemeinsamer Ground LEDs
 * 
*/
void initPorts(){
    DDRC |= 0b00111111;
    DDRD |= 0b11100011;
	PORTD |= 0b00011100;
}

/** TODO: PWM für Minuten und Stunden festlegen
 * 
*/
void initPWM(){
    TCCR1B |= (1 << CS10);

    OCR1A = brightness; //Stunden
    OCR1B = brightness; //Minuten

    TCCR1A |= (1 << WGM10);
    TCCR1B |= (1 << WGM12);

    TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << COM1A0) | (1 << COM1B0);
}

/** TODO: SleepMode einstellen
 * Power-save
 * TWI können wir auch abschalten
 * Watch Dog ausmachen FUSEBIT
*/
void initSleepMode(){
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    SMCR |= (1 << SE);
    PRR |= (1 << PRTWI); 
	PRR |= (1 << PRADC); 
    //Watchdog noch aus

}

/** TODO: 3 Buttons per Interrupt
*/
void initPinInterrupts(){
   EIMSK |= (1 << INT0) | (1 << INT1);
   EICRA |= (1 << ISC11) | (1 << ISC01);
   PCICR |= (1 << PCIE2);
   PCMSK2 |= (1 << PCINT20);
}



void main(){
    // Setup erstellen
    initPorts();
    initTimebase();
    initSleepMode();
    initPWM();
    initPinInterrupts();
    sei();
    while(1){
		if(prell > 0){
			updateLEDRegister();
			_delay_ms(10);
		 	prell--;
		}
		else if(show > 0){
    		OCR1A = brightness; //Stunden
    		OCR1B = brightness; //Minuten
            updateLEDRegister();
			_delay_ms(10);
			show--;
		}
		else{
			DDRB &= ~(0b00000110); 
        	sleep_mode();
		}
    }
}

// TODO: Interrupts

// Timer2 Interrupt
ISR(TIMER2_OVF_vect){
    s = (s + 1) % 60;
    if (s == 0){
        min = (min + 1) % 60;
        if(min == 0){
            h = (h + 1) % 24;
        }
    }
}

// Taster 1, INT0, Minuten
ISR(INT0_vect){
	if(prell == 0){
		min = (min + 1) % 60;
		s = 0;
		prell = 30;
	}
	
}

// Taster 2, INT1, Stunden
ISR(INT1_vect){
	if(prell == 0){
		h = (h + 1) % 24;
		s = 0;
		prell = 30;
	}
	
}

// Taster 3, PCINT20, Helligkeit
ISR(PCINT2_vect){
	if(show < 200){
		show = 255;
		brightness = (brightness + 50) % 150;
	}
}
