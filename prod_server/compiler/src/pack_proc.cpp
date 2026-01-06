//
//  get_packs, log, bef
//              defined in packs.cpp
//
#include <packs.h>
#include <filesystem>
#include <vector>

pack_proc::pack* gen_pack(std::string pth) {
    return new pack_proc::pack(pth);
}

void pack_proc::pack_proc::proc_dir(std::string path) {
    if (std::filesystem::exists(path)) { // Cross check
        for (const auto& tg : std::filesystem::directory_iterator(path)) {
            if (std::filesystem::is_regular_file(tg.status())) {
                //this->bef(std::string(tg.path().c_str()) + "\n      " + std::string(tg.path().filename().c_str()) + "\n");
                auto sub_pth = tg.path();
                if (sub_pth.has_filename() && std::string(sub_pth.filename().c_str()) == "pack.json") {
                    this->__tmp_packs.push_back(gen_pack(path));
                }
            } else if (std::filesystem::is_directory(tg.status())) {
                proc_dir(tg.path().c_str());
            }
        }
    }
}

std::string* pack_proc::pack_proc::cast_check(std::string from, std::string to) {
    if (this->casts.count(from) != 0) {
        auto sub = this->casts.at(from);
        return sub->count(to) != 0 ? &sub->at(to) : nullptr;
    }
    return nullptr;
}

pack_proc::pack_proc::pack_proc(std::list<std::string> list) {
    this->packs_path = getenv("PACKS_DIR");
    this->using_packs = list;
    proc_dir(this->packs_path);
    for (pack* p : this->__tmp_packs) {
        p->load();
        this->packs.insert({p->id, p});
    }
    this->__tmp_packs.clear();

    bool crash = false;
    std::list<std::string> sub_list;
    for (std::string id : list) {
        if (this->packs.count(id) == 0) {
            this->crash_code = "Aborted!!! " + id + " pack non present on build server!";
            crash = true;
            break;
        } else sub_list.push_back(id);
    }
    if (!crash) { // Loading section
        for (std::string id : sub_list) {
            pack* pack_ptr = this->packs.at(id);

            //Nodes
            json nodes = pack_ptr->json_obj["nodes"];
            for (size_t i = 0; i < nodes.size(); i++) {
                ninf* nf = new ninf(nodes.at(i), pack_ptr);
                std::string full_id = id + ":" + nf->id;
                this->ninfs.insert({full_id, nf});
                pack_ptr->ninfs.insert({nf->id, nf});
            }

            //Events
            if (pack_ptr->json_obj.contains("events")) {
                json events = pack_ptr->json_obj["events"];
                for (size_t i = 0; i < events.size(); i++) {
                    json sub = events.at(i);
                    ninf* nf = new ninf(sub, pack_ptr);
                    std::string full_id = id + ":" + nf->id;
                    nf->is_event = true;
                    nf->event = sub["event"];
                    this->ninfs.insert({full_id, nf});
                    pack_ptr->ninfs.insert({nf->id, nf});
                }
            }

            //Casts
            if (pack_ptr->json_obj.contains("casts")) {
                json casts = pack_ptr->json_obj["casts"];
                for (size_t i = 0; i < casts.size(); i++) {
                    json sub = casts.at(i);
                    std::string from = std::string(sub["from"]);
                    if (this->casts.count(from) == 0)
                        this->casts.insert({from, new std::map<std::string, std::string>()});
                    this->casts.at(from)->insert({std::string(sub["to"]), std::string(sub["conv"])});
                }
            }
        }
    }
}