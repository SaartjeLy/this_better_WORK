#include "../include/csv_writer.h"
#include <iostream>
#include <filesystem>

CSV_WRITER::CSV_WRITER(std::string filename) : filename(filename) {
    if(std::filesystem::exists("./"+filename)) {
        csv_file.open(filename, std::ios_base::app);
    }
    else {
        csv_file.open(filename, std::ios_base::app);
        write_line("block_number,magic_number,block_size,version,previous_block_hash,merkle_root,time,bits,nonce");    
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
void CSV_WRITER::write_header(uint32_t block_number, BSV_BLOCK block) {
    std::string line;

    line += std::to_string(block_number) + ",";
    line += block.magic_number + ",";
    line += block.block_size + ",";
    line += block.version + ",";
    line += block.previous_block_hash + ",";
    line += block.merkle_root + ",";
    line += block.time + ",";
    line += block.nonce + ",";

    write_line(line);
}

void CSV_WRITER::close() {
    csv_file.close();
}