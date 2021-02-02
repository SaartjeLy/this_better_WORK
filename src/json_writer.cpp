#include "../include/json_writer.h"
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
void JSON_WRITER::write_hash(BSV_BLOCK block) {
    std::string hash = block.get_previous_block_hash();

    json_file << "\"";
    write_line(hash, false);
    json_file << "\",";
    write_line("", true);
}

/**
 * @brief close the json file
 * 
 */
void JSON_WRITER::close() {
    json_file << "]";
    json_file.close();
}