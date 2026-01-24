package me.ldd.umpof.comps;

import me.ldd.umpof.ExecutiveModule;

public class Component {
    private ExecutiveModule module = null;

    public void link(ExecutiveModule module) {
        if (this.module == null) this.module = module;
        else throw new ComponentRelinkException();
    }

    public final ExecutiveModule getModule() {
        return module;
    }

    public final boolean isBound() {
        return module == null;
    }
}
