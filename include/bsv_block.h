#include <string>
#include <vector>

#ifndef BSV_BLOCK_H
#define BSV_BLOCK_H

class BSV_BLOCK {
    private:
        uint32_t* ptr;
        std::string* file_data;
        std::string* magic_number;
        std::string* block_size;
        std::string* version;
        std::string* previous_block_hash;
        std::string* merkle_root;
        std::string* time;
        std::string* bits;
        std::string* nonce;
        unsigned long number_of_transactions;
        std::vector<std::string>* transactions;

        void set_magic_number(std::string* magic_number);
        void set_block_size(std::string* block_size);
        void set_version(std::string* version);
        void set_previous_block_hash(std::string* previous_block_hash);
        void set_merkle_root(std::string* merkle_root);
        void set_time(std::string* time);
        void set_bits(std::string* bits);
        void set_nonce(std::string* nonce);
        void set_number_of_transactions (unsigned long num_of_transactions);
        void add_transaction(std::string* transaction);

        unsigned long hex_to_int(std::string* hex_string);
        std::string* read_bytes(uint32_t* ptr, int bytes, std::string* file_data);
        unsigned long read_variable_bytes(uint32_t* ptr, std::string* file_data);
        void parse_block();
    public:
        BSV_BLOCK(uint32_t* ptr, std::string* data);
        ~BSV_BLOCK();

        std::string* get_magic_number();
        std::string* get_block_size();
        std::string* get_version();
        std::string* get_previous_block_hash();
        std::string* get_merkle_root();
        std::string* get_time();
        std::string* get_bits();
        std::string* get_nonce();
        unsigned long get_number_of_transactions();
        std::vector<std::string>* get_transactions();
};

#endif