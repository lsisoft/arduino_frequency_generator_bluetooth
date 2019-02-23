#pragma once

using ActionCallback = void();

class OnOffButton {
    const int buttonPin;
    ActionCallback *buttonPressedCallback, *buttonDisabled;

    int lastState = 0;

    unsigned long lastTime = UINT32_MAX;

    int countFastChanges = 0;

    bool enabled = true;

public:

    OnOffButton(int buttonPin, ActionCallback *buttonPressedCallback, ActionCallback *buttonDisabled)
            : buttonPin(buttonPin), buttonPressedCallback(buttonPressedCallback), buttonDisabled(buttonDisabled) {
        pinMode(buttonPin, INPUT);

        lastTime = micros();
    }

    void CheckButton() {
        if (!enabled) return;

        int buttonState = digitalRead(buttonPin);

        if (buttonState != lastState) {
            unsigned long currentTime = micros();
            long delta = currentTime - lastTime;
            lastTime = currentTime;

            if (delta < 1e5) {
                LogDebug("onoff fast delta %ld", delta);
                ++countFastChanges;

                if (countFastChanges > 10) {
                    LogDebug("onoff disabled, pull up resistor missing");
                    enabled = false;
                    buttonDisabled();
                }
            }

            lastState = buttonState;

            if (buttonState == LOW)
                buttonPressedCallback();
        }
    }
};