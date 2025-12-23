#include <compiler.h>
#include <string.h>
#include <set>
#include <iostream>
#include <chrono>

#include <macro_defs.h>

compiler::compiler(const char* path, BUILD_MODE_MAIN mode_main) {
    this->folder_path = std::string(path);
    this->build_mode = mode_main;
    this->last_seq_index = 0;
}

void compiler::assign_rt_mode(RT_MODE mode) {
    this->adm_rt_mode = mode;
}


/*
*
*   Setup environ variables
*
*/
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


/*
*
*   Define parts
*
*/
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
            // TODO: Fix validation not working for some reason
            if (state == STATE::NAME_GRAB) {
                if (state != STATE::NONE && state != STATE::UNKNOWN) completed.insert(state);
                state = state_matcher.count(tmp_s) > 0 ? state_matcher.at(tmp_s) : STATE::UNKNOWN;
                if (
#ifdef COMP_DEBUG
                    true
#else
                    state == STATE::UNKNOWN
#endif
                ) {
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
                // if (completed.count(STATE::UPREFS) == 0) 
                //     this->handle_crash("Build interrupted on STATE::NODES. Reason: Wrong order of operations of PL build line. (STATE::UPREFS wasn't complete)");
                std::string sub1 = "", sub2 = "";
                bool flag = false;
                for (size_t j = 0; j < tmp_s.length(); j++) {
                    if (tmp_s[j] == ';') flag = true;
                    else {
                        if (flag) sub2+=tmp_s[j];
                        else sub1+=tmp_s[j];
                    }
                }
#ifdef COMP_DEBUG
                this->bef("Added (")->bef(sub2)->bef(") node: ")->log(sub1);
#endif
                this->nodes.insert({sub1, new nodeworks::node(sub1, sub2)});
            }
            else if (state == STATE::LINKS) {
                // if (completed.count(STATE::NODES) == 0) 
                //     this->handle_crash("Build interrupted on STATE::LINKS. Reason: Wrong order of operations of PL build line. (STATE::NODES wasn't complete)");
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
#ifdef COMP_DEBUG
                    this->bef("Added OUT link: ")->log(out_link->link_name);
#endif
                }
                this->links.push_back(in_link);
                node_to->links.insert({in_link->link_name, in_link});
#ifdef COMP_DEBUG
                this->bef("Added IN link: ")->log(in_link->link_name);
#endif
            }
            else if (state == STATE::STARTERS) {
                // if (completed.count(STATE::NODES) == 0) 
                //     this->handle_crash("Build interrupted on STATE::STARTERS. Reason: Wrong order of operations of PL build line. (STATE::NODES wasn't complete)");
                if (this->nodes.count(tmp_s) > 0) {
                    this->starters.push_back(this->nodes.at(tmp_s));
#ifdef COMP_DEBUG
                    this->bef("Detected starter: ")->log(tmp_s);
#endif
                }
                else {
#ifdef COMP_DEBUG
                    this->bef("[WARNING!!!] Try to add starter (")->bef(tmp_s)->log(") but this node doesn't exist... Skipping");
#endif
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

/*
*
*   Import packs
*
*/
bool compiler::import_packs() {
    this->log("Importing packs...");
    this->packs_info = new pack_proc::pack_proc(this->using_packs);

    this->log(this->packs_info->local_log)->nl(2);
    if (this->packs_info->crash_code.length() > 0) this->handle_crash(this->packs_info->crash_code);

#ifdef COMP_DEBUG
    this->log("Found some nodes:");
    for (std::pair<std::string, pack_proc::ninf*> pr : this->packs_info->ninfs) {
        this->bef(pr.first)->bef(" -> ")->log(pr.second->exec_type == pack_proc::ninf::EXEC_TYPE::DUMMY ? "DUMMY" : pr.second->get_exec_path());
    }
#endif

    return false;
}

void recursive_proc_node(std::set<nodeworks::node*>* list, nodeworks::node* node) {
    list->insert(node);
    for (std::pair<std::string, nodeworks::link*> pr : node->links) {
        if (pr.second->type == nodeworks::LINK_TYPE::IN) {
            recursive_proc_node(list, pr.second->get_source_node());
        }
    }
}

/* 
*
*   Compile seqenses
*
*/
bool compiler::comp_seq() {
    this->log("Compiling sequences");
    std::map<nodeworks::node*, seq*> binder;
    
#ifdef COMP_DEBUG
    std::list<nodeworks::node*> targets = this->get_targets();
    this->bef("Found starters ")->bef(std::to_string(targets.size()))->log(" starters: ");
    for (nodeworks::node* target : targets) {
        this->log(target->uuid);
    }
#endif

    // Deriving all possible sequence with given targets
    // TODO: Create better algorithmic solution... Reason: Very bad asymptothic on high amount of targets
    // For some reason addresses in logs are different between steps. I do not even want to try to start understanding this process and its reasoning
    for (nodeworks::node* target : targets) {
        std::set<nodeworks::node*>* innodes = new std::set<nodeworks::node*>();
        recursive_proc_node(innodes, target);

        bool flag0 = true;
        for (nodeworks::node* nd : *innodes) {
            if (binder.count(nd) > 0) {
                // Merge
                seq* sub_sq = binder.at(nd);
                this->bef("Merging &")->bef(std::to_string((long) innodes))->bef(" to sequence: &")->log(std::to_string((unsigned long) sub_sq));
                sub_sq->merge(innodes);
                flag0 = false;
                break;
            }
        }
        // Create new sequence
        if (flag0) {
            this->bef("Creating (")->bef(std::to_string(this->last_seq_index))->bef(") sequence. Addres (me): &")->log(std::to_string((unsigned long) innodes));
            seq* sq = new seq(innodes, this);
            for (nodeworks::node* nd : *innodes) {
                binder.insert({nd, sq});
            }
            this->seqs.insert({sq->index, sq});
        }
    }

    // Finalising sequences
    for (std::pair<size_t, seq*> pr : this->seqs) {
        if (pr.second->fin()) this->handle_crash("Aborted at sequnce finalisation. Critical compilation error.");
    }

    return false;
}

bool compiler::layerize() {
    this->log("Layerizing sequences...");
    for (std::pair<size_t, seq*> pr : this->seqs) {
        if (!pr.second->layerize()) this->handle_crash("Unknown crash on layerization. Idk what happend");
    }
    return false;
}

void compiler::handle_crash(const char* msg) {
    if (this->logfile_ptr != nullptr) this->log("\nCrash!!! :skull:")->log(msg)->end_log();
    std::cerr << msg << std::endl;
    exit(1);
}

void compiler::handle_crash(std::string msg) {
    this->handle_crash(msg.c_str());
}

compiler::RETURN_STATUS compiler::build() {
    this->start_log();
    auto start = grab_t;

    auto sub = grab_t;
    if (this->insert_vars()) return FAILED;
    this->log("[COOL] Vars successfuly defined!")->bef("Took: ")->log(dif_s(sub))->nl(2);

    sub = grab_t;
    if (this->define_parts()) return FAILED;
    this->log("[YAAAY] All PL parts defined!")->bef("Took: ")->log(dif_s(sub))->nl(2);

    sub = grab_t;
    if (this->import_packs()) return FAILED;
    this->log("[NO WAY] Packs imported!")->bef("Took: ")->log(dif_s(sub))->nl(2);

    // Cast

    sub = grab_t;
    if (this->comp_seq()) return FAILED;
    this->log("[SSadistic] Sequnces compiled!")->bef("Took: ")->log(dif_s(sub))->nl(2);

    sub = grab_t;
    if (this->layerize()) return FAILED;
    this->log("[ULTRAKILL] Layerization complete!")->bef("Took: ")->log(dif_s(sub))->nl(2);

    


    this->bef("Build finished! (took ")->bef(dif_s(start))->log(")");
    this->end_log();
    return compiler::RETURN_STATUS::SUCCESS;
}

