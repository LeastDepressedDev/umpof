#pragma once
#include <iostream>
#include <compiler.h>

#define PL_FILE_NAME ".ppl"

class compiler;
class seq;
class pl_gen;

namespace pl_block {

    enum TYPE {
        DULL = 0x00,
        USING = 0x01,
        NODE = 0x02,
        SEQUENCE = 0x03
    };

    /**
     * 2 bytes -> n          size
     * n bytes                  'of u_int8_t byte'
     */
    class dull { // SIZE bytes of BYTE spaceing
    public:
        u_int16_t size;
        u_int8_t byte;
        dull(u_int16_t, u_int8_t);
        void write(FILE* fptr);
    };

    /**
     * 1 byte                      exec_type // Types are indexed in packs.h #kostily production
     * 2 bytes                   "sub id"
     * 4 bytes -> n           length
     * n bytes                   string-path
     */
    class using_exec { // Defining executables that can be used
    private:
        u_int16_t id;
    public:
        nodeworks::node* node;
        std::string pth;
        using_exec(nodeworks::node*, pl_gen*);
        void write(FILE* fptr);
    };

    /**
     * +struct pref
     * 1 byte                     0xff
     * 4 bytes -> k           key-length
     * k bytes                   str: pref-key
     * 4 bytes -> v           val-length
     * v bytes                   str: pref-val
     * 1 byte                     0x00
     * -struct
     * 
     * +struct in-link
     * 1 byte                      0xff
     * 4 bytes                   source-node-index
     * 4 bytes -> n           out-link-name-length
     * n bytes                   str: out-link-name
     * 1 byte                      0x00
     * -struct
     * 
     * 
     * 
     * 4 bytes                    index
     * 2 bytes                    regged_exec
     * 1 byte                       0x00
     * 4 bytes -> n2          pref-count
     * n2 pref                     prefs
     * 4 bytes -> n3          in-links-count
     * n3 in-link                 in-links
     */
    class node { // NODE... lol
    public:
        FILE* f_ptr;
        nodeworks::node* node_ptr;
        u_int16_t regged_exec;
        node(nodeworks::node*, pl_gen*, u_int16_t regged);
        void write(FILE* fptr);
        void write_pref(std::pair<std::string, std::string>);
        void write_pref(std::string key, std::string val);
        void write_link(nodeworks::link*);
    };

    /**
     * +struct layer
     * 1 bytes                           0xff
     * 4 bytes -> n                  amount of nodes
     * n * 4 bytes                    indexes of nodes
     * 1 byte                             0x00
     * -struct
     * 
     * 
     * 4 bytes                          index
     * 4 bytes -> n1                event-length
     * n1 bytes                        event
     * 4 bytes -> n2               nodes-count
     * n2 * 4 bytes                 indexes of nodes
     * 4 byte -> n3                 number of layers
     * n3 layer                        layers
     */
    class b_seq {
    private:
        seq* seq_ptr;
        FILE* f_ptr;
    public:
        b_seq(seq*);
        void write(FILE* fptr);
        void write_layer(std::vector<nodeworks::node*>);
    };

    //TODO: Add starter pl_block

    class pl_block {
    public:
        TYPE type;
        void* ptr;

        pl_block(void*, TYPE);
    };
}

class pl_gen
{
private:
    FILE* f_ptr = nullptr;
    std::list<pl_block::pl_block> leg;
    u_int16_t lmused = 0;

    compiler* owner;
    size_t node_indexation = 0;
public:
    std::map<std::string, u_int16_t> using_mapper;
    pl_gen(compiler*);

    void setup();
    void write(pl_block::pl_block);
    void dump();
    
    size_t next_node_index();
    u_int16_t next_lm();
    void add_dull(u_int16_t size, u_int16_t);
};