#include <string>
#include <vector>
#include "bsv_input.h"
#include "bsv_output.h"

#ifndef BSV_TRANSACTION_H
#define BSV_TRANSACTION_H

class BSV_TRANSACTION {
    private:
    public:
        BSV_TRANSACTION();
        ~BSV_TRANSACTION();

        std::string version;
        unsigned long number_of_inputs;
        std::vector<BSV_INPUT> inputs;
        unsigned long number_of_outputs;
        std::vector<BSV_OUTPUT> outputs;
        std::string lock_time;
};

#endif