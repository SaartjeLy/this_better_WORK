#include "../include/csv_writer.h"
#include <iostream>
#include <filesystem>

CSV_WRITER::CSV_WRITER(std::string filename) : filename(filename) {
    if(std::filesystem::exists("./"+filename)) {
        csv_file.open(filename, std::ios_base::app);
    }
    else {
        csv_file.open(filename, std::ios_base::app);
        write_line("magic_number,block_size,version,previous_block_hash,merkle_root,time,bits,nonce");    
    }
}

/**
 * @brief writes a line to the CSV file
 * 
 * @param line the line of comma separated data to be written to the CSV
 */
void CSV_WRITER::write_line(std::string line) {
    mutex.lock();
    csv_file << line << "\n";
    mutex.unlock();
}

/**
 * @brief writes a block preamble and header to csv file in a single line
 * 
 * @param block_number the block number
 * @param block unordered map of block data
 */
void CSV_WRITER::write_block(std::string block_number, std::unordered_map<std::string, std::any>* block) {
    std::string* line = new std::string;

    std::unordered_map<std::string, std::any>::iterator preamble_it = (*block).find("preamble");
    if (preamble_it != (*block).end()) {
        std::unordered_map<std::string, std::string> preamble = std::any_cast<std::unordered_map<std::string, std::string>>(preamble_it->second);

        // probably need more error checks here

        *line += preamble.find("magic_number")->second + ","; 
        *line += preamble.find("block_size")->second + ","; 
    }
    else {
        perror("Can't find preamble in block");
        exit(1);
    }

    std::unordered_map<std::string, std::any>::iterator header_it = (*block).find("header");
    if (header_it != (*block).end()) {
        std::unordered_map<std::string, std::string> header = std::any_cast<std::unordered_map<std::string, std::string>>(header_it->second);
        
        // probably need more error checks here

        *line += header.find("version")->second + ",";
        *line += header.find("previous_block_hash")->second + ","; 
        *line += header.find("merkle_root")->second + ","; 
        *line += header.find("time")->second + ","; 
        *line += header.find("bits")->second + ","; 
        *line += header.find("nonce")->second;

    }
    else {
        perror("Can't find header in block");
        exit(1);
    }

    write_line(*line);
    
    delete line;
}

void CSV_WRITER::close() {
    csv_file.close();
}