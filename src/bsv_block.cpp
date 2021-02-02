#include "../include/bsv_block.h"

BSV_BLOCK::BSV_BLOCK(uint32_t* ptr, std::string* data) : file_data(data), ptr(ptr) {
    parse_block();
}

void BSV_BLOCK::parse_block() {
    // preamble
    set_magic_number(read_bytes(ptr, 4, file_data));
    set_block_size(read_bytes(ptr,4,file_data));

    // header
    set_version(read_bytes(ptr,4,file_data));
    set_previous_block_hash(read_bytes(ptr,32,file_data));
    set_merkle_root(read_bytes(ptr,32,file_data));
    set_time(read_bytes(ptr,4,file_data));
    set_bits(read_bytes(ptr,4,file_data));
    set_nonce(read_bytes(ptr,4,file_data));

    // transactions
    unsigned long number_of_transactions = read_variable_bytes(ptr, file_data);
    set_number_of_transactions(number_of_transactions);

    for (int i=0; i<number_of_transactions; i++) {
        std::string version = *read_bytes(ptr, 4, file_data);

        unsigned long number_of_inputs = read_variable_bytes(ptr, file_data);
        for( int x=0; x<number_of_inputs; x++) {
            std::string pre_transaction_has = *read_bytes(ptr, 32, file_data);
            std::string pre_transactions_out_index = *read_bytes(ptr, 4, file_data);

            unsigned long script_length = read_variable_bytes(ptr, file_data);
            std::string script_length_string = std::to_string(script_length);

            std::string script = *read_bytes(ptr, script_length, file_data);
            std::string sequence = *read_bytes(ptr, 4, file_data);
        }

        unsigned long number_of_outputs = read_variable_bytes(ptr, file_data);
        for( int y=0; y<number_of_outputs; y++) {
            std::string value = *read_bytes(ptr, 8, file_data);

            unsigned long script_length = read_variable_bytes(ptr, file_data);
            std::string script_length_string = std::to_string(script_length);

            std::string script = *read_bytes(ptr, script_length, file_data);
        }
    
        std::string lock_time = *read_bytes(ptr, 4, file_data);
    }
}

/**
 * @brief takes a hex string and converts it to its equivalent unsigned long value
 * 
 * @param hex_string 
 * @return unsigned long 
 */
unsigned long BSV_BLOCK::hex_to_int(std::string* hex_string) {
    unsigned long val = std::stoul(*hex_string, 0, 16);
    return val;
}

/**
 * @brief reads bytes number of bytes, starting at ptr, from the file_data
 * 
 * @param ptr 
 * @param bytes 
 * @param file_data 
 * @return std::string* 
 */
std::string* BSV_BLOCK::read_bytes(uint32_t* ptr, int bytes, std::string* file_data) {
    std::string byte_string = (*file_data).substr(*ptr, bytes*2);
    *ptr += bytes*2; // increments ptr as necessary

    // convert to big-endian format in O(n) time
    std::string* big_endian_byte_string = new std::string;
    int local_ptr = bytes*2 - 1;
    while (local_ptr >= 0) {
        *big_endian_byte_string += byte_string.substr(local_ptr-1, 2);
        local_ptr -= 2;
    }
    
    return big_endian_byte_string;
}

/**
 * @brief reads a VarInt from the file_data, starting from ptr pointer
 * 
 * @param ptr 
 * @param file_data 
 * @return unsigned long 
 */
unsigned long BSV_BLOCK::read_variable_bytes(uint32_t* ptr, std::string* file_data) {
    std::string* hex_string = read_bytes(ptr, 1, file_data);
    unsigned long val = hex_to_int(hex_string);

    if (val == 253) {
        std::string* two_byte_hex_string = read_bytes(ptr, 2, file_data);
        unsigned long two_byte_val = hex_to_int(two_byte_hex_string);
        // delete two_byte_hex_string;
        return two_byte_val;
    }
    else if (val == 254) {
        std::string* four_byte_hex_string = read_bytes(ptr, 4, file_data);
        unsigned long four_byte_val = hex_to_int(four_byte_hex_string);
        // delete four_byte_hex_string;
        return four_byte_val;
    }
    else if (val == 255) {
        std::string* eight_byte_hex_string = read_bytes(ptr, 8, file_data);
        unsigned long eight_byte_val = hex_to_int(eight_byte_hex_string);
        // delete eight_byte_hex_string;
        return eight_byte_val;
    }
    else if (val > 255) {
        perror("VarInt greater than 255!");
        exit(1);
    }

    // delete hex_string;

    return val;
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

BSV_BLOCK::~BSV_BLOCK() {
    //TODO: implement destructor if necessary in the future
}