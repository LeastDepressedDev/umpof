package me.ldd.umpof.lb;

import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

public class ByteHelper {
    public static ByteBuffer readNumeric(FileInputStream fs, int bytes) throws IOException {
        byte[] vec = fs.readNBytes(bytes);
        for (int i = 0; i < vec.length/2; i++) {
            byte b = vec[i];
            vec[i] = vec[vec.length-i-1];
            vec[vec.length-i-1] = b;
        }
        return ByteBuffer.wrap(vec);
    }

    public static String readSizedStr(FileInputStream fs, int size) throws IOException {
        return new String(fs.readNBytes(size), StandardCharsets.UTF_8);
    }
}
