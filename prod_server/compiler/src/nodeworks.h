#pragma once
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <packs.h>

namespace nodeworks
{
    class node;
    class link;

    enum LINK_TYPE {
        IN,
        OUT
    };

    class node {
    private:

    public:
        size_t fin_index;

        bool bound_ninf = false;
        pack_proc::ninf* ninf_ptr;


        std::string uuid;
        std::string class_id;
        size_t execution_layer = 0;
        
        std::map<std::string, link*> links;
        std::map<std::string, std::string> prefs;

        node(std::string uuid, std::string class_id);
        std::pair<bool, std::string> bind(pack_proc::ninf*);
    };

    class link {
    private:
        node* source_node; // For IN links
        std::list<node*>* target_nodes; // For OUT links

        link* source_link;
        std::list<link*>* target_links;
    public:
        node* owner;
        LINK_TYPE type;
        std::string link_name;
        std::string data_type;
        link(LINK_TYPE type, std::string name, node* owner);

        node* get_source_node();
        std::list<node*>* get_target_nodes();
        link* get_source();
        std::list<link*>* get_targets();
        link* set_source(node*);
        link* set_source(link*);
    };
} 
