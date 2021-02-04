#include <string>
#include <vector>

#ifndef BSV_OUTPUT_H
#define BSV_OUTPUT_H

class BSV_OUTPUT {
    private:
    public:
        BSV_OUTPUT();
        ~BSV_OUTPUT();

        std::string value;
        unsigned long script_length;
        std::string script;
};

#endif