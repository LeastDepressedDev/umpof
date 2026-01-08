#include <nodeworks.h>

nodeworks::node::node(std::string uuid, std::string class_id) {
    this->uuid = uuid;
    this->class_id = class_id;
}

std::pair<bool, std::string> nodeworks::node::bind(pack_proc::ninf* ninf_ptr) {
    this->ninf_ptr = ninf_ptr;
    for (auto pr : this->links) {
        std::string* rq = nullptr;
        switch (pr.second->type)
        {
        case nodeworks::LINK_TYPE::IN:
            rq = new std::string("input");
        break;
        case nodeworks::LINK_TYPE::OUT:
            rq = new std::string("output");
        default:
            break;
        }
        if (rq == nullptr) return {false, "Incorrect or unsupported node type on linking... Too bad nerd (c)" + pr.first};
        auto mpm = this->ninf_ptr->links.at(*rq);
        if (mpm->count(pr.first) == 0) return {false, "Linking link(xd) not found (c)" + pr.first};
        pr.second->data_type = mpm->at(pr.first);
    }
    this->bound_ninf = true;
    return {true, ""};
}




nodeworks::link::link(nodeworks::LINK_TYPE ltype, std::string name, nodeworks::node* owner) {
    this->link_name = name;
    this->type = ltype;
    this->owner = owner;

    switch (this->type)
    {
    case nodeworks::LINK_TYPE::IN:
        break;
    case nodeworks::LINK_TYPE::OUT:
        this->target_links = new std::list<nodeworks::link*>();
        this->target_nodes = new std::list<nodeworks::node*>();
        break;
    default:
        std::cerr << "Caught undefined LINK_TYPE... Reporting death :skull:" << std::endl;
        break;
    }
}


nodeworks::node* nodeworks::link::get_source_node() {
    return this->type == nodeworks::LINK_TYPE::IN ? this->source_node : nullptr;
}

nodeworks::link* nodeworks::link::get_source() {
    return this->type == nodeworks::LINK_TYPE::IN ? this->source_link : nullptr;
}



std::list<nodeworks::node*>* nodeworks::link::get_target_nodes() {
    return this->type == nodeworks::LINK_TYPE::OUT ? this->target_nodes : nullptr;
}

std::list<nodeworks::link*>* nodeworks::link::get_targets() {
    return this->type == nodeworks::LINK_TYPE::OUT ? this->target_links : nullptr;
}



nodeworks::link* nodeworks::link::set_source(nodeworks::node* node) {
    this->source_node = node;
    return this;
}

nodeworks::link* nodeworks::link::set_source(nodeworks::link* link) {
    this->source_link = link;
    return this;
}