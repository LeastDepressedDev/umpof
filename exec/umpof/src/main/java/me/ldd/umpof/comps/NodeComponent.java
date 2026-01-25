package me.ldd.umpof.comps;

import me.ldd.umpof.StorageModule;
import me.ldd.umpof.lb.ByteHelper;
import me.ldd.umpof.lb.Pair;

import java.io.FileInputStream;
import java.io.IOException;

public class NodeComponent extends Component {

    public static class InLink {
        public final int srcNodeId;
        public final String outLinkName;
        public final String inLinkName;

        public InLink(int srcNodeId, String outLinkName, String inLinkName) {
            this.srcNodeId = srcNodeId;
            this.outLinkName = outLinkName;
            this.inLinkName = inLinkName;
        }
    }

    public final Integer index;
    private final short execId;
    public final Pair<String, String>[] prefs;
    private final InLink[] links;

    private ExecutorComponent executor = null;

    public NodeComponent(Integer index, short execId, Pair<String, String>[] prefs, InLink[] links) {
        this.index = index;
        this.execId = execId;
        this.prefs = prefs;
        this.links = links;
    }

    public NodeComponent(FileInputStream fs) throws IOException {
        this.index = ByteHelper.readNumeric(fs, 4).getInt();
        this.execId = ByteHelper.readNumeric(fs, 2).getShort();
        byte skb = fs.readNBytes(1)[0];
        if (skb != (byte) 0x00) throw new NodeParseException(this, "Info section terminator is corrupted");

        int prefSz = ByteHelper.readNumeric(fs, 4).getInt();
        this.prefs = new Pair[prefSz];
        for (int i = 0; i < prefSz; i++) {
            byte vdb = fs.readNBytes(1)[0];
            if (vdb != (byte) 0xFF) throw new NodeParseException(this, "Pref starter corrupted byte:" + vdb);

            int sz = ByteHelper.readNumeric(fs, 4).getInt();
            String p1 = ByteHelper.readSizedStr(fs, sz);
            sz = ByteHelper.readNumeric(fs, 4).getInt();
            String p2 = ByteHelper.readSizedStr(fs, sz);
            this.prefs[i] = new Pair<>(p1, p2);

            vdb = fs.readNBytes(1)[0];
            if (vdb != (byte) 0x00) throw new NodeParseException(this, "Pref ender corrupted byte:" + vdb);
        }
        int lkSz = ByteHelper.readNumeric(fs, 4).getInt();
        this.links = new InLink[lkSz];
        for (int i = 0; i < lkSz; i++) {
            byte vdb = fs.readNBytes(1)[0];
            if (vdb != (byte) 0xFF) throw new NodeParseException(this, "Link starter corrupted byte:" + vdb);

            int ni = ByteHelper.readNumeric(fs, 4).getInt();
            int n = ByteHelper.readNumeric(fs, 4).getInt();
            String outlk = ByteHelper.readSizedStr(fs, n);
            n = ByteHelper.readNumeric(fs, 4).getInt();
            String inlk = ByteHelper.readSizedStr(fs, n);
            this.links[i] = new InLink(ni, outlk, inlk);

            vdb = fs.readNBytes(1)[0];
            if (vdb != (byte) 0x00) throw new NodeParseException(this, "Link ender corrupted");
        }
    }

    public ExecutorComponent getExecutor() {
        return executor;
    }

    public void setExecFrom(StorageModule module) {
        this.executor = module.getExecs().get(this.execId);
    }

    public String constructPrefName(String idp) {
        return constructPrefName(this, idp);
    }


    public static String constructPrefName(NodeComponent component, String idp) {
        return "__pref::%d.%s".formatted(component.index, idp);
    }

    public InLink[] getLinks() {
        return links;
    }
}
