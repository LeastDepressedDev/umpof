package me.ldd.umpof.comps;

import me.ldd.umpof.ExecutiveModule;
import me.ldd.umpof.lb.ByteHelper;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.HashSet;
import java.util.Set;
import java.util.TreeSet;
import java.util.concurrent.atomic.AtomicInteger;

public class SequenceComponent extends Component {

    public static class Layer {
        public final SequenceComponent sequence;
        public final int order;
        private final Set<Integer> __nodes = new TreeSet<>();
        public final Set<NodeComponent> nodesSet = new HashSet<>();
        public NodeComponent[] nodeQueue = null;


        public Layer(SequenceComponent sequence, int order) {
            this.sequence = sequence;
            this.order = order;
        }

        public void readLayer(FileInputStream fs) throws IOException {
            byte vdb = fs.readNBytes(1)[0];
            if (vdb != (byte) 0xFF) throw new SeqLayerParseException("Starter byte corrupted");

            int nsz = ByteHelper.readNumeric(fs, 4).getInt();
            for (int i = 0; i < nsz; i++) {
                this.__nodes.add(ByteHelper.readNumeric(fs, 4).getInt());
            }

            vdb = fs.readNBytes(1)[0];
            if (vdb != (byte) 0x00) throw new SeqLayerParseException("Terminator byte corrupted");
        }

        public void buildNodeSet(ExecutiveModule module) {
            this.nodeQueue = new NodeComponent[this.__nodes.size()];
            AtomicInteger i = new AtomicInteger();
            this.__nodes.forEach((id) -> {
                NodeComponent node = module.getNodes().get(id);
                this.nodesSet.add(node);
                this.nodeQueue[i.getAndIncrement()] = node;
            });

        }
    }

    public final int index;
    public final String event;
    public final Set<NodeComponent> nodes = new HashSet<>();
    public final Set<Integer> nodeIds = new TreeSet<>();
    public final Layer[] layers;

    public SequenceComponent(int index, String event, Layer[] layers) {
        this.index = index;
        this.event = event;
        this.layers = layers;
    }

    public SequenceComponent(FileInputStream fs) throws IOException {
        this.index = ByteHelper.readNumeric(fs, 4).getInt();
        this.event = ByteHelper.readSizedStr(fs, ByteHelper.readNumeric(fs, 4).getInt());
        int node_c = ByteHelper.readNumeric(fs, 4).getInt();
        for (int i = 0; i < node_c; i++) {
            nodeIds.add(ByteHelper.readNumeric(fs, 4).getInt());
        }
        int lrsz = ByteHelper.readNumeric(fs, 4).getInt();
        this.layers = new Layer[lrsz];
        for (int i = 0; i < lrsz; i++) {
            this.layers[i] = new Layer(this, i);
            this.layers[i].readLayer(fs);
        }
    }

    public void buildNodeSet(ExecutiveModule module) {
        nodeIds.forEach((id) -> nodes.add(module.getNodes().get(id)));
        for (Layer layer : this.layers) layer.buildNodeSet(module);
    }
}
