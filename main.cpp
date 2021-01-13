#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <time.h>
#include <chrono>
#include <experimental/filesystem>
#include "taskflow/taskflow/taskflow.hpp"

/**
 * @brief converts a binary string to a hex string, adapted from https://stackoverflow.com/a/10723475
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
 * @brief reads all the block data from the binary .dat block file and converts to a hex string
 * 
 * @param path the absolute path to the relevant directory
 * @param block the name of the block file
 * @return std::string 
 */
std::string get_block_data(std::string path, std::string block) {
    // open file
    std::ifstream file;
    file.open(path + block, std::ios::ate|std::ios::binary|std::ios::in);

    // exit if the file doesn't exist
    if(!file.is_open()) {
        perror("Error opening block");
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
std::vector<std::string> get_all_block_numbers(std::string path) {
    // reads all files in the blocks directory and returns an array of present blkXXXXX.dat filenames
    std::vector<std::string> block_numbers;

    for(const auto entry : std::experimental::filesystem::directory_iterator(path)) {
        std::string file_path = entry.path();

        // verifying the file is a .dat BLOCK file 
        std::size_t pos = file_path.find("blk");

        if (pos != std::string::npos) {
            block_numbers.push_back(file_path.substr(path.length(), file_path.length()));
        }
    }

    return block_numbers;
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
std::string read_bytes(int from, int bytes, std::string block_data) {
    std::string byte_string = block_data.substr(from, bytes*2);

    // convert to big-endian format in O(n) time
    std::string big_endian_byte_string = "";
    int ptr = bytes*2 - 1;
    while (ptr >= 0) {
        big_endian_byte_string += byte_string.substr(ptr-1, 2);
        ptr -= 2;
    }
    
    return big_endian_byte_string;
}

/**
 * @brief parses the preamble section of the BSV block and returns a dictionary representation
 * 
 * @param block_data the BSV block data
 * @return std::unordered_map<std::string, std::string> 
 */
std::unordered_map<std::string, std::string> parse_preamble(std::string block_data) {
    std::unordered_map<std::string, std::string> preamble;
    preamble["magic_number"] = read_bytes(0,4,block_data);
    preamble["block_size"] = read_bytes(8,4,block_data);

    return preamble;
}

/**
 * @brief parses the header section of the BSV block and returns a dictionary representation
 * 
 * @param block_data the BSV block data
 * @return std::unordered_map<std::string, std::string> 
 */
std::unordered_map<std::string, std::string> parse_header(std::string block_data) {
    std::unordered_map<std::string, std::string> header;
    header["version"] = read_bytes(16,4,block_data);
    header["previous_block_hash"] = read_bytes(24,32, block_data);
    header["merkle_root"] = read_bytes(88, 32, block_data);
    header["time"] = read_bytes(152, 4, block_data);
    header["bits"] = read_bytes(160, 4, block_data);
    header["nonce"] = read_bytes(168,4, block_data);
 
    return header;
}

/**
 * @brief parses the entire BSV block into its various sections and returns a dictionary representation
 * 
 * @param block_data the BSV block data
 * @return std::unordered_map<std::string, std::unordered_map<std::string, std::string>> 
 */
std::unordered_map<std::string, std::unordered_map<std::string, std::string>> parse_block(std::string block_data) {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> block;

    std::unordered_map<std::string, std::string> preamble = parse_preamble(block_data);
    std::unordered_map<std::string, std::string> header = parse_header(block_data);

    block["premable"] = preamble;
    block["header"] = header;

    //TODO: implement

    return block;
}

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

int main(int argc, char* argv[]) {
    tf::Taskflow taskflow;
    tf::Executor executor;

    if(argc < 2) {
        perror("Provide path to block files as an arg when running main.o");
        exit(1);
    }

    std::string path = argv[1];
    std::ios_base::sync_with_stdio(false);

    std::cout << "getting all block numbers!" << std::endl;
    std::vector<std::string> block_numbers = get_all_block_numbers(path);
    std::cout << "finished getting all block numbers!" << std::endl;
    std::cout << std::endl;
    std::cout << "now parsing" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    taskflow.for_each(block_numbers.begin(), block_numbers.end(), [&] (std::string block_number) {
        std::string block_data = get_block_data(path, block_number);

        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> block = parse_block(block_data);
    });

    executor.run(taskflow).wait();
    taskflow.clear();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    
    std::cout << "finished parsing!\n" << std::endl;
    std::cout << "It took " << duration << "ms to parse " << block_numbers.size() << " blocks!" << std::endl;

    return 0;
}