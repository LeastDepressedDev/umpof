package me.ldd.umpof;

import me.ldd.umpof.comps.Component;
import me.ldd.umpof.comps.ExecutorComponent;
import me.ldd.umpof.comps.NodeComponent;
import me.ldd.umpof.comps.SequenceComponent;
import org.w3c.dom.Node;

import java.util.*;

public class ExecutiveModule {
    private final Map<Short, ExecutorComponent> execs;
    private final Map<Integer, NodeComponent> nodes;

    // Sequence by index
    private final Map<Integer, SequenceComponent> iSeqMap;
    // Sequence by event str
    private final Map<String, SequenceComponent> eSeqMap;

    public final List<Component> components;

    public ExecutiveModule() {

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
        this.eSeqMap.put(seq.event, seq);
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

    public Map<String, SequenceComponent> getEventSequences() {
        return eSeqMap;
    }
}
