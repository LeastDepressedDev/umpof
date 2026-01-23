package me.ldd.umpof.comps;

import me.ldd.umpof.exec.ExecutiveModule;

public class ExecutorComponent {

    public enum Type {
        DUMMY,
        PYTHON
    }

    public final String path;
    public final short id;
    public final Type type;

    private ExecutiveModule module = null;

    public ExecutorComponent(short id, String path, byte exec_type) {
        this.path = path;
        this.id = id;
        switch (exec_type) {
            case 0x01 -> this.type = Type.PYTHON;
            default -> this.type = Type.DUMMY;
        }
    }

    boolean isBound() {
        return module != null;
    }

    public void link(ExecutiveModule module) {
        if (this.module == null) this.module = module;
        else throw new RuntimeException("Attempt to relink component... wtf");
    }

    public ExecutiveModule getModule() {
        return module;
    }
}
