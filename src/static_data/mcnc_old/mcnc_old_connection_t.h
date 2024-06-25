#ifndef MCNC_OLD_CONNECTION_T_H
#define MCNC_OLD_CONNECTION_T_H

#include <vector>
#include <string>

class mcnc_old_soft_moudle_t;

class mcnc_old_connection_pin_t {
public:
    std::string name;
    mcnc_old_module_t* belong_module;
    bool is_terminal;
    double x, y;
};

class mcnc_old_connection_t {
public:
    std::vector<mcnc_old_connection_pin_t*> modules;
};

#endif