package me.ldd.umpof;

import com.google.gson.JsonObject;
import me.ldd.umpof.comps.ExecutorComponent;
import me.ldd.umpof.comps.NodeComponent;
import me.ldd.umpof.comps.SequenceComponent;
import me.ldd.umpof.lb.ByteHelper;
import me.ldd.umpof.lb.Pair;
import me.ldd.umpof.lb.Utils;
import me.ldd.umpof.starter.UWorkMode;
import me.ldd.umpof.starter.UmpofStarter;
import net.lingala.zip4j.ZipFile;
import net.lingala.zip4j.exception.ZipException;
import redis.clients.jedis.Jedis;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.UUID;

public class Umpof {

    public enum State {
        SLEEPING,
        UNPACKING,
        PREPARING,
        REDIS_SETUP,
        RUNNING, // ONGOING, NOT FINISHED YET DESPITE BEING USED
        SECURE_STOP,
        END,
        CLEANUP
    }

    public final UWorkMode WORKMODE;
    public final Integer REDIS_PORT;
    public State state;
    public Path targetPath;
    public final UUID uuid;

    public final StorageModule hapCtr;
    public ExecutiveModule execCtr = null;
    public Process redisProc = null;

    private JsonObject cfg;


    public Umpof(UmpofStarter starter) {
        if (starter.target == null) throw new StarterException("Target pipeline not specified");
        if (!Files.exists(starter.target)) throw new StarterException("Target pipeline file not exists");
        this.targetPath = starter.target;
        this.cfg = starter.getConfig();

        this.WORKMODE = UWorkMode.rmod(cfg.get("workmode").getAsString());
        this.REDIS_PORT = cfg.get("redis_port").getAsInt();
        this.state = State.SLEEPING;
        this.uuid = UUID.randomUUID();


        this.hapCtr = new StorageModule(constructName(this.uuid));
    }

    //TODO: Add normal Exception proc for subsections
    public void start() throws ZipException {
        workdir();
        try {
            prepare();
        } catch (FileNotFoundException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        try {
            redis_setup();
        } catch (IOException e) {
            try {
                this.cleanup();
            } catch (IOException ex) {
                throw new RuntimeException(ex);
            }
            throw new RuntimeException(e);
        }

        try {
            this.run();
        } catch (Exception e) {
            this.innerCrashLogic(e);
        }

        secure_stop();
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

        this.hapCtr.getNodes().forEach((id, node) -> node.setExecFrom(this.hapCtr));
        this.hapCtr.getSequences().forEach((id, seq) -> seq.buildNodeSet(this.hapCtr));
        fin.close();
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

                this.hapCtr.registerExecutor(new ExecutorComponent(sub_id, str, type, this.hapCtr));
                System.out.printf("Registered executor: %s\n", str);
            }
            case 0x02 -> {
                NodeComponent node = new NodeComponent(fin);
                this.hapCtr.registerNode(node);
                System.out.printf("Registered node: %d\n", node.index);
            }
            case 0x03 -> {
                SequenceComponent seq = new SequenceComponent(fin);
                this.hapCtr.registerSequence(seq);
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

    public final void redis_setup() throws IOException {
        this.state = State.REDIS_SETUP;
        System.out.printf("Running on %s\n", Main.OS);
        boolean c_script = cfg.has("c_redist_launch");
        if (c_script) {c_script_redis();}
        else {
            switch (Main.OS) {
                case "Linux" -> {
                    ProcessBuilder pb = new ProcessBuilder("redis-server", "--port", REDIS_PORT.toString());
                    this.redisProc = pb.start();
                }
                case "Windows 11" -> {
                    ProcessBuilder pb = new ProcessBuilder(System.getenv("APPDATA")+"\\"+this.cfg.get("win_redis_pth").getAsString()+"\\redis-server.exe", "--port", REDIS_PORT.toString());
                    this.redisProc = pb.start();
                }
                default -> {
                    System.out.println("Unsupported OS type... Catching for custom bash launch script.");
                }
            }
        }

        while (!this.redisProc.isAlive());
        if (this.redisProc.isAlive()) {
            Jedis jedis = new Jedis("localhost", this.REDIS_PORT);
            this.hapCtr.getNodes().forEach((id, node) -> {
                for (Pair<String, String> pref : node.prefs) {
                    jedis.set(node.constructPrefName(pref.first), pref.second);
                }
            });
        }


    }

    private final void c_script_redis() throws IOException {
        ProcessBuilder pb = new ProcessBuilder("bash", this.cfg.get("redis-server").getAsString());
        pb.environment().put("REDIS_PORT_EXT", this.REDIS_PORT.toString());
        this.redisProc = pb.start();
    }



    private void run() {
        this.state = State.RUNNING;
        this.execCtr = new ExecutiveModule(this.hapCtr, this.cfg.get("parallel_procs").getAsInt());
        this.execCtr.callEvent("__nat_start");
        while (!this.execCtr.finished()) this.execCtr.step();
    }


    private void secure_stop() {
        this.state = State.SECURE_STOP;
        System.out.println("Finished!!! Entering secure stop stage.");

        general_stop();
    }


    private void general_stop() {
        try {
            this.state = State.END;
            this.redisProc.destroy();

            this.cleanup();
        } catch (IOException exception) {
            throw new RuntimeException("Failed general stop section! Too bad...", exception);
        }
    }

    private void cleanup() throws IOException {
        this.state = State.CLEANUP;
        Utils.deleteDirectoryWithFiles(Path.of(constructName(this.uuid)));
    }

    private void innerCrashLogic(Exception exception) {
        exception.printStackTrace();

    }
}
