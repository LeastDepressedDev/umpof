#include <compiler.h>

void compiler::start_log() {
    this->logfile_ptr = fopen(this->folder_path.append("/build.log").c_str(), "a");
    this->log("Starting build log...");
}

void compiler::end_log() {
    this->log("End of log...");
    fclose(this->logfile_ptr);
}

compiler* compiler::log(const char* msg) {
    fprintf(this->logfile_ptr, "%s\n", msg);
    return this;
}

compiler* compiler::log(std::string msg) {
    this->log(msg.c_str());
    return this;
}

compiler* compiler::bef(const char* msg) {
    fprintf(this->logfile_ptr, "%s", msg);
    return this;
}

compiler* compiler::bef(std::string msg) {
    this->bef(msg.c_str());
    return this;
}

compiler* compiler::nl(size_t amt) {
    while (amt--) fprintf(this->logfile_ptr, "\n");    
    return this;
}

compiler* compiler::nl() {
    this->nl(1); return this;
}

compiler* compiler::log() {
    this->nl(); return this;
}
