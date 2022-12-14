#include <string>
#include <unordered_map>
#include <any>
#include <fstream>
#include <mutex>
#include "../include/bsv_block.h"

#ifndef CSV_WRITER_H
#define CSV_WRITER_H

enum CSV_WRITE_FLAG {NONE, INPUT, OUTPUT, TRANSACTION};
class CSV_WRITER {
    private:
        std::ofstream csv_file;
        std::string filename;
        void write_line(std::string line);
    public:
        std::mutex mutex;
        CSV_WRITER(std::string filename, bool twetches, CSV_WRITE_FLAG flag = NONE);
        void write_header(uint32_t block_number, BSV_BLOCK block);
        void write_transaction(BSV_TRANSACTION transaction);
        void write_output(BSV_OUTPUT output);
        void write_input(BSV_INPUT input);

        void write_twetch_count(uint32_t block_number, BSV_BLOCK* block);
        void close();
};

#endif