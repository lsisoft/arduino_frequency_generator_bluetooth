cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)

set(CMAKE_CXX_STANDARD 17)
set(PROJECT_NAME t1)

set(${PROJECT_NAME}_BOARD mega)
set(ARDUINO_CPU atmega2560)

project(${PROJECT_NAME})
set(${PROJECT_NAME}_SRCS
        core.cpp
        Logging.h
        Logging.cpp
        FrequencyCalculator.h
        FrequencyCalculator.cpp
        SerialBuffer.h
        SerialBuffer.cpp
        OnOffButton.h
        PwmChanger.h
        )

# set(${PROJECT_NAME}_HDRS)
set(${PROJECT_NAME}_LIBS libscanf_flt.a)
# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
# add_subdirectory()
#set(${PROJECT_NAME}_SKETCH t1.ino)

set(${PROJECT_NAME}_PROGRAMMER avrdude)

message(DEBUG system_${CMAKE_HOST_SYSTEM_NAME}_)

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    message(DEBUG Linux)
    set(${PROJECT_NAME}_PORT /dev/ttyACM0)
elseif (${CMAKE_HOST_SYSTEM_NAME} EQUAL Windows)
    message(DEBUG Windows)
    set(${PROJECT_NAME}_PORT COM6)
else ()
    message(DEBUG platform default)
    set(${PROJECT_NAME}_PORT COM6)
endif ()

message(DEBUG ${${PROJECT_NAME}_PORT})

set(${PROJECT_NAME}_AFLAGS -v)
set(mega.upload.speed 115200)

generate_arduino_firmware(${PROJECT_NAME})
