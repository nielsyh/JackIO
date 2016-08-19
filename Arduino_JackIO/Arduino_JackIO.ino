#include <Arduino.h>
#include "ByteBuffer.h"
#include "Packet.h"

#define elif else if

#define INPUT_CHANNEL A0

#define MAX_BYTES 11

#define MIN ((1024 / 2) - 50)
#define MAX ((1024 / 2) + 50)

#define SEARCH_FLAG 0
#define START_FOUND_FLAG 1
#define CONTENT_LENGTH_FOUND_FLAG 2
#define CONTENT_FOUND_FLAG 3
#define END_FOUND_FLAG 4

#define COMMAND_WRITE 0
#define COMMAND_READ 1

#define ANALOG 4
#define DIGITAL 13

#define BIT_FRAME 32

int high[BIT_FRAME] = { 0 };
int low[BIT_FRAME] = { 0 };

void setup() {
    Serial.begin(9600);

    pinMode(A1, OUTPUT);
    analogReference(EXTERNAL);

    for(int i = 0; i < BIT_FRAME; i++){
        high[i] = 128 + (128 * (-sin(PI * i / BIT_FRAME * 2)));
        low[i] = 128 + (128 * (sin(PI * i / BIT_FRAME * 2)));
    }
    
    OCR1A = 15624;
    TCCR1B |= (1 << WGM12);
    TIMSK |= (1 << OCIE1A);
    TCCR1B |= (1 << CS10);
    
    sei();
    
    //digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
}

Packet *packet;
boolean hasPacketBool = false;

void loop() {
    while (hasPacket()) {
        int command = (packet->getHeader() & 0x3);
        int id = (packet->getHeader() >> 3);
        int pinID = id > DIGITAL ? id + 1 : id;
        
        Serial.print("Commando: ");
        Serial.println(command);
        
        Serial.print("Pin ID: ");
        Serial.println(pinID);

        switch (command){
            case COMMAND_WRITE:
                pinMode(pinID, OUTPUT);

                if(id <= DIGITAL) {
                    digitalWrite(pinID, packet->getContent()[0]);
                } else {
                    analogWrite(pinID, bytesToInt(packet->getContent()));
                }
                break;
            case COMMAND_READ:
                pinMode(pinID, INPUT);

                Packet* rPacket;
                if(id <= DIGITAL) {
                    rPacket = Packet::build(packet->getHeader(), digitalIntToBytes(digitalRead(pinID)));
                } else {
                    rPacket = Packet::build(packet->getHeader(), analogIntToBytes(analogRead(pinID)));
                }

                Serial.println("Sending Reply.");
                sendPacket(rPacket);
                break;
        }
    }
}

int audio;

byte pos = 0;
byte data = 0;

byte last = 255;
byte current = 0;
byte lastVector = 255;
byte currentVector = 255;
byte vectorChanges = 0;

boolean ghost = false;
byte found = false;

byte state = SEARCH_FLAG;

byte contentLength = 0;
ByteBuffer *buffer = new ByteBuffer(MAX_BYTES);

ISR(TIMER1_COMPA_vect) {
    short lastSample = audio;
    audio = analogRead(INPUT_CHANNEL);

    if (audio > MAX) {
        current = 1;
    } elif (audio < MIN) {
        current = 0;
    }
    
    if(found && audio < MAX && audio > MIN){
      found = false;
      last = 255;
      current = 0;
    }

    if (last != 255 && last != current && !ghost) {
        if (current == 0) {
            data = (byte) (data << 1);
        } else {
            data = (byte) ((data << 1) | 0x01);
        }

        switch (state) {
            case SEARCH_FLAG:
                if ((data & 0xF0) == 0xE0) {
                    state = START_FOUND_FLAG;
                    Serial.println("START_FOUND");
                }
                break;
            case START_FOUND_FLAG:
                if (buffer->position() == 2) {
                    contentLength = ((buffer->get(1) & 0x0F) + 1);
                    state = CONTENT_LENGTH_FOUND_FLAG;
                    Serial.println("CONTENT_LENGTH_FOUND");
                }
                break;
            case CONTENT_LENGTH_FOUND_FLAG:
                if (buffer->position() == contentLength + 2) {
                    state = CONTENT_FOUND_FLAG;
                    Serial.println("CONTENT_FOUND");
                }
                break;
            case CONTENT_FOUND_FLAG:
                if ((data & 0x0F) == 0x0F) {
                    state = END_FOUND_FLAG;
                    data = (byte) (data << 4);
                    pos = 7;
                    Serial.println("END_FOUND");
                }
                break;
        }

        lastVector = 255;
        vectorChanges = 0;
        ghost = true;
        
        pos++;
        if (pos == 8) {
            if (state != SEARCH_FLAG) {
                //Serial.println(data, BIN);
                buffer->put(data);
            }
            if (state == END_FOUND_FLAG) {
                packet = Packet::descript(buffer->get());
                buffer->clear();
                
                hasPacketBool = true;
                found = true;
                state = SEARCH_FLAG;
            }
            pos = 0;
            data = 0;
        }
    } elif (ghost) {

        if (last != current) {
            ghost = false;
        } else {
            currentVector = lastSample < audio ? 1 : 0;

            if (currentVector != lastVector) {
                vectorChanges++;
            }

            if (vectorChanges >= 3) {
                ghost = false;
            }
            lastVector = currentVector;
        }
    }
    last = current;
}

boolean hasPacket() {
    if(hasPacketBool){
        hasPacketBool = false;
        return true;
    }
    return false;
}

void sendPacket(Packet *packet) {
    byte packetSize = Packet::EMPTY_SIZE + (packet->getLength() * 8);
    byte *bytes = packet->toBytes();

    for(int i = 0; i < packetSize; i++){
        byte pos = i / 8;
        byte bitPos = i % 8;

        Serial.print("Sending: ");
        Serial.println(bytes[pos], BIN);

        if(((bytes[pos] << bitPos) & 0x80) == 0x80){
            for(int j = 0; j < BIT_FRAME; j++){
                Serial.println(high[j]);
                analogWrite(A1, high[j]);
                
                long current = millis();
                while(true){
                  if(millis() - current > 10){
                    break;
                  }
                }
            }
        } else {
            for(int j = 0; j < BIT_FRAME; j++){
                Serial.println(low[j]);
                analogWrite(A1, low[j]);
                
                long current = millis();
                while(true){
                  if(millis() - current > 10){
                    break;
                  }
                }
            }
        }
    }
    analogWrite(A1, 255 / 2);
    free(bytes);
}

byte* digitalIntToBytes(int value){
    byte* bytes = (byte *) malloc(1);
    bytes[0] = (byte) value;
    return bytes;
}

byte* analogIntToBytes(int value){
    byte* bytes = (byte *) malloc(2);
    bytes[0] = (byte) (value >> 8);
    bytes[1] = (byte) (value & 0xff);
    return bytes;
}

int bytesToInt(byte* bytes){
    int integer = (int) bytes[0] << 8;
    return (integer | bytes[1]);
}
