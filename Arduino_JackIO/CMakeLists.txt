cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME Arduino_JackIO)
project(${PROJECT_NAME})

set(${CMAKE_PROJECT_NAME}_SKETCH Arduino_JackIO.ino)
set(${CMAKE_PROJECT_NAME}_BOARD atmega8)
set(${TARGET_NAME}_PROGRAMMER arduinoisp)
generate_arduino_firmware(${CMAKE_PROJECT_NAME})
