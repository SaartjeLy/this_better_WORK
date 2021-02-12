#include <string>
#include <unordered_map>
#include <any>
#include <fstream>
#include <mutex>
#include "../include/bsv_block.h"

#ifndef CSV_WRITER_H
#define CSV_WRITER_H

class CSV_WRITER {
    private:
        std::ofstream csv_file;
        std::string filename;
        void write_line(std::string line);
    public:
        std::mutex mutex;
        CSV_WRITER(std::string filename, bool twetches);
        void write_header(uint32_t block_number, BSV_BLOCK block);
        void write_twetch_count(uint32_t block_number, BSV_BLOCK* block);
        void close();
};

#endif