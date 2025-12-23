#pragma once
#include <iostream>
#include <list>
#include <map>
#include <vector>

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
        std::string uuid;
        std::string class_id;
        size_t execution_layer = 0;
        
        std::map<std::string, link*> links;

        node(std::string uuid, std::string class_id);
    };

    class link {
    private:
        node* source_node; // For IN links
        std::list<node*>* target_nodes; // For OUT links

        link* source_link;
        std::list<link*>* target_links;
    public:
        LINK_TYPE type;
        std::string link_name;
        std::string data_type;
        link(LINK_TYPE type, std::string name);

        node* get_source_node();
        std::list<node*>* get_target_nodes();
        link* get_source();
        std::list<link*>* get_targets();
        link* set_source(node*);
        link* set_source(link*);
    };
} 
