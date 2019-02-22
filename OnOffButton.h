#pragma once

using ButtonPressedCallback = void();

class OnOffButton {
    const int buttonPin;
    ButtonPressedCallback *buttonPressedCallback;

    int lastState = 0;

public:

    OnOffButton(int buttonPin, ButtonPressedCallback *buttonPressedCallback)
            : buttonPin(buttonPin), buttonPressedCallback(buttonPressedCallback) {
        pinMode(buttonPin, INPUT);
    }

    void CheckButton() {
        int buttonState = digitalRead(buttonPin);

        if (buttonState != lastState) {
            lastState = buttonState;

            if (buttonState == LOW)
                buttonPressedCallback();
        }
    }
};