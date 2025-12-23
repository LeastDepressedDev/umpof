#include <pl_gen.h>

u_int32_t c4_0 = 0x00000000;
u_int32_t c4_ff = 0xffffffff;

pl_gen::pl_gen(compiler* owner_ptr) {
    this->owner = owner_ptr;
}

size_t pl_gen::next_node_index() {
    return this->node_indexation++;
}

void pl_gen::add_dull(u_int16_t sz, u_int16_t byte) {
    this->leg.push_back(pl_block::pl_block(new pl_block::dull(sz, byte), pl_block::TYPE::DULL));
}

void pl_gen::write(pl_block::pl_block block) {
    fwrite(&c4_ff, sizeof(u_int32_t), 1, this->f_ptr);
    fwrite(&block.type, 1, 1, this->f_ptr);
    switch (block.type) {
    case pl_block::TYPE::DULL:
        ((pl_block::dull*) block.ptr)->write(this->f_ptr);
    break;
    case pl_block::TYPE::NODE:
        ((pl_block::node*) block.ptr)->write(this->f_ptr);
    break;
    case pl_block::TYPE::SEQUENCE:
        ((pl_block::b_seq*) block.ptr)->write(this->f_ptr);
    break;
    }
    fwrite(&c4_0, sizeof(u_int32_t), 1, this->f_ptr);
}

void pl_gen::setup() {
    for (std::pair<size_t, seq*> pr : this->owner->seqs) {
        auto vec_f = pr.second->layers();
        for (auto sub_vec : vec_f) {
            for (nodeworks::node* nd : sub_vec) {
                pl_block::node* node = new pl_block::node(nd, this);
                this->leg.push_back(pl_block::pl_block((void*) node, pl_block::TYPE::NODE));
            }
        }
        pl_block::b_seq* sq = new pl_block::b_seq(pr.second);
        this->leg.push_back(pl_block::pl_block((void*) sq, pl_block::TYPE::SEQUENCE));
    }
}

void pl_gen::dump() {
    std::string pth = this->owner->folder_path + "/" + std::string(PL_FILE_NAME);
#ifdef COMP_DEBUG
    this->owner->log(pth);
#endif
    this->f_ptr = fopen(pth.c_str(), "w");

    for (pl_block::pl_block block : this->leg) {
        this->write(block);
    }

    fclose(this->f_ptr);
}