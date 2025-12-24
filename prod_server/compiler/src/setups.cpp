#include <setups.h>
#include <filesystem>
#include <string>

void setup_dir(const char* path) {
    std::filesystem::create_directories(path);
}

void copy_file(const char* file, const char* out_name) {
    setup_dir(std::filesystem::path(out_name).parent_path().c_str());
    std::filesystem::copy(file, out_name);
}