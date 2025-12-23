#include <packs.h>

pack_proc::pack::pack(std::string path) {
    this->path = path;
}

std::map<std::string, pack_proc::pack*> pack_proc::pack_proc::get_packs() {
    return this->packs;
}

pack_proc::pack_proc* pack_proc::pack_proc::bef(std::string msg) {
    this->local_log += msg;
    return this;
}

pack_proc::pack_proc* pack_proc::pack_proc::log(std::string msg) {
    this->bef(msg + "\n");
    return this;
}

void pack_proc::pack::load() {
    std::string json_s = "";
    char cbuf[256];
    FILE* f = fopen((this->path+"/pack.json").c_str(), "r");
    while (fgets(cbuf, 256, f) != NULL) json_s+=cbuf;
    this->json_obj = json::parse(json_s);
    this->id = json_obj["id"];
}

pack_proc::ninf::ninf(json def, pack* src) {
    this->source = src;
    this->id = std::string(def["id"]);
    if (def.contains("flag")) this->flag = std::string(def["flag"]);
    const std::map<std::string, ninf::EXEC_TYPE> matcher = {
        {"dummy", DUMMY},
        {"python", PYTHON}
    };
    this->links.insert({std::string("input"), new std::map<std::string, std::string>()});
    this->links.insert({std::string("output"), new std::map<std::string, std::string>()});
    this->exec_type = matcher.count(def["exec-type"]) > 0 ? matcher.at(def["exec-type"]) : DUMMY;
    if (this->exec_type != DUMMY) {
        this->exec_path = def["exec"];
    }
    json format_block = def["format"];
    for (size_t i = 0; i < format_block.size(); i++) {
        json sub = format_block.at(i);
        this->links.at(sub["block"])->insert({std::string(sub["unid"]), std::string(sub["type"])});
    }
}

std::string pack_proc::ninf::get_exec_path() {
    return this->source->path + "/" + this->exec_path;
}
