package com.company;

/**
 * Created by lars on 21-10-15.
 */
public class Packet {

    public static final int EMPTY_SIZE = 20;

    private byte header;
    private byte length;
    private byte[] content;

    private Packet(){
        //Not accessible without use of static methods
    }

    public static Packet build(byte header, byte[] content) {//throws IllegalPacketException {
        if(content.length == 0){
            //throw new IllegalPacketException("Packet content can't be empty");
        }
        if(content.length > 8){
            // new IllegalPacketException("Packet content can't be larger than 8 bytes");
        }

        Packet packet = new Packet();

        packet.header = header;
        packet.content = content;
        packet.length = (byte) content.length;

        return packet;
    }

    public static Packet descript(byte[] data) {//throws IllegalPacketException {
        //Check for frame header and start bit
        if((data[0] & 0xF0) != 0xE0){
            //throw new IllegalPacketException("Packet has a wrong frame header and start bit");
        }

        Packet packet = new Packet();

        packet.header = (byte) ((data[0] << 4) | (data[1] & 0x0F));
        packet.length = (byte) (data[1] & 0x0F);
        packet.content = new byte[packet.length];

        System.arraycopy(data, 2, packet.content, 0, packet.length);

        if((data[packet.length + 2] & 0xF0) != 0xF0){
            //throw new IllegalPacketException("Packet has a wrong frame tail and stop bit");
        }
        return packet;
    }

    public byte getHeader() {
        return header;
    }

    public byte getLength() {
        return length;
    }

    public byte[] getContent() {
        return content;
    }

    public byte[] toBytes(){
        byte[] data = new byte[length + 3];
        //Add frame header and start bit
        data[0] = (byte) 0xE0;

        //Add content header
        data[0] = (byte) (data[0] | ((header & 0xF0) >> 4));
        data[1] = (byte) ((header & 0x0F) << 4);

        //Add content length
        data[1] = (byte) (data[1] | length - 1);

        //Add content
        System.arraycopy(content, 0, data, 2, length);

        //Add stop bit and frame tail
        data[length + 2] = (byte) 0xF0;

        return data;
    }
}