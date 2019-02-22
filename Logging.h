#pragma once

struct Logger
{
    static char outputBuffer[200];
};

#ifndef DEBUG
#define LogDebug(...) ;
#else
#define LogDebug(...) sprintf(Logger::outputBuffer, __VA_ARGS__);System.println(outputBuffer);
#endif
