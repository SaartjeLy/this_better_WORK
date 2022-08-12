#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <filesystem>
#include "taskflow/taskflow/taskflow.hpp"
#include "../include/csv_writer.h"
#include "../include/json_writer.h"
#include "../include/bsv_block.h"

/**
 * @brief uses bit-shifting to convert a binary string to a hex string, adapted from https://stackoverflow.com/a/10723475
 * 
 * @param binary_string the binary string
 * @param size the length of the binary string
 * @return std::string 
 */
std::string* get_hex(char* binary_string, int size);

/**
 * @brief reads a number (to - from) of files in a parallelised way, storing the file data in a vector collection of strings
 * 
 * @param from 
 * @param to 
 * @param path 
 * @param file_names 
 * @param collection 
 */
void get_file_data(int from, int to, std::string path, std::vector<std::string> file_names, std::vector<std::string*>* collection);

/**
 * @brief reads a file storing the file data in a vector collection of strings
 * 
 * @param path 
 * @param file_names 
 * @param collection 
 */
void get_file_data(std::string path, std::vector<std::string> file_names, std::vector<std::string*>* collection);

/**
 * @brief gets a vector of all the block file names
 * 
 * @param path the absolute path to the directory with all the block files
 * @return std::vector<std::string> 
 */
std::vector<std::string> get_all_file_names(std::string path);

/**
 * @brief parse all files in the file data vector and write block headers to csv
 * 
 * @param file_names 
 * @param vector_of_file_data 
 */
void parse_and_export_to_csv(std::vector<std::string> file_names, std::vector<std::string*> vector_of_file_data, std::string name = "bitcoinsv.csv", std::string tr_name = "transactions.csv");

/**
 * @brief parse all files in the file data vector and write the previous block hash to a json file
 * 
 * @param file_names 
 * @param vector_of_file_data 
 */
void parse_and_export_to_json(std::vector<std::string> file_names, std::vector<std::string*> vector_of_file_data);

/**
 * @brief parses the bsv blocks and exports to CSV
 * 
 * @param path absolute path to the directory holding the .dat bsv block files
 * @param block_numbers string vector of the .dat filenames
 */
uint16_t read_files_and_parse(std::string path, std::vector<std::string> file_names);

/**
 * @brief parses the bsv blocks and exports to CSV
 * 
 * @param path absolute path to the directory holding the .dat bsv block files
 * @param block_numbers string vector of the .dat filenames
 */
uint16_t read_file_and_parse(std::string path, std::vector<std::string> file_names, std::string name);

/**
 * @brief Runs the parser on the given path for multiple .dat files or 1 .dat file
 * 
 * @param path absolute path to the file or directory holding .dat files
 * @param name Name for block header file data to be saved in
 */
int run(std::string path, std::string name = "blockHeader.csv");
