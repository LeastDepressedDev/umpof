#include <compiler.h>
#include <string.h>
#include <set>


compiler::compiler(const char* path, BUILD_MODE_MAIN mode_main) {
    this->folder_path = std::string(path);
    this->build_mode = mode_main;
}

void compiler::assign_rt_mode(RT_MODE mode) {
    this->adm_rt_mode = mode;
}

bool compiler::insert_vars() {
    this->log("Getting");
    this->main_line = getenv("MAIN_LINE");
    this->prefs_sz = std::stoi(getenv("PREF_NODES_SZ"));

    if (this->main_line != nullptr && this->prefs_sz != -1) {
        this->log("Imported")->log("MAIN_LINE")->log(this->main_line);
        this->bef("PREF_SZ = ")->log(std::to_string(this->prefs_sz));
        return false;
    } else {
        this->log("Failed to import important env vars!");
        return true;
    }
}

bool compiler::define_parts() {
    this->log("Defining parts...");
    enum STATE {
        NONE, NAME_GRAB, UNKNOWN,

        UPREFS,
        NODES,
        STARTERS,
        EVENTS,
        LINKS
    };
    std::set<STATE> completed;
    const std::map<std::string, STATE> state_matcher = {
        {"uprefs", STATE::UPREFS},
        {"nodes", STATE::NODES},
        {"starters", STATE::STARTERS},
        {"events", STATE::EVENTS},
        {"links", STATE::LINKS}
    };
    STATE state = STATE::NONE;
    std::string tmp_s = "";
    size_t lst = 0;
    for (size_t i = 0; i < strlen(this->main_line); i++) {
        bool ap = true;
        if (this->main_line[i] == '\n') {
            // TODO: Maybe rewrite as switch(state)
            // TODO: Implement normal validation checks for this part
            if (state == STATE::NAME_GRAB) {
                if (state != STATE::NONE && state != STATE::UNKNOWN) completed.insert(state);
                state = state_matcher.count(tmp_s) > 0 ? state_matcher.at(tmp_s) : STATE::UNKNOWN;
                if (this->debug_log || state == STATE::UNKNOWN) {
                    if (state == STATE::UNKNOWN) this->bef("[WARNING!!!] ");
                    this->bef("State changed: ")->log(tmp_s);
                }
            }
            else if (state == STATE::UPREFS) {
                this->bef("Processing line with STATE::UPREFS <- ")->log(tmp_s);
                size_t sub_id = 0;
                std::string sub = "";
                for (size_t j = 0; j < tmp_s.length(); j++) {
                    if (tmp_s[j] == ' ') {
                        j++;
                        if (sub == "using") {
                            sub = "";
                            for (j; j < tmp_s.length(); j++) {
                                if (tmp_s[j] == ' ') {
                                    if (sub.length() > 0) {
                                        this->using_packs.push_back(sub);
                                        this->bef("Found pack: ")->log(sub);
                                    }
                                    sub = "";
                                    continue;
                                } 
                                sub+=tmp_s[j];
                            }
                            if (sub.length() > 0) {
                                this->using_packs.push_back(sub);
                                this->bef("Found pack: ")->log(sub);
                            }
                        }
                    }

                    sub+=tmp_s[j];
                }
            }
            else if (state == STATE::NODES) {
                if (completed.count(STATE::UPREFS) == 0) 
                    this->handle_crash("Build interrupted on STATE::NODES. Reason: Wrong order of operations of PL build line. (STATE::UPREFS wasn't complete)");
                std::string sub1 = "", sub2 = "";
                bool flag = false;
                for (size_t j = 0; j < tmp_s.length(); j++) {
                    if (tmp_s[j] == ';') flag = true;
                    else {
                        if (flag) sub2+=tmp_s[j];
                        else sub1+=tmp_s[j];
                    }
                }
                if (this->debug_log) this->bef("Added (")->bef(sub2)->bef(") node: ")->log(sub1);
                this->nodes.insert({sub1, new nodeworks::node(sub1, sub2)});
            }
            else if (state == STATE::LINKS) {
                if (completed.count(STATE::NODES) == 0) 
                    this->handle_crash("Build interrupted on STATE::LINKS. Reason: Wrong order of operations of PL build line. (STATE::NODES wasn't complete)");
                std::vector<std::string> segments;
                std::string sub = "";
                for (size_t j = 0; j < tmp_s.length(); j++) {
                    if (tmp_s[j] == '.' || tmp_s[j] == ';') {
                        segments.push_back(sub);
                        sub = "";
                    } else sub+=tmp_s[j];
                }
                segments.push_back(sub);

                nodeworks::node* node_from;
                nodeworks::node* node_to;

                bool add_out = true;
                nodeworks::link* out_link;
                if (this->nodes.count(segments[0]) > 0) {
                    node_from = this->nodes.at(segments[0]);
                    if (node_from->links.count(segments[1]) > 0) {
                        out_link = node_from->links.at(segments[1]);
                        add_out = false;
                    } else {
                        out_link = new nodeworks::link(nodeworks::LINK_TYPE::OUT, segments[1]);
                    }
                } else {
                    this->handle_crash("Critical situation happened on STATE::LINKS line processing: Node doesn't exist");
                }

                if (this->nodes.count(segments[2]) == 0) this->handle_crash("Critical situation happened on STATE::LINKS line processing: Node doesn't exist");
                node_to = this->nodes.at(segments[2]);
                nodeworks::link* in_link = new nodeworks::link(nodeworks::LINK_TYPE::IN, segments[3]);

                out_link->get_target_nodes()->push_back(this->nodes.at(segments[2]));
                out_link->get_targets()->push_back(in_link);
                in_link->set_source(out_link)->set_source(this->nodes.at(segments[0]));

                if (add_out) {
                    this->links.push_back(out_link);
                    node_from->links.insert({out_link->link_name, out_link});
                    if (this->debug_log) this->bef("Added OUT link: ")->log(out_link->link_name);
                }
                this->links.push_back(in_link);
                node_to->links.insert({in_link->link_name, in_link});
                if (this->debug_log) this->bef("Added IN link: ")->log(in_link->link_name);
            }
            else if (state == STATE::STARTERS) {
                if (completed.count(STATE::NODES) == 0) 
                    this->handle_crash("Build interrupted on STATE::STARTERS. Reason: Wrong order of operations of PL build line. (STATE::NODES wasn't complete)");
                if (this->nodes.count(tmp_s) > 0) {
                    this->starters.push_back(this->nodes.at(tmp_s));
                    if (this->debug_log) this->bef("Detected starter: ")->log(tmp_s);
                }
                else {
                    if (this->debug_log) this->bef("[WARNING!!!] Try to add starter (")->bef(tmp_s)->log(") but this node doesn't exist... Skipping");
                }
            } else {
                // HOW TF this happend???
                this->handle_crash("How TF did you managed to create variable contradiction in state machine???...\nUhm... Embrace it now, and live with it, asshole.");
            }
            lst = 0;
            tmp_s = "";
            continue;
        }

        if (lst == 0 && this->main_line[i] == '#') {
            state = STATE::NAME_GRAB;
            ap = false;
        }

        
        
        if (ap) tmp_s+=this->main_line[i];
        lst++;
    }
    return false;
}



void compiler::handle_crash(const char* msg) {
    if (this->logfile_ptr != nullptr) this->log("Crash!!! :skull:\n")->log(msg)->end_log();
    std::cerr << msg << std::endl;
    exit(1);
}

void compiler::handle_crash(std::string msg) {
    this->handle_crash(msg.c_str());
}

compiler::RETURN_STATUS compiler::build() {
    this->start_log();
    if (insert_vars()) return FAILED;
    this->log("[COOL] Vars successfuly defined!")->nl(2);
    if (define_parts()) return FAILED;
    this->log("[YAAAY] All PL parts defined!")->nl(2);




    this->end_log();
    return compiler::RETURN_STATUS::SUCCESS;
}

