#include "../include/bsv_block.h"

BSV_BLOCK::BSV_BLOCK() {}

BSV_BLOCK::~BSV_BLOCK() {
    //TODO: implement destructor if necessary in the future
}

std::string* BSV_BLOCK::get_magic_number() {
    return magic_number;
}

std::string* BSV_BLOCK::get_block_size() {
    return block_size;
}

std::string* BSV_BLOCK::get_version() {
    return version;
}

std::string* BSV_BLOCK::get_previous_block_hash() {
    return previous_block_hash;
}

std::string* BSV_BLOCK::get_merkle_root() {
    return merkle_root;
}

std::string* BSV_BLOCK::get_time() {
    return time;
}

std::string* BSV_BLOCK::get_bits() {
    return bits;
}

std::string* BSV_BLOCK::get_nonce() {
    return nonce;
}

unsigned long BSV_BLOCK::get_number_of_transactions() {
    return number_of_transactions;
}

std::vector<std::string>* BSV_BLOCK::get_transactions() {
    return transactions;
}

void BSV_BLOCK::set_magic_number(std::string* data) {
    magic_number = data;
}

void BSV_BLOCK::set_block_size(std::string* data) {
    block_size = data;
}

void BSV_BLOCK::set_version(std::string* data) {
    version = data;
}

void BSV_BLOCK::set_previous_block_hash(std::string* data) {
    previous_block_hash = data;
}

void BSV_BLOCK::set_merkle_root(std::string* data) {
    merkle_root = data;
}

void BSV_BLOCK::set_time(std::string* data) {
    time = data;
}

void BSV_BLOCK::set_bits(std::string* data) {
    bits = data;
}

void BSV_BLOCK::set_nonce(std::string* data) {
    nonce = data;
}

void BSV_BLOCK::set_number_of_transactions(unsigned long data) {
    number_of_transactions = data;
}

void BSV_BLOCK::add_transaction(std::string* transaction) {
    //TODO: implement
    ;
}