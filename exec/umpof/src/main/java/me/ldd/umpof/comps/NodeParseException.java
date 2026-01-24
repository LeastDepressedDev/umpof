package me.ldd.umpof.comps;

public class NodeParseException extends RuntimeException {
    public NodeParseException(NodeComponent comp, String message) {
        super("%s <- nodeid:%s".formatted(message, comp.index == null ? "undefined" : comp.index.toString()));
    }
}
