#include <string>
#include <vector>

#ifndef BSV_INPUT_H
#define BSV_INPUT_H

class BSV_INPUT {
    private:
    public:
        BSV_INPUT();
        ~BSV_INPUT();

        std::string pre_transaction_hash;
        std::string pre_transaction_out_index;
        unsigned long long script_length;
        std::string script;
        std::string sequence;
};

#endif