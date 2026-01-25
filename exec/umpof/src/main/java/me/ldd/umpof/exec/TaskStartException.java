package me.ldd.umpof.exec;

public class TaskStartException extends RuntimeException {
    public TaskStartException(String message, SubTask task, Exception exception) {
        super(message + "task -> " + task.toString());
        if (exception != null) {
            exception.printStackTrace();
        }
    }
}
