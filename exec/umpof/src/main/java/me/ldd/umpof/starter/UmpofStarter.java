package me.ldd.umpof.starter;

import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import me.ldd.umpof.InnerConfigNotFoundException;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Map;

public class UmpofStarter {

    private JsonObject config;
    public boolean active = true;
    public Path target = null;

    public UmpofStarter(String[] args) {
        JsonObject defCfg = null;
        try (InputStream in = getClass().getResourceAsStream("/defcfg.json");
             BufferedReader reader = new BufferedReader(new InputStreamReader(in))) {
            StringBuilder jsonStr = new StringBuilder();
            reader.lines().forEach(jsonStr::append);
            defCfg = JsonParser.parseString(jsonStr.toString()).getAsJsonObject();
        } catch (IOException e) {
            throw new RuntimeException(e);
        } catch (NullPointerException e) {
            throw new InnerConfigNotFoundException("Inner stream is null");
        }
        if (defCfg == null) {
            throw new InnerConfigNotFoundException(null);
        }

        JsonObject runtimeCfg = new JsonObject();
        runtimeCfg.add("flags", new JsonArray());
        for (int i = 0; i < args.length; i++) {
            switch (args[i]) {
                case "-e" -> {
                    i++;
                    this.target = Path.of(args[i]);
                }
                case "-s" -> {
                    this.active = false;

                    break;
                }
                case "-rp" -> {
                    i++;
                    runtimeCfg.addProperty("redis_port", Integer.parseInt(args[i]));
                }
                case "-c" -> {
                    i++;
                    String a = args[i];
                    if (Files.exists(Path.of(a))) {
                        File file = new File(a);
                        if (file.canRead()) {
                            JsonObject obj = null;
                            try {
                                BufferedReader reader = new BufferedReader(new FileReader(file));
                                StringBuilder bldr = new StringBuilder();
                                reader.lines().forEach(bldr::append);
                                obj = JsonParser.parseString(bldr.toString()).getAsJsonObject();
                            } catch (FileNotFoundException e) {
                                throw new RuntimeException(e);
                            }
                            if (obj != null) mergeJ(runtimeCfg, obj);
                        } else {
                            System.out.println("Can not read config. Skipping...");
                        }
                    } else {
                        System.out.println("Invalid config to import. Skipping...");
                    }
                }
                default -> {
                    String str = args[i];
                    if (str.contains("=")) {
                        String[] splt = args[i].split("=");
                        if (splt.length != 2) throw new RuntimeException("Too many equals for keyed argument in cfg line");
                        runtimeCfg.addProperty(splt[0], splt[1]);
                    } else {
                        runtimeCfg.getAsJsonArray("flags").add(str);
                    }
                }
            }
        }
        //runtimeCfg.getAsJsonArray("flags").forEach((jsonElement -> System.out.println(jsonElement.toString())));
        mergeJ(runtimeCfg, defCfg);

        System.out.println(runtimeCfg.toString());

        this.config = runtimeCfg;
    }

    private void mergeJ(JsonObject source, JsonObject eta) {
        for (Map.Entry<String, JsonElement> pr : eta.entrySet()) {
            if (source.has(pr.getKey())) {
                if (pr.getValue().isJsonObject()) {
                    mergeJ(source.getAsJsonObject(pr.getKey()), pr.getValue().getAsJsonObject());
                }
            } else source.add(pr.getKey(), pr.getValue());

        }
    }

    public JsonObject getConfig() {
        return config;
    }
}
