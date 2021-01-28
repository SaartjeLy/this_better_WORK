#include <string>
#include <vector>

#ifndef BSV_BLOCK_H
#define BSV_BLOCK_H

class BSV_BLOCK {
    private:
        std::string magic_number;
        std::string block_size;
        std::string version;
        std::string previous_block_hash;
        std::string merkle_root;
        std::string time;
        std::string bits;
        std::string nonce;
        uint64_t number_of_transactions;
        std::vector<std::string> transactions;
    public:
        BSV_BLOCK();
        ~BSV_BLOCK();
        std::string get_magic_number();
        std::string get_block_size();
        std::string get_version();
        std::string get_previous_block_hash();
        std::string get_merkle_root();
        std::string get_time();
        std::string get_bits();
        uint64_t get_number_of_transactions();
        std::vector<std::string> get_transactions();
        void set_transaction(uint64_t idx);
        void set_magic_number(std::string magic_number);
        void set_block_size(std::string block_size);
        void set_version(std::string version);
        void set_previous_block_hash(std::string previous_block_hash);
        void set_merkle_root(std::string merkle_root);
        void set_time(std::string time);
        void set_bits(std::string bits);
        void set_number_of_transactions(uint64_t num_of_transactions);
        void add_transaction(std::string transaction);
};

#endif