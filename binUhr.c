// TODO: Binäruhr mit Atmega mController realisieren
// Aufbau nach selbstentworfenem Schaltplan mit selbst erstellter Platine

// Binäruhr auf exterenen uhrenquarz als taktgeber 
// Zählen per "Taktinterrupt" ansonsten sleepmode um Energie zu sparen
// Zeitkorrektur nach messung im Labor

// Includes


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

/** TODO: Pin Initalisierung
 * PC0 - PC5 Minuten Ausgang
 * PD0, PD1 und PD5-PD7 Stunden Ausgang
 * PD2-PD4 Buttons Eingang
 * 
*/
void initPorts(){
    DDRC |= 0b00111111;
    DDRD |= 0b11100011;
}

/** TODO: SleepMode einstellen
* Power-save
* TWI können wir auch abschalten
* Watch Dog ausmachen FUSEBIT
*/
void enterSleepMode(){
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    //usw...
}


// TODO: Zeitzählung über Interrupts
    // Interrupt muss bis 60s zählen für Minute -> 6 LED Kette hoch
    // Interrupt muss bei 60min zählen -> 5 LED Kette hoch, 6 LED Kette reset
    // Interrupt muss bei 24h -> 5 LED Kette reset
    // Zeitkorrektur ausführen

    // LEDs über Pulsweitenmodulation maximal 25% Leistung

// TODO: 3 Buttons per Interrupt
    //Funktionen festlegen


// globale volatile Variablen für Steuerung der Interrupts
// bspw. zum Entprellen der Schalter, ...
volatile uint_8 s = 0;
volatile uint_8 min = 0;
volatile uint_8 h = 0;

void main(){

    // Setup erstellen
    initPorts();
    initTimebase();


    sei(); // Interrupts einschalten
    while(1){
        enterSleepMode();
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
        PORTC = (PORTC & 0b11000000) | (min & 0b00111111); 
        PORTD = (PORTD & 0b00011100) | (h & 0b00000011) | ((h & 0b00011100) << 3);
    }
}