package me.ldd.umpof;

import com.google.gson.JsonObject;
import me.ldd.umpof.comps.ExecutorComponent;
import me.ldd.umpof.comps.NodeComponent;
import me.ldd.umpof.comps.SequenceComponent;
import me.ldd.umpof.lb.ByteHelper;
import me.ldd.umpof.starter.UWorkMode;
import me.ldd.umpof.starter.UmpofStarter;
import net.lingala.zip4j.ZipFile;
import net.lingala.zip4j.exception.ZipException;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.UUID;

public class Umpof {

    public enum State {
        SLEEPING,
        UNPACKING,
        PREPARING,
        REDIS_SETUP,
        RUNNING,
        SECURE_STOP,
        END,
        CLEANUP
    }

    public final UWorkMode WORKMODE;
    public final int REDIS_PORT;
    public State state;
    public Path targetPath;
    public final UUID uuid;

    public final ExecutiveModule execCtr;

    public Umpof(UmpofStarter starter) {
        if (starter.target == null) throw new StarterException("Target pipeline not specified");
        if (!Files.exists(starter.target)) throw new StarterException("Target pipeline file not exists");
        this.targetPath = starter.target;
        JsonObject cfg = starter.getConfig();

        this.WORKMODE = UWorkMode.rmod(cfg.get("workmode").getAsString());
        this.REDIS_PORT = cfg.get("redis_port").getAsInt();
        this.state = State.SLEEPING;
        this.uuid = UUID.randomUUID();


        this.execCtr = new ExecutiveModule();
    }

    public void start() throws ZipException {
        workdir();
        try {
            prepare();
        } catch (FileNotFoundException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    private void workdir() throws ZipException {
        this.state = State.UNPACKING;
        ZipFile zf = new ZipFile(targetPath.toString());
        zf.extractAll(constructName(this.uuid));
    }

    private void prepare() throws IOException {
        this.state = State.PREPARING;
        File file = new File(constructName(this.uuid)+"/.ppl");
        // TODO: Proper checks
        FileInputStream fin = new FileInputStream(file);
        while (readSection(fin));

        this.execCtr.getNodes().forEach((id, node) -> node.setExecFrom(this.execCtr));
        this.execCtr.getSequences().forEach((id, seq) -> seq.buildNodeSet(this.execCtr));
    }

    public static String constructName(UUID uuid) {
        return ".%s".formatted(uuid.toString());
    }

    private boolean readSection(FileInputStream fin) throws IOException {
        if (fin.available() == 0) return false;

        byte[] sbyte = fin.readNBytes(4);
        for (byte c : sbyte) {
            if (c != (byte) 0xFF) throw new RuntimeException("Section first line offset corrupted.");
        }
        byte TYPE = fin.readNBytes(1)[0];
        switch (TYPE) {
            case 0x00 -> {
                short size = ByteHelper.readNumeric(fin, 2).getShort();
                fin.skipNBytes(size);
                System.out.println("Worked over skip section.");
            }
            case 0x01 -> {
                byte type = fin.readNBytes(1)[0];
                short sub_id = ByteHelper.readNumeric(fin, 2).getShort();
                int strSize = ByteHelper.readNumeric(fin, 4).getInt();
                String str = ByteHelper.readSizedStr(fin, strSize);

                this.execCtr.registerExecutor(new ExecutorComponent(sub_id, str, type));
                System.out.printf("Registered executor: %s\n", str);
            }
            case 0x02 -> {
                NodeComponent node = new NodeComponent(fin);
                this.execCtr.registerNode(node);
                System.out.printf("Registered node: %d\n", node.index);
            }
            case 0x03 -> {
                SequenceComponent seq = new SequenceComponent(fin);
                this.execCtr.registerSequence(seq);
                System.out.printf("Registered sequence: %s\n", seq.event);
            }
            default -> {
                System.out.println("Got an unknown type... skipping...");
            }
        }
        sbyte = fin.readNBytes(4);
        for (byte c : sbyte) {
            if (c != (byte) 0x00) throw new RuntimeException("Section last line offset corrupted.");
        }
        return true;
    }
}
