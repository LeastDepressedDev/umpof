#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <nodeworks.h>
#include <packs.h>
#include <set>
#include <pl_gen.h>

#define COMP_DEBUG
#define TLESS_LOG

class pl_gen;
class compiler;
class seq;

enum BUILD_MODE_MAIN {
    B_UDEF = 0x00,
    RT = 0x01,
    DOCK = 0x02
};

enum RT_MODE {
    RT_UDEF = 0x00,
    LINEAR = 0x01,
    SERVICE = 0x02
};

enum DOCK_MODE {
    D_UDEF = 0x00,
    dkF = 0x01
};

class compiler
{
private:
    BUILD_MODE_MAIN build_mode;
    RT_MODE adm_rt_mode;
    DOCK_MODE dmode;

    FILE* logfile_ptr;

    char* main_line;
    size_t prefs_sz = -1;

    std::list<std::string> using_packs;
    std::map<std::string, nodeworks::node*> nodes;
    std::list<nodeworks::link*> links;
    std::list<nodeworks::node*> starters;

    pack_proc::pack_proc* packs_info;

    size_t casts = 0;

    size_t last_seq_index;

    pl_gen* ppl_c;
public:
#ifdef TLESS_LOG
    std::string _tless_log_pth;
    void _tless_log_head();
    void _tless_log_tail();
#endif
    enum RETURN_STATUS {
        SUCCESS,
        FAILED,
        FAILED_IDK_WHY
    };

    // Why not std::vector? Cuz MORE STABLE UGA-BUGA to use map of index to seq*. Faster search, even if it doesn't mater on small values
    std::map<size_t, seq*> seqs;
    std::string folder_path; // Doesn't end with '/'


    compiler(const char* base_dir, BUILD_MODE_MAIN bm);

    void assign_rt_mode(RT_MODE);
    void assign_docker_mode(DOCK_MODE);

    
    // Build tasks
    bool insert_vars();
    bool define_parts();
    bool load_prefs();
    bool import_packs();
    bool cast();
    bool comp_seq();
    bool layerize(); // It was totaly optional but i thought that this would be great thing to have in such a big project
    bool gen_ppl();
    bool compile();
    
    
    compiler::RETURN_STATUS build();


    void add_link(nodeworks::node*, nodeworks::link*);
    void do_ninf(nodeworks::node*);
    void recursive_proc_node(std::set<nodeworks::node*>* list, nodeworks::node* node);

    std::list<nodeworks::node*> get_targets();
    size_t next_seq_index();
    pack_proc::pack_proc* get_pack_info(); // Defined in comp_seq.cpp
    
    std::string gen_cast_name();

    void end_log();
    //Shit... This part looks awful
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

class seq {
private:
    compiler* owner;

    std::list<std::list<nodeworks::node*>*> __layers;
public:
    std::set<nodeworks::node*>* nodes;
    size_t index;
    std::string event;
    bool invalid;

    seq(std::set<nodeworks::node*>* node_set, compiler* caller);
    void merge(std::set<nodeworks::node*>* node_set);

    bool invalidate(std::string reason);    
    bool fin();
    bool layerize();

    std::list<nodeworks::node*> get_starters();
    std::vector<std::vector<nodeworks::node*>> layers();
};