#ifndef MCNC_OLD_MODULE_T_H
#define MCNC_OLD_MODULE_T_H

#include <string>
#include <vector>
#include "mcnc_old_terminal_t.h"

class mcnc_old_module_t {
public:
    std::string name;
    std::vector<mcnc_old_terminal_t*> pins;
};

#endif