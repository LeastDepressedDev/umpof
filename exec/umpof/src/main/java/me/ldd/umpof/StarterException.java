package me.ldd.umpof;

public class StarterException extends RuntimeException {
    public StarterException(String message) {
        super("\nStarter said bye bye: "+message);
    }
}
