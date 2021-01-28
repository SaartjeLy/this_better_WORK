#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <experimental/filesystem>
#include "taskflow/taskflow/taskflow.hpp"
#include "csv_writer.h"

/**
 * @brief uses bit-shifting to convert a binary string to a hex string, adapted from https://stackoverflow.com/a/10723475
 * 
 * @param binary_string the binary string
 * @param size the length of the binary string
 * @return std::string 
 */
std::string* get_hex(char* binary_string, int size) {
    char const hex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    std::string* hex_string = new std::string;

    for(int i=0; i<size; i++) {
        const char ch = binary_string[i];
        (*hex_string).append(&hex[(ch & 0xF0) >> 4], 1); // append MSH/MSB of current char to hex string
        (*hex_string).append(&hex[ch & 0xF], 1); // append LSH/LSB of current char to hex string
    }

    return hex_string;
}

/**
 * @brief 
 * 
 * @param hex_string 
 * @return int 
 */
unsigned long hex_to_int(std::string* hex_string) {
    unsigned long val = std::stoul(*hex_string, 0, 16);
    return val;
}

/**
 * @brief reads a number (to - from) of files in a parallelised way, storing the file data in a vector collection of strings
 * 
 * @param from 
 * @param to 
 * @param path 
 * @param file_names 
 * @param collection 
 */
void get_file_data(int from, int to, std::string path, std::vector<std::string> file_names, std::vector<std::string*>* collection) {
    std::mutex mutex;
    
    tf::Taskflow taskflow;
    tf::Executor executorA;

    taskflow.for_each_index(from, to, 1, [&] (int i) {
        // open file
        std::ifstream file;
        file.open(path + file_names[i], std::ios::ate|std::ios::binary|std::ios::in);

        // exit if the file doesn't exist
        if(!file.is_open()) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }

        // read input (each char is in 2's comp format)
        std::ifstream::pos_type size = file.tellg();
        char* buffer = new char[size];
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        std::string* hex_string_of_data = get_hex(buffer, size);
        delete [] buffer;
        
        mutex.lock();
        (*collection).push_back(hex_string_of_data);
        mutex.unlock();
    });

    executorA.run(taskflow).wait();
    taskflow.clear();

    std::cout << to << " DONE!" << std::endl;
}

/**
 * @brief gets a vector of all the block file names
 * 
 * @param path the absolute path to the directory with all the block files
 * @return std::vector<std::string> 
 */
std::vector<std::string> get_all_file_names(std::string path) {
    // reads all files in the blocks directory and returns an array of present blkXXXXX.dat filenames
    std::vector<std::string> file_names;

    for(const auto entry : std::experimental::filesystem::directory_iterator(path)) {
        std::string file_path = entry.path();

        // verifying the file is a .dat BLOCK file 
        std::size_t pos = file_path.find("blk");

        if (pos != std::string::npos) {
            file_names.push_back(file_path.substr(path.length(), file_path.length()));
        }
    }

    return file_names;
}

/**
 * @brief reads some number of bytes from the block data, starting at a particular
 *        pointer, into a buffer then converts this the big-endian format in O(n) time
 * 
 * @param from pointer to start reading from
 * @param bytes number of bytes to read
 * @param block_data the block data to read from
 * @return std::string 
 */
