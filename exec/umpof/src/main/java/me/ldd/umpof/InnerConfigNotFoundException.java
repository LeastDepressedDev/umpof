package me.ldd.umpof;

public class InnerConfigNotFoundException extends RuntimeException {
    public InnerConfigNotFoundException(String message) {
        super("Inner config not found" + (message == null ? "" : ": " + message));
    }
}
