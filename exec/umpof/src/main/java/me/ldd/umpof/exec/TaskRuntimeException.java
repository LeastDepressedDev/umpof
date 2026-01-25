package me.ldd.umpof.exec;

public class TaskRuntimeException extends RuntimeException {
    public TaskRuntimeException(String message, SubTask task) {
        super(message + "task -> " + task.toString());
    }
}
