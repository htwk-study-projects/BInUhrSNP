// TODO: Binäruhr mit Atmega mController realisieren
// Aufbau nach selbstentworfenem Schaltplan mit selbst erstellter Platine

// Binäruhr auf exterenen uhrenquarz als taktgeber 
// Zählen per "Taktinterrupt" ansonsten sleepmode um Energie zu sparen
// Zeitkorrektur nach messung im Labor

// Includes


// TODO: Zeitbasis generieren
    // Uhrenquarz = 32.768Hz
    // Prescaler und Timerwidth = 128 und 256
    // damit bekommt man Zeitbasis 1s

void initTimebase(){
    ASSR |= (1 << AS2);

    TIMSK2 |= (1 << TOIE2); // Overflow Interrupt, 8Bit register -> 256
    TCCR2B = (1 << CS22) | (1 << CS20);  // Prescaler 128

    TCCR2A = 0; // normaler modus, kein CTC, kein Compare

}

// TODO: SleepMode einstellen
    // Power-save
    // TWI können wir auch abschalten
    // Watch Dog ausmachen FUSEBIT
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

void main(){

    // Setup erstellen
    initTimebase();


    sei(); // Interrupts einschalten
    while(1){
        enterSleepMode();
    }
}

// TODO: Interrupts

// Timer2 Interrupt
ISR(TIMER2_OVF_vect){
    //Zeit zählen, register davon abhängig ändern
}