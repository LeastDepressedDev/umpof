#include <compiler.h>
#include <macro_defs.h>

std::list<nodeworks::node*> compiler::get_targets() {
    std::list<nodeworks::node*> targets;
    for (std::pair<std::string, nodeworks::node*> pr : this->nodes) {
        if (pr.second->ninf_ptr->flag == "target") {
            targets.push_back(pr.second);
        }
    }
    return targets;
}

size_t compiler::next_seq_index() {
    return this->last_seq_index++;
}

pack_proc::pack_proc* compiler::get_pack_info() {
    return this->packs_info;
}

seq::seq(std::set<nodeworks::node*>* set_ptr, compiler* caller) {
    this->owner = caller;

    this->nodes = set_ptr;
    this->index = this->owner->next_seq_index();
    this->invalid = false;
    this->event = std::string("__nat_start");
}

void seq::merge(std::set<nodeworks::node*>* set_ptr) {
    for (nodeworks::node* nd : *set_ptr) {
        this->nodes->insert(nd);
    }
}

bool seq::fin() {

    // Process events
    pack_proc::ninf* evt = nullptr;
    for (nodeworks::node* nd : *this->nodes) { // Doing full check cuz it's the same asympthotic + more validation scope
        if (nd->ninf_ptr->is_event) {
            if (evt != nullptr) return this->invalidate("More then two event nodes in sequence detected!");
            else evt = nd->ninf_ptr; 
        }
    }

    if (evt != nullptr) {
        this->event = evt->event;
#ifdef COMP_DEBUG
        this->owner->bef("Sequence (")->bef(std::to_string(this->index))->log(") set as custom event.");
#endif
    }
#ifdef COMP_DEBUG
    else {
        this->owner->bef("Sequence (")->bef(std::to_string(this->index))->log(") keeps __nat_start (default) event.");
    }
#endif
    

    return this->invalid;
}

bool seq::invalidate(std::string reason) {
    this->owner->bef("[SEQ(")->bef(std::to_string(this->index))->bef(")] Invalidated itself. Reason: ")->log(reason);
    this->invalid = true;
    return this->invalid;
}

std::list<nodeworks::node*> seq::get_starters() {
    std::list<nodeworks::node*> nt;
    for (nodeworks::node* nd : *this->nodes) {
        bool flag0 = true; // true -> no IN links in node
        for (std::pair<std::string, nodeworks::link*> pr : nd->links) {
            if (pr.second->type == nodeworks::LINK_TYPE::IN) {
                flag0 = false;
                break;
            }
        }
        if (flag0) nt.push_back(nd);
    }
    return nt;
}

bool seq::layerize() {
    auto start = grab_t;
    std::set<nodeworks::node*> completed;
#ifdef COMP_DEBUG
    this->owner->bef("Layerizing seq(")->bef(std::to_string(this->index))->bef("): &")->log(std::to_string((unsigned long) this));
#endif

    // This gotta be bad on time... TODO: MAAAAAAYBE optimize it for the good
    // Also there can occur an infinite loop, but this is kinda 'turing machine' like problem so we gotta ignore it xd ||| FIXED I GUES ||| ON CHECK
    size_t layer_ind = 0;
    while (completed.size() < this->nodes->size()) 
    {
#ifdef COMP_DEBUG
        this->owner->bef("Processing n=")->bef(std::to_string(layer_ind))->log(" layer of layerization...");
#endif
        std::list<nodeworks::node*>* layer = new std::list<nodeworks::node*>();
        for (nodeworks::node* nd : *this->nodes) {
            if (completed.count(nd) > 0) continue;
            bool flag0 = true; // Can be executed on step 'layer_ind'?
            for (std::pair<std::string, nodeworks::link*> pr : nd->links) {
                if (pr.second->type == nodeworks::LINK_TYPE::IN) {
                    if (!completed.count(pr.second->get_source_node())) { flag0 = false; break; }
                }
            }
            if (flag0) {
                layer->push_back(nd);
            }
        }
        if (layer->size() == 0) this->owner->handle_crash("Bad new.s.. On layerization we got a layer with size of 0. That means the continuos recursion, so aborting!");

#ifdef COMP_DEBUG
        this->owner->bef("Got a layer: size=")->log(std::to_string(layer->size()));
#endif

        for (nodeworks::node* nd : *layer) {
            completed.insert(nd);
            nd->execution_layer = layer_ind;
        }

        this->__layers.push_back(layer);
        layer_ind++;
    }

#ifdef COMP_DEBUG
    this->owner->bef("Finished layerizing this section in ")->bef(dif_s(start))->log(".")->nl();
#endif

    return true;
}

std::vector<std::vector<nodeworks::node*>> seq::layers() {
    std::vector<std::vector<nodeworks::node*>> vec_main(this->__layers.size());
    size_t i = 0;
    for (std::list<nodeworks::node*>* part : this->__layers) {
        std::vector<nodeworks::node*> sub_vec(part->size());
        size_t j = 0;
        for (nodeworks::node* nd : *part) {
            sub_vec[j] = nd; j++;
        }
        vec_main[i] = sub_vec;
        i++;
    }
    return vec_main;
}