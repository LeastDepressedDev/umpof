package me.ldd.umpof.lb;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Comparator;
import java.util.stream.Stream;

public class Utils {
    public static void deleteDirectoryWithFiles(Path directoryPath) throws IOException {
        try (Stream<Path> paths = Files.walk(directoryPath)) {
            paths.sorted(Comparator.reverseOrder()) // Sort in reverse order (innermost files first)
                    .forEach(path -> {
                        try {
                            Files.delete(path); // Delete the file or directory
                        } catch (IOException e) {
                            System.err.println("Failed to delete " + path + ": " + e.getMessage());
                        }
                    });
        }
    }

}
