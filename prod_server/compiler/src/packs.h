#pragma once
#include <iostream>
#include <map>
#include <list>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace pack_proc {
    class pack_proc;
    class pack;
    class ninf;


    class pack_proc
    {
    private:
        std::list<std::string> using_packs;
        std::string packs_path;
        
        std::list<pack*> __tmp_packs;
        std::map<std::string, pack*> packs;
        std::map<std::string, std::map<std::string, std::string>*> casts;
    public:
        std::map<std::string, ninf*> ninfs;
        std::string local_log;
        std::string crash_code = "";

        pack_proc(std::list<std::string> packs);
        void proc_dir(std::string path);

        pack_proc* log(std::string);
        pack_proc* bef(std::string);

        std::string* cast_check(std::string, std::string);

        std::map<std::string, pack*> get_packs();
    };




    class pack {
    public:
        std::string path;
        std::string id;
        json json_obj;
        std::map<std::string, ninf*> ninfs;

        pack(std::string path);
        void load();
    };

    class ninf {
    private:
        std::string exec_path = "";
    public:
        enum EXEC_TYPE {
            DUMMY = 0x00,
            PYTHON = 0x01
        };
        pack* source;
        EXEC_TYPE exec_type;
        std::string id;
        std::map<std::string, std::map<std::string, std::string>*> links;
        std::list<std::string> pref_names;
        std::string flag = "";
        bool is_event = false;
        std::string event;
        
        ninf(json, pack*);
        std::string get_exec_path();
        std::string rel_path();
    };
}