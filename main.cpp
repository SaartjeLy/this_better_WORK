#include <iostream>
#include <string>
#include <vector>
#include <bitset>
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
std::string ToHex(const std::string& s, bool upper_case) {
    std::ostringstream ret;

    for (std::string::size_type i = 0; i < s.length(); ++i)
    {
        int z = s[i]&0xff;
        ret << std::hex << std::setfill('0') << std::setw(2) << (upper_case ? std::uppercase : std::nouppercase) << z;
    }

    return ret.str();
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

    std::string hex_string_of_data = ToHex(std::string(buffer, size), true);

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

int main(int argc, char* argv[]) {
    if(argc < 2) {
        perror("Provide path to block files as an arg when running main.o");
        exit(1);
    }

    std::string path = argv[1];

    std::vector<std::string> block_numbers = get_all_block_numbers(path);

    for (std::string block : block_numbers) {
        std::string block_data = get_block_data(path, block);
        std::cout << block << ": " << block_data.substr(0,8)<< std::endl;
    }

    return 0;
}