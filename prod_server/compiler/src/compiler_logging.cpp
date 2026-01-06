#include <compiler.h>

#ifdef TLESS_LOG
void compiler::_tless_log_head() {
    this->logfile_ptr = fopen(this->_tless_log_pth.c_str(), "a"); // Not an omptimized solution but it is a debug logging mode so we just gonna ignore it. >///<
}

void compiler::_tless_log_tail() {
    fclose(this->logfile_ptr);
    this->logfile_ptr = nullptr;
}

#endif

void compiler::start_log() {
#ifdef TLESS_LOG
    this->_tless_log_pth = (this->folder_path + std::string("/build.log"));
#else
    this->logfile_ptr = fopen((this->folder_path + std::string("/build.log")).c_str(), "a");
#endif
    this->log("Starting build log...");
}

void compiler::end_log() {
    this->log("End of log...");
#ifndef TLESS_LOG
    fclose(this->logfile_ptr);
#endif
}

compiler* compiler::log(const char* msg) {
#ifdef TLESS_LOG
    this->_tless_log_head();
#endif 
    fprintf(this->logfile_ptr, "%s\n", msg);
#ifdef TLESS_LOG
    this->_tless_log_tail();
#endif 
    return this;
}

compiler* compiler::log(std::string msg) {
    this->log(msg.c_str());
    return this;
}

compiler* compiler::bef(const char* msg) {
#ifdef TLESS_LOG
    this->_tless_log_head();
#endif
    fprintf(this->logfile_ptr, "%s", msg);
#ifdef TLESS_LOG
    this->_tless_log_tail();
#endif 
    return this;
}

compiler* compiler::bef(std::string msg) {
    this->bef(msg.c_str());
    return this;
}

compiler* compiler::nl(size_t amt) {
#ifdef TLESS_LOG
    this->_tless_log_head();
#endif
    while (amt--) fprintf(this->logfile_ptr, "\n"); 
#ifdef TLESS_LOG
    this->_tless_log_tail();
#endif
    return this;
}

compiler* compiler::nl() {
    this->nl(1); return this;
}

compiler* compiler::log() {
    this->nl(); return this;
}
