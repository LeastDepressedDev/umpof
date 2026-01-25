package me.ldd.umpof.comps;

import me.ldd.umpof.StorageModule;

public class Component {
    private StorageModule module = null;

    public void link(StorageModule module) {
        if (this.module == null) this.module = module;
        else throw new ComponentRelinkException();
    }

    public final StorageModule getModule() {
        return module;
    }

    public final boolean isBound() {
        return module == null;
    }
}
