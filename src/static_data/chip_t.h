//
// Created by 林士傑 on 2023/3/8.
//

#ifndef INC_2023PD_CHIP_H
#define INC_2023PD_CHIP_H

#include <string>
#include <vector>
#include <fstream>
#include <exception>
#include <unordered_map>
//#include "static_data/module_t.h"
//#include "static_data/soft_module_t.h"
//#include "static_data/fixed_module_t.h"

class module_t;
class soft_module_t;
class fixed_module_t;

class chip_t {
    static int width, height;
    static int softCount, fixedCount;
    static std::unordered_map<std::string, int> moduleNameToIndex;

    static std::vector<module_t*> modules;
    static std::vector<soft_module_t*> soft_modules;
    static std::vector<fixed_module_t*> fixed_modules;
    static std::vector<std::vector<int>> connectionTable;

public:
    static void file_input(std::string fileName);

    static const int getWidth();

    static const int getHeight();

    static const std::vector<module_t *> &getModules();

    static const std::vector<soft_module_t *> &getSoftModules();

    static const std::vector<fixed_module_t *> &getFixedModules();
};


#endif //INC_2023PD_CHIP_H
