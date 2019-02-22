#pragma once


using ProcessBufferCallback = void(const char *buffer);

class SerialBuffer {
    ProcessBufferCallback *callback;

    static constexpr int MAX_INPUT = 90;

    int s_len = 0; // static variables default to 0
    char inputBuffer[MAX_INPUT + 1]; // Handles up to 90 bytes in a c-style string, with a null character termination.

public:

    SerialBuffer(ProcessBufferCallback *callback);

    void ReadSerialChars();
};
