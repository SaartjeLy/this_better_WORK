#include <string>
#include <unordered_map>
#include <any>
#include <fstream>
#include <mutex>
#include "../include/bsv_block.h"

#ifndef JSON_WRITER_H
#define JSON_WRITER_H

class JSON_WRITER {
    private:
        std::ofstream json_file;
        std::string filename;
        std::string get_ascii(std::string big_endian_string);
    public:
        std::mutex mutex;
        JSON_WRITER(std::string filename);
        void write_line(std::string line, bool new_line);
        void write_hash(BSV_BLOCK* block);
        void write_twetches(BSV_BLOCK* block);
        void close();
};

#endif