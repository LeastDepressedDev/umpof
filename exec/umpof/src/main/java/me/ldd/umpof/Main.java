package me.ldd.umpof;

import com.google.gson.stream.JsonReader;
import me.ldd.umpof.starter.UmpofStarter;
import net.lingala.zip4j.exception.ZipException;

public class Main {

    private static Umpof instance;
    public static String OS;

    public static void main(String[] args) {
        OS = System.getProperty("os.name");
        UmpofStarter starter =  new UmpofStarter(args);
        if (starter.active) {
            instance = new Umpof(starter);

            try {
                instance.start();
            } catch (ZipException e) {
                throw new RuntimeException(e);
            }
        }
    }

    public static Umpof getInstance() {
        return instance;
    }
}