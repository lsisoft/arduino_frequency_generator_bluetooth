void setup() {
    //timer 3: pins 5, 3, 2 ; for fast PWM just 2 works
    pinMode(2, OUTPUT);  analogWrite(2, 1);

    TCCR3A = _BV(COM3B1) | _BV(WGM31) | _BV(WGM30);
    TCCR3B = _BV(WGM33) | _BV(CS00); // 8 divisor
    OCR3A = 400;
    OCR3B = 50;

    //timer4: 8,7,6 ; for fast PWM 7 works
    pinMode(7, OUTPUT); analogWrite(7, 1);

    TCCR4A = _BV(COM4B1) | _BV(WGM41) | _BV(WGM40);
    TCCR4B = _BV(WGM43) | _BV(CS00);
    OCR4A = 300;
    OCR4B = 50;

    //timer 5: 46,45,44 ; for fast PWM 45 works
    pinMode(45, OUTPUT); analogWrite(45, 1);

    TCCR5A = _BV(COM5B1) | _BV(WGM51) | _BV(WGM50);
    TCCR5B = _BV(WGM53) | _BV(CS00);
    OCR5A = 300;
    OCR5B = 50;

    // attempt to synchronize the timers - this works
    TCNT3 = 0;  TCNT4 = 0;  TCNT5 = 0;

    // or add offsets between them - this doesn't work as expected
    TCNT3 = 0;  TCNT4 = 100;  TCNT5 = 200;
}

void loop() {}