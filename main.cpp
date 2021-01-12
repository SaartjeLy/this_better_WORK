#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <time.h>
#include <experimental/filesystem>
#include "taskflow/taskflow/taskflow.hpp"

tf::Taskflow taskflow;
tf::Executor executor;

/**
 * @brief converts a binary string to a hex string, from https://stackoverflow.com/questions/9621893
 * 
 * @param s binary string
 * @param upper_case bool for whether alpha chars should be in uppercase or not
 * @return std::string 
 */
std::string ToHex(char* bin_string, int size) {
    std::ostringstream hex_string;

    for(int i=0; i<size; i++) {
        int z = bin_string[i] & 0xff;
        hex_string << std::hex << std::setfill('0') << std::setw(2) << z;
    }

    return hex_string.str();
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

    std::string hex_string_of_data = ToHex(buffer, size);
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
        std::size_t pos = file_path.find("blk0000");

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
    std::string byte_string = block_data.substr(from, from+bytes*2);

    // convert to big-endian format in O(n) time
    std::string big_endian_byte_string = "";
    int ptr = bytes*2 - 1;
    while (ptr >= from) {
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

int main(int argc, char* argv[]) {
    if(argc < 2) {
        perror("Provide path to block files as an arg when running main.o");
        exit(1);
    }

    std::string path = argv[1];

    std::cout << "getting all block numbers!" << std::endl;
    std::vector<std::string> block_numbers = get_all_block_numbers(path);
    std::cout << "finished getting all block numbers!" << std::endl;
    std::cout << std::endl;
    std::cout << "now parsing" << std::endl;
    time_t start = clock();

    taskflow.for_each(block_numbers.begin(), block_numbers.end(), [&] (std::string block_number) {
        std::string block_data = get_block_data(path, block_number);

        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> block = parse_block(block_data);
    });

    executor.run(taskflow).wait();
    taskflow.clear();
    
    time_t end = clock();
    std::cout << "finished parsing!\n" << std::endl;

    std::cout << "It took " << (double)(end-start)/CLOCKS_PER_SEC/10 << "s to parse " << block_numbers.size() << " blocks!" << std::endl;

    return 0;
}