std::string* read_bytes(uint32_t* ptr, int bytes, std::string* file_data) {
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
 * @brief reads a VarInt as per BSV block specs:
 *        252 or less, read 1 byte
 *        253, read 2 bytes
 *        254, read 4 bytes
 *        255, read 8 bytes
 * 
 * @param ptr 
 * @param bytes 
 * @param block_data 
 * @return int
 */
unsigned long read_variable_bytes(uint32_t* ptr, std::string* file_data) {
    std::string* hex_string = read_bytes(ptr, 1, file_data);
    unsigned long val = hex_to_int(hex_string);

    if (val == 253) {
        std::string* two_byte_hex_string = read_bytes(ptr, 2, file_data);
        unsigned long two_byte_val = hex_to_int(two_byte_hex_string);
        delete two_byte_hex_string;
        return two_byte_val;
    }
    else if (val == 254) {
        std::string* four_byte_hex_string = read_bytes(ptr, 4, file_data);
        unsigned long four_byte_val = hex_to_int(four_byte_hex_string);
        delete four_byte_hex_string;
        return four_byte_val;
    }
    else if (val == 255) {
        std::string* eight_byte_hex_string = read_bytes(ptr, 8, file_data);
        unsigned long eight_byte_val = hex_to_int(eight_byte_hex_string);
        delete eight_byte_hex_string;
        return eight_byte_val;
    }
    else if (val > 255) {
        perror("VarInt greater than 255!");
        exit(1);
    }

    delete hex_string;

    return val;
}

/**
 * @brief parses the preamble section of the BSV block and returns a dictionary representation
 * 
 * @param block_data the BSV block data
 * @return std::unordered_map<std::string, std::string> 
 */
std::unordered_map<std::string, std::string> parse_preamble(uint32_t* ptr, std::string* block_data) {
    std::unordered_map<std::string, std::string> preamble;
    preamble["magic_number"] = *read_bytes(ptr,4,block_data);
    preamble["block_size"] = *read_bytes(ptr,4,block_data);

    return preamble;
}

/**
 * @brief parses the header section of the BSV block and returns a dictionary representation
 * 
 * @param block_data the BSV block data
 * @return std::unordered_map<std::string, std::string> 
 */
std::unordered_map<std::string, std::string> parse_header(uint32_t* ptr, std::string* block_data) {
    std::unordered_map<std::string, std::string> header;
    header["version"] = *read_bytes(ptr,4,block_data);
    header["previous_block_hash"] = *read_bytes(ptr,32, block_data);
    header["merkle_root"] = *read_bytes(ptr, 32, block_data);
    header["time"] = *read_bytes(ptr, 4, block_data);
    header["bits"] = *read_bytes(ptr, 4, block_data);
    header["nonce"] = *read_bytes(ptr,4, block_data);
 
    return header;
}

/**
 * @brief 
 * 
 * @param ptr 
 * @param file_data 
 * @return std::vector<std::unordered_map<std::string, std::any>> 
 */
std::vector<std::unordered_map<std::string, std::any>> parse_transactions(uint32_t* ptr, std::string* file_data) {
    std::vector<std::unordered_map<std::string, std::any>> transactions;

    unsigned long number_of_transactions = read_variable_bytes(ptr, file_data);
    for(int i=0; i< number_of_transactions; i++) {
        std::unordered_map<std::string, std::any> transaction;

        transaction["version"] = *read_bytes(ptr, 4, file_data);

        unsigned long number_of_inputs = read_variable_bytes(ptr, file_data);
        std::vector<std::unordered_map<std::string, std::any>> transaction_inputs;
        for(int x=0; x < number_of_inputs; x++) {
            std::unordered_map<std::string, std::any> transaction_input;
            
            transaction_input["pre_transaction_hash"] = *read_bytes(ptr, 32, file_data);
            transaction_input["pre_transaction_out_index"] = *read_bytes(ptr, 4, file_data);

            unsigned long script_length = read_variable_bytes(ptr, file_data);

            transaction_input["script_length"] = script_length;

            std::string script = *read_bytes(ptr, script_length, file_data);

            transaction_input["script"] = script;
            transaction_input["sequence"] = *read_bytes(ptr, 4, file_data);

            transaction_inputs.push_back(transaction_input);
        }

        unsigned long number_of_outputs = read_variable_bytes(ptr, file_data);
        std::vector<std::unordered_map<std::string, std::any>> transaction_outputs;
        for(int y=0; y < number_of_outputs; y++) {
            std::unordered_map<std::string, std::any> transaction_output;

            transaction_output["value"] = *read_bytes(ptr, 8, file_data);

            unsigned long script_length = read_variable_bytes(ptr, file_data);
            transaction_output["script_length"] = script_length;

            transaction_output["script"] = *read_bytes(ptr, script_length, file_data);

            transaction_outputs.push_back(transaction_output);
        }

        transaction["transaction_inputs"] = transaction_inputs;
        transaction["transaction_outputs"] = transaction_outputs;
        transaction["lock_time"] = *read_bytes(ptr, 4, file_data);

        transactions.push_back(transaction);
    }

    return transactions;
}

/**
 * @brief parses a BSV block into its various sections and returns a dictionary representation
 * 
 * @param ptr 
 * @param file_data 
 * @return std::unordered_map<std::string, std::any>
 */
std::unordered_map<std::string, std::any>* parse_block(uint32_t* ptr, std::string* file_data) {
    std::unordered_map<std::string, std::any>* block = new std::unordered_map<std::string, std::any>;
    
    std::unordered_map<std::string, std::string> preamble = parse_preamble(ptr, file_data);
    std::unordered_map<std::string, std::string> header = parse_header(ptr, file_data);
    std::vector<std::unordered_map<std::string, std::any>> transactions = parse_transactions(ptr, file_data);

    (*block)["preamble"] = preamble;
    (*block)["header"] = header;
    (*block)["transactions"] = transactions;

    return block;
}

/**
 * @brief parse all files in the file data vector and write block headers to csv
 * 
 * @param file_names 
 * @param vector_of_file_data 
 */
void parse_and_export_to_csv(std::vector<std::string> file_names, std::vector<std::string*> vector_of_file_data) {
    tf::Taskflow taskflow;
    tf::Executor executor;

    CSV_WRITER csv_writer("bitcoinsv.csv"); // create csv file and open file stream)

    tf::Task D = taskflow.for_each(vector_of_file_data.begin(), vector_of_file_data.end(), [&] (std::string* file_data) {
        uint32_t ptr = 0;
        uint32_t block_count = 0;

        while (ptr < (*file_data).length()) {
            block_count++;
            std::unordered_map<std::string, std::any>* block = parse_block(&ptr, file_data);

            csv_writer.write_block("blkxxxxx.dat", block); // write block to csv file

            delete block; // dealloc memory for block
        }
    });

    executor.run(taskflow).wait();
    taskflow.clear();
}

/**
 * @brief parses the bsv blocks and exports to CSV
 * 
 * @param path absolute path to the directory holding the .dat bsv block files
 * @param block_numbers string vector of the .dat filenames
 */
void read_files_and_parse(std::string path, std::vector<std::string> file_names) {
    std::vector<std::string*> vector_of_file_data;

    get_file_data(0, 1, path, file_names, &vector_of_file_data);

    std::cout << "FINISHED READING FILES INTO MEMORY" << std::endl;

    parse_and_export_to_csv(file_names, vector_of_file_data);
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        perror("Provide path to block files as an arg when running main.o");
        exit(1);
    }

    std::string path = argv[1];
    std::ios_base::sync_with_stdio(false);

    std::cout << "getting all file names!" << std::endl;
    std::vector<std::string> file_names = get_all_file_names(path);
    std::cout << "finished getting all file names!\n" << std::endl;

    std::cout << "now parsing" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    read_files_and_parse(path, file_names);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    
    std::cout << "finished parsing!\n" << std::endl;
    std::cout << "It took " << duration << "ms to parse " << file_names.size() << " files!" << std::endl;

    return 0;
}