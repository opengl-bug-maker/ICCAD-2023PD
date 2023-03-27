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
#include "module_t.h"

class chip_t {
public:
    int width, height;
    int softCount, fixedCount;
    std::unordered_map<std::string, int> moduleNameToIndex;

    std::vector<module_t*> modules;
    std::vector<std::vector<int>> connectionTable;

public:
    void file_input(std::string fileName);
};


#endif //INC_2023PD_CHIP_H
