#include "../include/bsv_block.h"

BSV_BLOCK::BSV_BLOCK(uint32_t* ptr, std::string* data) : file_data(data), ptr(ptr) {
    parse_block();
}

void BSV_BLOCK::parse_block() {
    // preamble
    magic_number = read_bytes(4);
    block_size = read_bytes(4);

    // header
    version = read_bytes(4);
    previous_block_hash = read_bytes(32);
    merkle_root = read_bytes(32);
    time = read_bytes(4);
    bits = read_bytes(4);
    nonce = read_bytes(4);

    // transactions
    number_of_transactions = read_variable_bytes();

    for (int i=0; i<number_of_transactions; i++) {
        std::string version = read_bytes(4);

        unsigned long number_of_inputs = read_variable_bytes();
        for( int x=0; x<number_of_inputs; x++) {
            std::string pre_transaction_has = read_bytes(32);
            std::string pre_transactions_out_index = read_bytes(4);

            unsigned long script_length = read_variable_bytes();
            std::string script_length_string = std::to_string(script_length);

            std::string script = read_bytes(script_length);
            std::string sequence = read_bytes(4);
        }

        unsigned long number_of_outputs = read_variable_bytes();
        for( int y=0; y<number_of_outputs; y++) {
            std::string value = read_bytes(8);

            unsigned long script_length = read_variable_bytes();
            std::string script_length_string = std::to_string(script_length);

            std::string script = read_bytes(script_length);
        }
    
        std::string lock_time = read_bytes(4);
    }
}

/**
 * @brief takes a hex string and converts it to its equivalent unsigned long value
 * 
 * @param hex_string 
 * @return unsigned long 
 */
unsigned long BSV_BLOCK::hex_to_int(std::string hex_string) {
    unsigned long val = std::stoul(hex_string, 0, 16);
    return val;
}

/**
 * @brief reads bytes number of bytes, starting at ptr, from the file_data
 * 
 * @param bytes 
 * @return std::string* 
 */
std::string BSV_BLOCK::read_bytes(int bytes) {
    std::string byte_string = (*file_data).substr(*ptr, bytes*2);
    *ptr += bytes*2; // increments ptr as necessary

    // convert to big-endian format in O(n) time
    std::string big_endian_byte_string;
    int local_ptr = bytes*2 - 1;
    while (local_ptr >= 0) {
        big_endian_byte_string += byte_string.substr(local_ptr-1, 2);
        local_ptr -= 2;
    }
    
    return big_endian_byte_string;
}

/**
 * @brief reads a VarInt from the file_data, starting from ptr pointer
 * 
 * @return unsigned long 
 */
unsigned long BSV_BLOCK::read_variable_bytes() {
    std::string hex_string = read_bytes(1);
    unsigned long val = hex_to_int(hex_string);

    if (val == 253) {
        std::string two_byte_hex_string = read_bytes(2);
        unsigned long two_byte_val = hex_to_int(two_byte_hex_string);
        return two_byte_val;
    }
    else if (val == 254) {
        std::string four_byte_hex_string = read_bytes(4);
        unsigned long four_byte_val = hex_to_int(four_byte_hex_string);
        return four_byte_val;
    }
    else if (val == 255) {
        std::string eight_byte_hex_string = read_bytes(8);
        unsigned long eight_byte_val = hex_to_int(eight_byte_hex_string);
        return eight_byte_val;
    }
    else if (val > 255) {
        perror("VarInt greater than 255!");
        exit(1);
    }

    return val;
}

BSV_BLOCK::~BSV_BLOCK() {
    //TODO: implement destructor if necessary in the future
}