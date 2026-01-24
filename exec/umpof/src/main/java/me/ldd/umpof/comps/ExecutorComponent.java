package me.ldd.umpof.comps;

import me.ldd.umpof.ExecutiveModule;

public class ExecutorComponent extends Component {

    public enum Type {
        DUMMY,
        PYTHON
    }

    public final String path;
    public final short id;
    public final Type type;

    public ExecutorComponent(short id, String path, byte exec_type) {
        this.path = path;
        this.id = id;
        switch (exec_type) {
            case 0x01 -> this.type = Type.PYTHON;
            default -> this.type = Type.DUMMY;
        }
    }

}
