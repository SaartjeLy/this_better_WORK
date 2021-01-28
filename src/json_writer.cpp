#include "json_writer.h"
#include <iostream>
#include <filesystem>

JSON_WRITER::JSON_WRITER(std::string filename) : filename(filename) {
    if(std::filesystem::exists("./"+filename)) {
        json_file.open(filename, std::ios_base::app);
    }
    else {
        json_file.open(filename, std::ios_base::app);
        write_line("[", false); // write opening bracket to the json array
    }
}

/**
 * @brief write a line to the json file
 * 
 * @param line 
 * @param new_line 
 */
void JSON_WRITER::write_line(std::string line, bool new_line) {
    mutex.lock();

    json_file << line;
    if (new_line) {
        json_file << "\n";
    }

    mutex.unlock();
}

/**
 * @brief writes hash line to the json file in the following format, with a new line character on the end:
 *        "<hash_here>",
 * 
 * @param hash 
 */
void JSON_WRITER::write_hash(std::unordered_map<std::string, std::any>* block) {
    std::unordered_map<std::string, std::any>::iterator header_it = (*block).find("header");
    if (header_it != (*block).end()) {
        std::unordered_map<std::string, std::string> header = std::any_cast<std::unordered_map<std::string, std::string>>(header_it->second);
        
        // probably need more error checks here
        std::string hash = header.find("previous_block_hash")->second;

        json_file << "\"";
        write_line(hash, false);
        json_file << "\",";
        write_line("", true);

    }
    else {
        perror("Can't find header in block");
        exit(1);
    }
}

/**
 * @brief close the json file
 * 
 */
void JSON_WRITER::close() {
    json_file << "]";
    json_file.close();
}