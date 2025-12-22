#include <nodeworks.h>

nodeworks::node::node(std::string uuid, std::string class_id) {
    this->uuid = uuid;
    this->class_id = class_id;
}





nodeworks::link::link(nodeworks::LINK_TYPE ltype, std::string name) {
    this->link_name = name;
    this->type = ltype;

    switch (this->type)
    {
    case nodeworks::LINK_TYPE::IN:
        break;
    case nodeworks::LINK_TYPE::OUT:
        this->target_links = new std::list<nodeworks::link*>();
        this->target_nodes = new std::list<nodeworks::node*>();
        break;
    default:
        std::cerr << "Caught undefined LINK_TYPE... Reporting death :skull:\n";
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