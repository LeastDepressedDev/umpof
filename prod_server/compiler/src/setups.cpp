#include <setups.h>
#include <filesystem>
#include <string>

void setup_dir(const char* path) {
    std::filesystem::create_directories(path);
}