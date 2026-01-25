package me.ldd.umpof.exec;

import me.ldd.umpof.Main;
import me.ldd.umpof.comps.ExecutorComponent;
import me.ldd.umpof.comps.NodeComponent;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.UUID;

public class SubTask {
    public enum State {
        PREPARED,
        RUNNING,
        DONE,
        ERROR
    }

    public final UUID uuid;
    private final NodeComponent component;
    public State state = State.PREPARED;
    public Process process = null;

    public SubTask(NodeComponent ex) {
        this.uuid = UUID.randomUUID();
        this.component = ex;
    }

    public NodeComponent getComponent() {
        return component;
    }

    public long run() throws IOException {
        NodeComponent comp = this.getComponent();
        List<String> cmd = genCmd(comp);
        if (comp.getExecutor().type == ExecutorComponent.Type.DUMMY) return 0;
        ProcessBuilder pb = new ProcessBuilder(cmd);
        this.process = pb.start();
        long pid = this.process.pid();
        System.out.printf("Running task as pid: %d\n", pid);
        while (!process.isAlive()); //TODO: Normal single thread start logic
        this.state = State.RUNNING;
        return pid;
    }

    private List<String> genCmd(NodeComponent comp) {
        String pth = "%s/%s".formatted(comp.getExecutor().getModule().ppl_dir, this.component.getExecutor().path);
        if (Main.OS.startsWith("Windows")) pth = pth.replaceAll("/", "\\\\");
        String prog = switch (this.component.getExecutor().type) {
            case PYTHON -> "python";
            default -> ".";
        };
        List<String> cmd = new ArrayList<>(Arrays.asList(prog, pth, "rport="+Main.getInstance().REDIS_PORT, "node="+this.component.index));
        for (NodeComponent.InLink link : comp.getLinks()) {
            String cp = "oe:%d=%s".formatted(link.srcNodeId, link.outLinkName);
            System.out.println(cp);
            cmd.add(cp);
        }
        return cmd;
    }

    public void finish() {
        if (this.process.exitValue() == 0) this.state = State.DONE;
        else {
            this.state = State.ERROR;
            this.process.errorReader().lines().forEach(System.out::println);
        }

    }

    @Override
    public String toString() {
        return "%s:%s | %s".formatted(this.uuid.toString(), this.state.toString(), this.process == null ? "None" : Long.toString(this.process.pid()));
    }
}
