package me.ldd.umpof.exec;

import me.ldd.umpof.comps.ExecutorComponent;

import java.util.HashMap;
import java.util.Map;

public class ExecutiveModule {
    private final Map<Short, ExecutorComponent> execs;

    public ExecutiveModule() {

        this.execs = new HashMap<>();
    }

    void registerExecutor(ExecutorComponent exec) {
        exec.link(this);
        this.execs.put(exec.id, exec);
    }
}
