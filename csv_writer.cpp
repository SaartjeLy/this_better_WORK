#include "csv_writer.h"
#include <iostream>

CSV_WRITER::CSV_WRITER(std::string filename) : filename(filename) {
    csv_file.open(filename);
    write_line("block_number, magic_number, block_size, version, previous_block_hash, merkle_root, time, bits, nonce");
}

/**
 * @brief writes a line to the CSV file
 * 
 * @param line the line of comma separated data to be written to the CSV
 */
void CSV_WRITER::write_line(std::string line) {
    csv_file << line << "\n";
}

/**
 * @brief writes a block preamble and header to csv file in a single line
 * 
 * @param block_number the block number
 * @param block unordered map of block data
 */
void CSV_WRITER::write_block(std::string block_number, std::unordered_map<std::string, std::unordered_map<std::string, std::string>> block) {
    std::string line;
    std::string magic_number;
    std::string block_size;
    std::string version;
    std::string previous_block_hash;
    std::string merkle_root;
    std::string time;
    std::string bits;
    std::string nonce; 

    std::unordered_map<std::string, std::unordered_map<std::string, std::string>>::iterator preamble_it = block.find("preamble");
    if (preamble_it != block.end()) {
        std::unordered_map<std::string, std::string> preamble = preamble_it->second;

        // probably need more error checks here
        magic_number = preamble.find("magic_number")->second;
        block_size = preamble.find("block_size")->second;
    }
    else {
        perror("Can't find preamble in block");
        exit(1);
    }

    std::unordered_map<std::string, std::unordered_map<std::string, std::string>>::iterator header_it = block.find("header");
    if (header_it != block.end()) {
        std::unordered_map<std::string, std::string> header = header_it->second;
        // probably need more error checks here
        version = header.find("version")->second;
        previous_block_hash = header.find("previous_block_hash")->second;
        merkle_root = header.find("merkle_root")->second;
        time = header.find("time")->second;
        bits = header.find("bits")->second;
        nonce = header.find("nonce")->second;
    }
    else {
        perror("Can't find header in block");
        exit(1);
    }

    // construct line to be written to the csv file
    line = block_number + "," + magic_number + "," + block_size + "," + version + "," + previous_block_hash +\
             "," + merkle_root + "," + time + "," + bits + "," + nonce;

    write_line(line);
}

void CSV_WRITER::close() {
    csv_file.close();
}