package me.ldd.umpof;

import me.ldd.umpof.comps.Component;
import me.ldd.umpof.comps.ExecutorComponent;
import me.ldd.umpof.comps.NodeComponent;
import me.ldd.umpof.comps.SequenceComponent;

import java.util.*;

public class StorageModule {
    private final Map<Short, ExecutorComponent> execs;
    private final Map<Integer, NodeComponent> nodes;

    // Sequence by index
    private final Map<Integer, SequenceComponent> iSeqMap;
    // Sequence by event str
    private final Map<String, Set<SequenceComponent>> eSeqMap;

    public final List<Component> components;

    public final String ppl_dir;

    public StorageModule(String pplDir) {
        ppl_dir = pplDir;

        this.execs = new TreeMap<>();
        this.nodes = new TreeMap<>();
        this.components = new LinkedList<>();

        this.iSeqMap = new TreeMap<>();
        this.eSeqMap = new HashMap<>();
    }

    protected void linkM(Component cmp) {
        cmp.link(this);
        this.components.add(cmp);
    }

    public final void registerExecutor(ExecutorComponent exec) {
        this.linkM(exec);
        this.execs.put(exec.id, exec);
    }

    public final void registerNode(NodeComponent node) {
        this.linkM(node);
        this.nodes.put(node.index, node);
    }

    public final void registerSequence(SequenceComponent seq) {
        this.linkM(seq);
        this.iSeqMap.put(seq.index, seq);
        if (this.eSeqMap.containsKey(seq.event)) this.eSeqMap.get(seq.event);
        else {
            Set<SequenceComponent> st = new HashSet<>(); st.add(seq);
            this.eSeqMap.put(seq.event, st);
        }
    }


    public Map<Integer, NodeComponent> getNodes() {
        return nodes;
    }

    public Map<Short, ExecutorComponent> getExecs() {
        return execs;
    }

    public Map<Integer, SequenceComponent> getSequences() {
        return iSeqMap;
    }

    public Map<String, Set<SequenceComponent>> getEventSequences() {
        return eSeqMap;
    }
}
