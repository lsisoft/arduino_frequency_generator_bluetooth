#pragma once

#ifndef DEBUG
#define LOG(...) ;
#else
#define LOG(...) sprintf(outputBuffer, __VA_ARGS__);System.println(outputBuffer);
#endif
