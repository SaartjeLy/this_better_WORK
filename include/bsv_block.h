#include <string>
#include <vector>
#include "bsv_transaction.h"
#include "bsv_output.h"
#include "bsv_input.h"

#ifndef BSV_BLOCK_H
#define BSV_BLOCK_H

class BSV_BLOCK {
    private:
        uint32_t* ptr;
        std::string* file_data;

        void parse_block();
    public:
        BSV_BLOCK(uint32_t* ptr, std::string* data);
        ~BSV_BLOCK();

        unsigned long long hex_to_int(std::string hex_string);
        std::string read_bytes(int bytes);
        unsigned long long read_variable_bytes();

        std::string magic_number;
        std::string block_size;
        std::string version;
        std::string previous_block_hash;
        std::string merkle_root;
        std::string time;
        std::string bits;
        std::string nonce;
        unsigned long long number_of_transactions;
        std::vector<BSV_TRANSACTION*> transactions;
};

#endif