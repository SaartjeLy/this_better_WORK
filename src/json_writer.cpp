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
void JSON_WRITER::write_hash(BSV_BLOCK* block) {
    json_file << "\"";
    write_line(block->previous_block_hash, false);
    json_file << "\",";
    write_line("", true);
}

/**
 * @brief converts a big endian hex string to little endian, then converts the
          little endian string to equivalent ascii and returns it
 * 
 * @param big_endian_string 
 * @return std::string 
 */
std::string JSON_WRITER::get_ascii(std::string big_endian_string) {
    // convert to little endian in O(n) time
    std::string little_endian_string;
    int local_ptr = big_endian_string.size() - 1;
    while (local_ptr >= 0) {
        std::string big_endian_substring = big_endian_string.substr(local_ptr-1, 2);

        // if this character is a quote mark then insert a forward slash first, to escape them
        if (big_endian_substring == "22") {
            little_endian_string += "5C"; // this is hex for a forward slash char
        }
        little_endian_string += big_endian_substring;
        local_ptr -= 2;
    }

    // convert little endian string to ascii (this can be sped up significantly)
    std::string ascii_string;
    for(int i=0; i<little_endian_string.size(); i+=2) {
            char ascii_char = std::stoi(little_endian_string.substr(i,2), 0, 16);
            ascii_string.push_back(ascii_char);
    }

    return ascii_string;
}

/**
 * @brief looks through each output script for all transactions in a block and finds the twetches,
          writing them to json
          please remember that the data is read into the BSV_BLOCK in big-endian format, so is technically
          the "reverse" of the original .dat file, so the suffix and prefix are opposites (find suffix first
          then find prefix)
 * 
 * @param block 
 */
void JSON_WRITER::write_twetches(BSV_BLOCK* block) {
    std::string twetch_prefix = "7475416F64565A7A7031717951455563565170487433764279513456676978483931";
    std::string twetch_suffix = "61766643683763715357376956576155536F4D584A514C4E53323247486963503531";
    uint8_t prefix_length = twetch_prefix.length();
    uint8_t suffix_length = twetch_suffix.length();

    for(BSV_TRANSACTION transaction : block->transactions) {
        for(BSV_OUTPUT output : transaction.outputs) {
            uint32_t local_ptr = 0;
            std::size_t suffix_pos = output.script.find(twetch_suffix, local_ptr); // starting ptr to suffix
            
            if (suffix_pos != std::string::npos) {
                local_ptr = suffix_pos+suffix_length; // increment local_ptr to end of suffix

                std::size_t prefix_pos = output.script.find(twetch_prefix, local_ptr); // starting ptr to prefix
                if (prefix_pos != std::string::npos) {
                    local_ptr = prefix_pos+prefix_length; // increment local_ptr to end of suffix
                    size_t output_data_length = prefix_pos+prefix_length - suffix_pos; // length is whatever is between prefix & suffix (inclusive)
                    std::string output_data = output.script.substr(suffix_pos, output_data_length);

                    // check if output data has "twetch" in it, if so, write data to file
                    size_t pos = output_data.find("686374657774");

                    if(pos != std::string::npos) {
                        std::string ascii_string = get_ascii(output_data);

                        json_file << "\"";
                        write_line(ascii_string, false);
                        json_file << "\",";
                        write_line("", true);
                    }
                }
            }
        }
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