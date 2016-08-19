//
// Created by Lars Werkman on 23-12-15.
//

#include "ByteBuffer.h"

ByteBuffer::ByteBuffer(size_t len) {
    this->len = len;
    this->pos = 0;
    this->bytes = new uint8_t[len];
}

uint8_t* ByteBuffer::get() {
    return bytes;
}

uint8_t ByteBuffer::get(uint8_t pos) {
    return this->bytes[pos];
}

void ByteBuffer::put(uint8_t byte) {
    this->bytes[this->pos] = byte;
    this->pos++;
}

void ByteBuffer::clear() {
    for(uint8_t i = 0; i < this->len; i++){
        this->bytes[i] = 0;
    }
    this->pos = 0;
}

size_t ByteBuffer::length() {
    return this->len;
}

uint8_t ByteBuffer::position() {
    return this->pos;
}