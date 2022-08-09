#include "../include/csv_writer.h"
#include <iostream>
#include <filesystem>

CSV_WRITER::CSV_WRITER(std::string filename, bool twetches, bool transaction) : filename(filename) {
    // twetches is for whether the CSV file is meant for twetches or not
    // this definitely needs to be refactored, but it's my last day @ 21e8 and i'm rushing this feature lol

    if(std::filesystem::exists("./"+filename)){
        csv_file.open(filename, std::ios_base::app);
        return;
        }
    csv_file.open(filename, std::ios_base::app);
    
    if(transaction)
    {
        write_line("transaction_version,num_inputs,num_outputs,lock_time,TXID");
    }
    else if(std::filesystem::exists("./"+filename)) {
        write_line("block_number,block_hash,magic_number,block_size,version,previous_block_hash,merkle_root,time,difficulty,nonce,number_of_transactions"); 
    }
    else if (twetches == 0) {
        write_line("block_number,magic_number,block_size,version,previous_block_hash,merkle_root,time,bits,nonce");    
    }
    else {
        write_line("block_number,time,number_of_twetches");    
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
    line += block.block_hash + ",";
    line += block.magic_number + ",";
    line += block.block_size + ",";
    line += block.version + ",";
    line += block.previous_block_hash + ",";
    line += block.merkle_root + ",";
    line += block.time + ",";
    line += block.bits + ",";
    line += block.nonce + ",";
    line += std::to_string(block.number_of_transactions);

    write_line(line);
}
void CSV_WRITER::write_transaction(BSV_TRANSACTION transaction)
{
    std::string line;
    line += transaction.version + ",";
    line += std::to_string(transaction.number_of_inputs) + ",";
    line += std::to_string(transaction.number_of_outputs) + ",";
    line += transaction.lock_time + ",";
    line += transaction.TXID;

    write_line(line);
}

/**
 * @brief writes the number of twetches to the CSV file
 *        please remember that the data is read into the BSV_BLOCK in big-endian format, so is technically
          the "reverse" of the original .dat file, so the suffix and prefix are opposites (find suffix first
          then find prefix)
 * 
 * @param block_number 
 * @param block 
 */
void CSV_WRITER::write_twetch_count(uint32_t block_number, BSV_BLOCK* block) {
    std::string twetch_prefix = "7475416F64565A7A7031717951455563565170487433764279513456676978483931";
    std::string twetch_suffix = "61766643683763715357376956576155536F4D584A514C4E53323247486963503531";
    uint8_t prefix_length = twetch_prefix.length();
    uint8_t suffix_length = twetch_suffix.length();
    uint64_t twetch_count = 0;

    // first find the total number of twetches in this block
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

                    // check if output data has "twetch" in it, if so, write to file
                    size_t pos = output_data.find("686374657774");

                    if(pos != std::string::npos) {
                        twetch_count++; // found a twetch, so increment the twetch count!
                    }
                }
            }
        }
    }

    // now write this to the CSV file
    std::string line;

    line += std::to_string(block_number) + ",";
    line += std::to_string(std::stoi(block->time, 0, 16)) + ",";
    line += std::to_string(twetch_count);
    
    write_line(line);
}

void CSV_WRITER::close() {
    csv_file.close();
}