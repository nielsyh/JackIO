package com.company;

/**
 * Created by lars on 21-10-15.
 */
public class AudioEncoder {

    private static int bitFrame = 32;

    private static short[] high = new short[bitFrame];
    private static short[] low = new short[bitFrame];

    static {
        for (int i = 0; i < bitFrame; i++) {
            high[i] = (short) (Short.MAX_VALUE * (-Math.sin(Math.PI * i / bitFrame * 2)));
            low[i] = (short) (Short.MAX_VALUE * (Math.sin(Math.PI * i / bitFrame * 2)));
        }
    }

    private AudioEncoder() {

    }

    public static short[] encode(Packet packet) {
        int packetSize = Packet.EMPTY_SIZE + (packet.getLength() * 8);
        short[] audio = new short[bitFrame * packetSize];
        byte[] bytes = packet.toBytes();

        for(int i = 0; i < packetSize; i++){
            int pos = i / 8;
            int bitPos = i % 8;

            System.out.println(bitPos + " bitPos " + pos + );

            if(((bytes[pos] << bitPos) & 0x80) == 0x80){
                System.arraycopy(high, 0, audio, i * bitFrame, bitFrame);
                System.out.println(audio[i]+ "  high");
            } else {
                System.arraycopy(low, 0, audio, i * bitFrame, bitFrame);
                System.out.println(audio[i]+ "  low");
            }
        }

        return audio;
    }
}
