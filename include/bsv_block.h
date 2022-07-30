#include <string>
#include <iostream>
#include <vector>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <assert.h>
#include "bsv_transaction.h"
#include "bsv_output.h"
#include "bsv_input.h"

#ifndef BSV_BLOCK_H
#define BSV_BLOCK_H

class BSV_BLOCK {
    private:
        uint32_t* ptr;
        std::string* file_data;
        std::string reverse_pairs(std::string const & src);
        std::string get_blockHash(std::string version, std::string previous_block_hash, std::string merkle_root, std::string time, std::string bits, std::string nonce);
        std::string get_TXID(BSV_TRANSACTION transaction);

    public:
        BSV_BLOCK(uint32_t* ptr, std::string* data);
        ~BSV_BLOCK();

        unsigned long long hex_to_int(std::string hex_string);
        std::string read_bytes(int bytes);
        unsigned long long read_variable_bytes();

        std::string magic_number;
        std::string block_size;
        std::string version;
        std::string block_hash;
        std::string previous_block_hash;
        std::string merkle_root;
        std::string time;
        std::string bits;
        std::string nonce;
        std::string tx_count;
        std::string version_num;
        std::string inputs;
        std::string previous_tx;
        std::string prev_index;
        std::string script_length;

        // Magic Number: d9b4bef9
        // Blocksize: 285
        // Version: 1
        // Previous Hash 00000000000000000000000000000000
        // Merkle Root 4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b
        // Time 1231006505
        // Difficulty 1d00ffff
        // Nonce 2083236893
        // Tx Count 1
        // Version Number 1
        // Inputs 1
        // Previous Tx 00000000000000000000000000000000
        // Prev Index 4294967295
        // Script Length 77
        // ScriptSig 4ffff01d14455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73
        // ScriptSig O��EThe Times 03/Jan/2009 Chancellor on brink of second bailout for banks
        // Seq Num ffffffff
        // Outputs 1
        // Value 50.0
        // Script Length 67
        // Script Pub Key 414678afdb0fe5548271967f1a67130b7105cd6a828e0399a67962e0ea1f61deb649f6bc3f4cef38c4f3554e51ec112de5c384df7bab8d578a4c702b6bf11d5fac
        // Lock Time 0
        unsigned long long number_of_transactions;
        std::vector<BSV_TRANSACTION> transactions;
};

#endif