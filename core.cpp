#include <Arduino.h>

#include "Logging.h"
#include "FrequencyCalculator.h"
#include "SerialBuffer.h"
#include "OnOffButton.h"

//#define DEBUG

bool active = false;
double currentFreq = 5;
double currentDuty = .1;
double targetFreq = currentFreq;
double targetDuty = currentDuty;
double speedFreq = 0;
double speedDuty = 0;


FrequencyCalculator frequencyCalculator;
FrequencyCalculator::RegCountDuty currentRegs{0, 0, 0};

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 52;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin

// variables will change:
int lastState = 0;
int ledState = 0;

void logStatus() {
    sprintf(outputBuffer, "%s f %lu d %d count_reg %d duty_reg %d scaler %d",
            active ? "ON " : "OFF",
            (unsigned long) currentFreq, (int) (100 * currentDuty),
            currentRegs.count_reg, currentRegs.duty_reg, currentRegs.scaler);

    Serial.println(outputBuffer);

    if (active)
        digitalWrite(LED_BUILTIN, HIGH);
    else
        digitalWrite(LED_BUILTIN, LOW);
}

void processBuffer(const char *buffer) {
    unsigned long freq;
    unsigned int duty, speedSeconds;

    if (!strcmp(buffer, "on")) {
        active = true;
        logStatus();
        return;
    }

    if (!strcmp(buffer, "off")) {
        active = false;
        logStatus();
        return;
    }

    if (sscanf(buffer, "f%lud%us%u", &freq, &duty, &speedSeconds) != 3) {
        sprintf(outputBuffer, "Expecting f30000d10s30, received %s", buffer);
        Serial.println(outputBuffer);
        return;
    } else { LOG("sscanf: f%lu d%u s%u", freq, duty, speedSeconds);
    }

    if (duty > 15)
        duty = 15;

    targetFreq = freq;
    targetDuty = duty / 100.;
    speedSeconds = max(1, speedSeconds);

    speedFreq = (targetFreq - currentFreq) / (double) speedSeconds;
    speedDuty = (targetDuty - currentDuty) / (double) speedSeconds;

    LOG("debug: f%lu d%d tf%lu td%d sf%ld sd%ldm",
        (unsigned long) currentFreq, (int) (currentDuty * 100),
        (unsigned long) targetFreq, (int) (targetDuty * 100),
        (long) speedFreq, (long) (speedDuty * 1000)
    );
}

void ButtonPressed() {
    active = !active;
    logStatus();
}

SerialBuffer serialBuffer(processBuffer);

OnOffButton onOffButton(52, ButtonPressed);

void setup() {
    Serial.begin(9600);

    frequencyCalculator.frequency(1e6, .10);
    frequencyCalculator.frequency(10, .10);

    pinMode(LED_BUILTIN, OUTPUT);

    currentRegs = frequencyCalculator.frequency(currentFreq, currentDuty);

    Serial.println("INIT");
    logStatus();
}


unsigned long lastTime = UINT32_MAX;

void loop() {
    onOffButton.CheckButton();

    serialBuffer.ReadSerialChars();

    unsigned long currentTime = micros();

    if (currentTime < lastTime) // after ~50 days there is a reset in micros
    { LOG("CT%lu LT%lu CT<LT", currentTime, lastTime);

        lastTime = currentTime;

        return;
    }

    const double sliceTime = 100e3;

    if (currentTime - lastTime >= sliceTime) // 100 millisecond
    {
        bool change = false;
        if (active) {

            double deltaTime = min(currentTime - lastTime, 2 * sliceTime);// min to prevent errors
            deltaTime /= 1e6; //speed is per second

            if (speedFreq != 0) {
                change = true;

                currentFreq += speedFreq * deltaTime;
                if (signbit(targetFreq - currentFreq) != signbit(speedFreq)) {
                    currentFreq = targetFreq;
                    speedFreq = 0;
                }
            }
            if (speedDuty != 0) {
                change = true;

                currentDuty += speedDuty * deltaTime;
                if (signbit(targetDuty - currentDuty) != signbit(speedDuty)) {
                    currentDuty = targetDuty;
                    speedDuty = 0;
                }
            }

            if (change) {
                currentRegs = frequencyCalculator.frequency(currentFreq, currentDuty);

//            char str_freq[6], str_duty[6];
//            dtostrf(currentFreq, 6, 0, str_freq);
//            dtostrf(currentDuty, 1, 2, str_duty);

                logStatus();
            }
        }

        lastTime = currentTime;
    }
}

