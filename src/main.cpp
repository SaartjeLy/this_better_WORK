#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <experimental/filesystem>
#include "taskflow/taskflow/taskflow.hpp"
#include "../include/csv_writer.h"
#include "../include/json_writer.h"
#include "../include/bsv_block.h"

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
            BSV_BLOCK block(&ptr, file_data);

            //TODO: re-implement write_block for CSV_WRITER class
            // csv_writer.write_block("blkxxxxx.dat", block); // write block to csv file

            // delete block; // dealloc memory for block
        }
    });

    executor.run(taskflow).wait();
    taskflow.clear();

    csv_writer.close();
}

/**
 * @brief parse all files in the file data vector and write the previous block hash to a json file
 * 
 * @param file_names 
 * @param vector_of_file_data 
 */
void parse_and_export_to_json(std::vector<std::string> file_names, std::vector<std::string*> vector_of_file_data) {
    tf::Taskflow taskflow;
    tf::Executor executor;
    std::mutex mutex;

    JSON_WRITER json_writer("bitcoinsv.json"); // create json file and open file stream)

    taskflow.for_each(vector_of_file_data.begin(), vector_of_file_data.end(), [&] (std::string* file_data) {
        uint32_t ptr = 0;
        uint32_t block_count = 0;

        while (ptr < (*file_data).length()) {
            block_count++;
            BSV_BLOCK block(&ptr, file_data);
            
            mutex.lock();
            json_writer.write_hash(block); // write block to json file
            mutex.unlock();
        }
    });

    executor.run(taskflow).wait();
    taskflow.clear();

    json_writer.close();
}

/**
 * @brief parses the bsv blocks and exports to CSV
 * 
 * @param path absolute path to the directory holding the .dat bsv block files
 * @param block_numbers string vector of the .dat filenames
 */
void read_files_and_parse(std::string path, std::vector<std::string> file_names) {
    std::vector<std::string*> vector_of_file_data;

    get_file_data(0, 10, path, file_names, &vector_of_file_data);

    std::cout << "FINISHED READING FILES INTO MEMORY" << std::endl;

    parse_and_export_to_json(file_names, vector_of_file_data);
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