#include <string>
#include <unordered_map>
#include <any>
#include <fstream>
#include <mutex>

#ifndef CSV_WRITER_H
#define CSV_WRITER_H

class CSV_WRITER {
    private:
        std::ofstream csv_file;
        std::string filename;
        void write_line(std::string line);
    public:
        std::mutex mutex;
        CSV_WRITER(std::string filename);
        void write_block(std::string block_number, std::unordered_map<std::string, std::any>* block);
        void close();
};

#endif