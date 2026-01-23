package me.ldd.umpof.exceptions;

public class StarterException extends RuntimeException {
    public StarterException(String message) {
        super("\nStarter said bye bye: "+message);
    }
}
