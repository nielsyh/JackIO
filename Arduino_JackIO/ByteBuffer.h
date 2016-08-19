//
// Created by Lars Werkman on 23-12-15.
//
#include "stdio.h"

#ifndef ARDUINO_JACKIO_BYTEBUFFER_H
#define ARDUINO_JACKIO_BYTEBUFFER_H

class ByteBuffer {
private:
    size_t  len;
    uint8_t pos;
    uint8_t  *bytes;

public:
    ByteBuffer(size_t len);

    uint8_t get(uint8_t pos);
    uint8_t* get();
    void put(uint8_t byte);
    void clear();
    size_t length();
    uint8_t position();
};


#endif //ARDUINO_JACKIO_BYTEBUFFER_H
