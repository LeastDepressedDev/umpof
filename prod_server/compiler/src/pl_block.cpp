#include <pl_gen.h>

pl_block::pl_block::pl_block(void* ptr, TYPE type) {
    this->ptr = ptr;
    this->type = type;
}

pl_block::node::node(nodeworks::node* node, pl_gen* owner) {
    this->node_ptr = node;
    this->node_ptr->fin_index = owner->next_node_index();
}

pl_block::dull::dull(u_int16_t size, u_int8_t byte) {
    this->size = size;
    this->byte = byte;
}

void pl_block::dull::write(FILE* f_ptr) {
    for (u_int16_t i = 0; i < this->size; i++) {
        fwrite(&this->byte, 1, 1, f_ptr);
    }
}

void pl_block::node::write(FILE* f_ptr) {
    this->f_ptr = f_ptr;
    fwrite(&this->node_ptr->fin_index, sizeof(u_int32_t), 1, f_ptr);
    std::string pth = this->node_ptr->ninf_ptr->rel_path();
    u_int32_t size = (u_int32_t) pth.length();
    fwrite(&size, sizeof(u_int32_t), 1, f_ptr);
    if (size > 0) fwrite(pth.c_str(), sizeof(char), size, f_ptr);
    size = (u_int32_t) this->node_ptr->prefs.size();
    fwrite(&size, sizeof(u_int32_t), 1, f_ptr);
    if (size>0) {
        for (std::pair<std::string, std::string> pr : this->node_ptr->prefs) {
            this->write_pref(pr);
        }
    }
    size = (u_int32_t) this->node_ptr->links.size();
    fwrite(&size, sizeof(u_int32_t), 1, f_ptr);
    if (size>0) {
        for (std::pair<std::string, nodeworks::link*> pr : this->node_ptr->links) {
            if (pr.second->type == nodeworks::LINK_TYPE::IN) {
                this->write_link(pr.second);
            }
        }
    }
}

void pl_block::node::write_pref(std::string key, std::string val) {
    u_int8_t se[2] = {0xff, 0x00};
    fwrite(se, sizeof(u_int8_t), 1, this->f_ptr);
    u_int32_t size = (u_int32_t) key.length();
    fwrite(&size, sizeof(u_int32_t), 1, this->f_ptr);
    fwrite(key.c_str(), sizeof(char), size, this->f_ptr);
    size = (u_int32_t) val.length();
    fwrite(&size, sizeof(u_int32_t), 1, this->f_ptr);
    fwrite(val.c_str(), sizeof(char), size, this->f_ptr);
    fwrite(se+1, sizeof(u_int8_t), 1, this->f_ptr);
}

void pl_block::node::write_pref(std::pair<std::string, std::string> pr) {
    this->write_pref(pr.first, pr.second);
}

void pl_block::node::write_link(nodeworks::link* lk) {
    u_int8_t se[2] = {0xff, 0x00};
    fwrite(se, sizeof(u_int8_t), 1, this->f_ptr);
    fwrite(&lk->get_source_node()->fin_index, sizeof(u_int32_t), 1, this->f_ptr);
    u_int32_t size = (u_int32_t) lk->link_name.size();
    fwrite(&size, sizeof(u_int32_t), 1, this->f_ptr);
    if (size > 0) {
        fwrite(lk->link_name.c_str(), sizeof(char), size, this->f_ptr);
    }
    fwrite(se+1, sizeof(u_int8_t), 1, this->f_ptr);
}

pl_block::b_seq::b_seq(seq* sq_ptr) {
    this->seq_ptr = sq_ptr;
}

void pl_block::b_seq::write(FILE* f_ptr) {
    this->f_ptr = f_ptr;
    fwrite(&this->seq_ptr->index, sizeof(u_int32_t), 1, f_ptr);
    u_int32_t size = this->seq_ptr->event.size();
    fwrite(&size, sizeof(u_int32_t), 1, f_ptr);
    fwrite(this->seq_ptr->event.c_str(), sizeof(char), size, f_ptr);
    size = (u_int32_t) this->seq_ptr->nodes->size();
    for (nodeworks::node* nd : *this->seq_ptr->nodes) {
        fwrite(&nd->fin_index, sizeof(u_int32_t), 1, f_ptr);
    }
    std::vector<std::vector<nodeworks::node*>> vec = this->seq_ptr->layers();
    size = (u_int32_t) vec.size();
    fwrite(&size, sizeof(u_int32_t), 1, f_ptr);
    for (auto sub_vec : vec) {
        this->write_layer(sub_vec);
    }
}

void pl_block::b_seq::write_layer(std::vector<nodeworks::node*> vec) {
    u_int8_t se[2] = {0xff, 0x00};
    fwrite(se, sizeof(u_int8_t), 1, f_ptr);
    u_int32_t size = (u_int32_t) vec.size();
    fwrite(&size, sizeof(u_int32_t), 1, f_ptr);
    for (nodeworks::node* nd : vec) {
        fwrite(&nd->fin_index, sizeof(u_int32_t), 1, f_ptr);
    }
    fwrite(se+1, sizeof(u_int8_t), 1, f_ptr);
}