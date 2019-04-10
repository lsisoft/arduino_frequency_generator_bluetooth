#include <Arduino.h>

#define __DEBUG__
//#define __USE_BUTTON__

//#define LIMIT_DUTY_BY_FREQ(duty, freq) if (freq<1000 && duty>15) duty=15;
#define LIMIT_DUTY_BY_FREQ(duty, freq) ;

#include "Logging.h"
#include "FrequencyCalculator.h"
#include "SerialBuffer.h"
#include "OnOffButton.h"
#include "PwmChanger.h"

bool active = false;

double currentFreq = 20;
double currentDuty = .01;


double targetFreq = currentFreq;
double targetDuty = currentDuty;
double speedFreq = 0;
double speedDuty = 0;

FrequencyCalculator frequencyCalculator;
FrequencyCalculator::RegCountDuty currentRegs{0, 0, 0};

void processBuffer(const char *buffer);

SerialBuffer serialBuffer(processBuffer);
PwmChanger pwmChanger;

#ifdef __USE_BUTTON__

void ButtonPressed() {
    active = !active;
    logStatus();
}

void ButtonDisabled() {
    active = false;
    logStatus();
}
OnOffButton onOffButton(52, ButtonPressed, ButtonDisabled);
#endif


void logStatus() {
    sprintf(Logger::outputBuffer, "%s f %lu d %d count_reg %u duty_reg %u scaler %u gen_freq %lu (.1khz)",
            active ? "ON " : "OFF",
            (unsigned long) currentFreq, (int) (100 * currentDuty),
            currentRegs.count_reg, currentRegs.duty_reg, currentRegs.scaler,
            (unsigned long) (100.*currentRegs.gen_frequency)
            );

    Serial.println(Logger::outputBuffer);

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

        currentRegs = frequencyCalculator.frequency(currentFreq, currentDuty);
        pwmChanger.ApplyRegs(currentRegs);
        logStatus();
        return;
    }

    if (!strcmp(buffer, "off")) {
        active = false;

        currentRegs.duty_reg = 0;
        pwmChanger.ApplyRegs(currentRegs);
        logStatus();
        return;
    }

    if (sscanf(buffer, "f%lud%us%u", &freq, &duty, &speedSeconds) != 3) {
        // not debug LogDebug

        sprintf(Logger::outputBuffer, "Expecting f30000d10s30, received %s", buffer);
        Serial.println(Logger::outputBuffer);
        return;
    } else {
        LogDebug("sscanf: f%lu d%u s%u", freq, duty, speedSeconds);
    }

    LIMIT_DUTY_BY_FREQ(duty, freq);

    targetFreq = freq;
    targetDuty = duty / 100.;
    speedSeconds = max(1, speedSeconds);

    speedFreq = (targetFreq - currentFreq) / (double) speedSeconds;
    speedDuty = (targetDuty - currentDuty) / (double) speedSeconds;

    LogDebug("debug: f%lu d%d tf%lu td%d sf%ld sd%ldm",
             (unsigned long) currentFreq, (int) (currentDuty * 100),
             (unsigned long) targetFreq, (int) (targetDuty * 100),
             (long) speedFreq, (long) (speedDuty * 1000)
    );
}

void setup() {
    Serial.begin(9600);

    pinMode(LED_BUILTIN, OUTPUT);

    currentRegs = frequencyCalculator.frequency(currentFreq, currentDuty);

    Serial.println("INIT");

    if (!active)
        currentRegs.duty_reg = 0;
    logStatus();
    pwmChanger.ApplyRegs(currentRegs);
}

unsigned long lastTime = UINT32_MAX;

void loop() {
#ifdef __USE_BUTTON__
    onOffButton.CheckButton();
#endif

    serialBuffer.ReadSerialChars();

    unsigned long currentTime = micros();

    if (currentTime < lastTime) { // after ~50 days there is a reset in micros
        LogDebug("CT%lu LT%lu CT<LT", currentTime, lastTime);

        lastTime = currentTime;

        return;
    }

    const double sliceTime = 20e3;

    if (currentTime - lastTime >= sliceTime) // 100 millisecond
    {
        bool change = false;
        if (active) {

            double deltaTime = min(currentTime - lastTime, 2 * sliceTime);// min to prevent errors
            lastTime = currentTime;
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

                logStatus();
                pwmChanger.ApplyRegs(currentRegs);
            }
        }
    }
}
