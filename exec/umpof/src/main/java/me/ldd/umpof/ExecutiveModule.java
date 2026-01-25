package me.ldd.umpof;

import me.ldd.umpof.exec.ETask;
import me.ldd.umpof.exec.SubTask;

import java.util.Deque;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Set;

public class ExecutiveModule {
    public final StorageModule storage;

    public final Deque<ETask> tasks = new LinkedList<>();
    private final int parallel;

    public ExecutiveModule(StorageModule storage, int parallel) {
        this.storage = storage;
        this.parallel = parallel;
    }

    public void callEvent(String event) {
        this.storage.getEventSequences().get(event).forEach((a) -> this.tasks.addFirst(new ETask(a)));
    }

    public void step() {
        ETask task = tasks.peekFirst();
        Set<SubTask> t = new HashSet<>();
        task.executing.forEach((a) -> {
            if (a.process == null) {
                System.out.println("Skipp null dummy process.");
                t.add(a);
            }
            else if (!a.process.isAlive()) {
                a.finish();
                t.add(a);
            }
        });
        task.tasks.removeAll(t);
        task.executing.removeAll(t);
        if (task.executing.size() >= this.parallel) return;
        if(task.next()) {
            tasks.pollFirst();
        }
    }

    public boolean finished() {
        return tasks.isEmpty();
    }
}
