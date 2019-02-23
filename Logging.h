#pragma once

#define __DEBUG__

#include <HardwareSerial.h>

struct Logger
{
    static char outputBuffer[200];
};

#ifdef __DEBUG__
#define LogDebug(...) sprintf(Logger::outputBuffer, __VA_ARGS__);Serial.println(Logger::outputBuffer);
#else
#define LogDebug(...) ;
#endif

#define LogInfo(...) sprintf(Logger::outputBuffer, __VA_ARGS__);Serial.println(Logger::outputBuffer);