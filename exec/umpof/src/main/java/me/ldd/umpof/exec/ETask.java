package me.ldd.umpof.exec;

import me.ldd.umpof.comps.NodeComponent;
import me.ldd.umpof.comps.SequenceComponent;

import java.io.IOException;
import java.util.*;

public class ETask {
    public final UUID uuid;
    public final Queue<SubTask> tasks;
    public final SequenceComponent src;

    public final Set<SubTask> executing = new HashSet<>();

    public ETask(SequenceComponent seq) {
        this.uuid = UUID.randomUUID();
        this.tasks = new LinkedList<>();
        this.src = seq;

        for (SequenceComponent.Layer layer : src.layers) {
            for (NodeComponent comp : layer.nodeQueue) {
                tasks.add(new SubTask(comp));
            }
        }
    }

    public boolean next() {
        SubTask task = tasks.poll();
        if (task == null && executing.isEmpty()) return true;
        if (task != null) {
            try {
                long pid = task.run();
            } catch (IOException e) {
                throw new TaskStartException("Failed to start SubTask", task, e);
            }
            executing.add(task);
        }
        return false;
    }
}
