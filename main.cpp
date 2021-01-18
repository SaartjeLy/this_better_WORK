#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <time.h>
#include <chrono>
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
std::string get_hex(char* binary_string, int size) {
    char const hex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    std::string hex_string;

    for(int i=0; i<size; i++) {
        const char ch = binary_string[i];
        hex_string.append(&hex[(ch & 0xF0) >> 4], 1); // append MSH/MSB of current char to hex string
        hex_string.append(&hex[ch & 0xF], 1); // append LSH/LSB of current char to hex string
    }

    return hex_string;
}

/**
 * @brief reads all the file data from the binary .dat blk file and converts to a hex string
 * 
 * @param path the absolute path to the relevant directory
 * @param block the name of the blk file
 * @return std::string 
 */
std::string get_file_data(std::string path, std::string file_name) {
    // open file
    std::ifstream file;
    file.open(path + file_name, std::ios::ate|std::ios::binary|std::ios::in);

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

    std::string hex_string_of_data = get_hex(buffer, size);
    free(buffer);

    return hex_string_of_data;
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
        std::size_t pos = file_path.find("blk00000");

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
std::string read_bytes(uint32_t* ptr, int bytes, std::string block_data) {
    std::string byte_string = block_data.substr(*ptr, bytes*2);
    *ptr += bytes*2; // increments ptr as necessary

    // convert to big-endian format in O(n) time
    std::string big_endian_byte_string = "";
    int local_ptr = bytes*2 - 1;
    while (local_ptr >= 0) {
        big_endian_byte_string += byte_string.substr(local_ptr-1, 2);
        local_ptr -= 2;
    }
    
    return big_endian_byte_string;
}

/**
 * @brief prints the bsv block to stdio
 *  
 * @param block the unordered map representing the parsed bsv block
 */
void pretty_print(std::unordered_map<std::string, std::unordered_map<std::string, std::string>> block) {
    std::cout << "{\n";
    for (auto const& pair : block) {
        std::cout << " {" << pair.first << ":\n";
        for (auto const& sub_pair : pair.second) {
            std::cout << "\t{" << sub_pair.first << ": " << sub_pair.second << "}\n";
        }
        std::cout << " }" << std::endl;
    }
    std::cout << "}\n" << std::endl;
}

/**
 * @brief parses the preamble section of the BSV block and returns a dictionary representation
 * 
 * @param block_data the BSV block data
 * @return std::unordered_map<std::string, std::string> 
 */
std::unordered_map<std::string, std::string> parse_preamble(uint32_t* ptr, std::string block_data) {
    std::unordered_map<std::string, std::string> preamble;
    preamble["magic_number"] = read_bytes(ptr,4,block_data);
    preamble["block_size"] = read_bytes(ptr,4,block_data);

    return preamble;
}

/**
 * @brief parses the header section of the BSV block and returns a dictionary representation
 * 
 * @param block_data the BSV block data
 * @return std::unordered_map<std::string, std::string> 
 */
std::unordered_map<std::string, std::string> parse_header(uint32_t* ptr, std::string block_data) {
    std::unordered_map<std::string, std::string> header;
    header["version"] = read_bytes(ptr,4,block_data);
    header["previous_block_hash"] = read_bytes(ptr,32, block_data);
    header["merkle_root"] = read_bytes(ptr, 32, block_data);
    header["time"] = read_bytes(ptr, 4, block_data);
    header["bits"] = read_bytes(ptr, 4, block_data);
    header["nonce"] = read_bytes(ptr,4, block_data);
 
    return header;
}

/**
 * @brief parses a BSV block into its various sections and returns a dictionary representation
 * 
 * @param ptr 
 * @param file_data 
 * @return std::unordered_map<std::string, std::unordered_map<std::string, std::string>> 
 */
std::unordered_map<std::string, std::unordered_map<std::string, std::string>> parse_block(uint32_t* ptr, std::string file_data) {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> block;
    
    std::unordered_map<std::string, std::string> preamble = parse_preamble(ptr, file_data);
    std::unordered_map<std::string, std::string> header = parse_header(ptr, file_data);

    block["preamble"] = preamble;
    block["header"] = header;

    //TODO: implement the rest of the block parsing

    return block;
}

/**
 * @brief parses a blk file into its multiple BSV blocks, writing returning a vector with all blocks
 * 
 * @param block_data the BSV block data
 * @return std::unordered_map<std::string, std::unordered_map<std::string, std::string>> 
 */
void parse_file(CSV_WRITER* csv_writer, std::string file_name, std::string file_data) {
    uint32_t ptr = 0;
    std::vector<std::unordered_map<std::string, std::unordered_map<std::string, std::string>>> blocks_in_file; // vector of all blocks in the file

    while (ptr < file_data.length()) {
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> block = parse_block(&ptr, file_data);
        blocks_in_file.push_back(block);
        (*csv_writer).write_block(file_name, block); // write block to csv file
    }
}


/**
 * @brief parses the bsv blocks and exports to CSV
 * 
 * @param path absolute path to the directory holding the .dat bsv block files
 * @param block_numbers string vector of the .dat filenames
 */
void parse_and_export(std::string path, std::vector<std::string> file_names) {
    tf::Taskflow taskflow;
    tf::Executor executor;
    
    CSV_WRITER csv_writer("bitcoinsv.csv"); // create csv file and open file stream

    taskflow.for_each(file_names.begin(), file_names.end(), [&] (std::string file_name) {
        std::string file_data = get_file_data(path, file_name);

        parse_file(&csv_writer, file_name, file_data);
    });

    executor.run(taskflow).wait();
    taskflow.clear();

    csv_writer.close(); // close the csv file here
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

    parse_and_export(path, file_names);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    
    std::cout << "finished parsing!\n" << std::endl;
    std::cout << "It took " << duration << "ms to parse " << file_names.size() << " files!" << std::endl;

    return 0;
}