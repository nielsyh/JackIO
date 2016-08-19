package com.company;

import sun.jvm.hotspot.utilities.BitMap;
import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

public class Main {


    public static void main(String[] args) throws IOException {
        Packet packet = Packet.build((byte) 0b10101010, new byte[]{0b01010101});
        short[] audio = AudioEncoder.encode(packet);

        BufferedImage image = new BufferedImage(audio.length, 64, BufferedImage.TYPE_INT_ARGB);
        Graphics2D g2 = image.createGraphics();
        g2.translate(0, 32);

        for(int i = 0; i < audio.length; i++) {
            int y = (int) (((32.f) / Short.MAX_VALUE) * audio[i]);
            if(i % 32 == 0){
                g2.setColor(Color.BLUE);
                g2.drawLine(i, -32, i, 32);
            }
            g2.setColor(Color.RED);
            g2.drawLine(i, y, i, y);
        }
        ImageIO.write(image, "PNG", new File("image.png"));
    }
}
