#include <HardwareSerial.h>

#include "SerialBuffer.h"

#include "Logging.h"

SerialBuffer::SerialBuffer(ProcessBufferCallback *callback) : callback(callback) {
    inputBuffer[0] = 0;
}

void SerialBuffer::ReadSerialChars() {
    while (Serial.available() > 0) {
        char input = Serial.read();

        if (s_len >= MAX_INPUT) {
            // Have received already the maximum number of characters
            // Ignore all new input until line termination occurs
        } else if (input != '\n' && input != '\r')
            inputBuffer[s_len++] = input;
        else if (s_len > 0) { // ignore new line or \r as first caracter of new line
            inputBuffer[s_len] = 0;

//            LogDebug("RECEIVED MSG: %s", inputBuffer);

            callback(inputBuffer);

            s_len = 0;
            inputBuffer[0] = 0;
        }
    }
}
