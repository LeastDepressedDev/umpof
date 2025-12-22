#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <nodeworks.h>

enum BUILD_MODE_MAIN {
  RT,
  DOCK
};

enum RT_MODE {
    LINEAR,
    SERVICE
};

enum DOCK_MODE {
    dkF
};

class compiler
{
private:
    const bool debug_log = true;

    std::string folder_path; // Doesn't end with '/'
    BUILD_MODE_MAIN build_mode;
    RT_MODE adm_rt_mode;
    DOCK_MODE dmode;

    FILE* logfile_ptr;

    char* main_line;
    size_t prefs_sz = -1;

    std::vector<std::string> using_packs;
    std::map<std::string, nodeworks::node*> nodes;
    std::list<nodeworks::link*> links;
    std::list<nodeworks::node*> starters;
public:
    enum RETURN_STATUS {
        SUCCESS,
        FAILED,
        FAILED_IDK_WHY
    };
    compiler(const char* base_dir, BUILD_MODE_MAIN bm);

    void assign_rt_mode(RT_MODE);
    void assign_docker_mode(DOCK_MODE);

    
    // Build tasks
    bool insert_vars();
    bool define_parts();
    
    
    compiler::RETURN_STATUS build();
    void interrupt();

    void end_log();
    compiler* log(const char* msg);
    compiler* log(std::string msg);
    compiler* log(); // Same as nl
    compiler* nl();
    compiler* nl(size_t);
    compiler* bef(const char* msg);
    compiler* bef(std::string msg);
    void handle_crash(std::string crash_msg);
    void handle_crash(const char* crash_msg);
    void start_log();
};