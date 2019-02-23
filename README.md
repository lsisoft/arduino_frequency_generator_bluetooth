# arduino_frequency_generator_bluetooth
Arduino frequency generator for (1Hz ... 4Mhz)

All frequencies we can request are integers.

The PWM driving method allows for frequencies of less then 1Hz but this
application serial language is not designed for it.

## Reference
Please see fast_pwm_reference.ino about how the PWM is driven. The reference
shows using 3 PWM 16 bit timers, on Fast PWM, with an attempt of synchronization.

For applications requiring more PWM pins with variable frequency, probably
the whole 6 PWM timers could be used, each driving its own pin.  

## Installation

The project has been developed in Jetbrains CLion using arduino plugin.
The plugin uses the arduino-cmake system (find it as a project on github).

### Points of interest
cmake/ArduinoToolchain.cmake - you will find a if (UNIX) elseif(WIN32)
with some hint paths.
Depending on Linux/Windows (you could add APPLE), add your arduino sdk folder
(the download form arduino.cc) in it.
The folder you add should be the root folder for bin, lib, hardware, examples, etc.

Also, the eeprom image is generated with zero length and that in the template
cmake files from arduino-cmake was generating a problem during upload.
To make it work I've commented a line in cmake/Platform/Arduino.cmake:

`    list(APPEND AVRDUDE_ARGS "-Ueeprom:w:${TARGET_PATH}.eep:i")
`

CMakeLists.txt also contains a port for the Arduino which you might need
to change.

On Jetbrains CLion, there should be a target upload, available as a build
configuration which will do the upload to Arduino.

## Usage

The PWM will be generated on pin 2.
On pin 52 you an add a button, with a pull-up resistor on 5V.
This on off button can be disabled from core.cpp, commenting out `#define __USE_BUTTON__`

The generator works with a relatively verbose system on Serial
(or bluetooth module if connected on RX/TX).

### Serial options:

* on - turn on pwm generation

* off - turn it off
  * button 52 when pressed inverts on/off
  * on or off can also be observed on the onboard LED, equivalent to pip 13

* f100d10s10 - change the generated frequency to 100Hz, duty cycle to 10%
  and apply the change linearly from what it is right now during 10 seconds


# Testing
Using Simutron (with simutron.simu), the arduino board can be simulated
and it will run the textual commands. Simutron does't simulate pwm, so
my attempts to capture have failed.

Simutron also provides GDB on port 1212, which with avr-gdb should be 
possible to debug the code (not yet tested).

