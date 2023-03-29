//
// Created by 林士傑 on 2023/3/8.
//

#include "chip_t.h"
#include "soft_module_t.h"
#include "fixed_module_t.h"

int chip_t::width, chip_t::height;
int chip_t::softCount, chip_t::fixedCount;
std::unordered_map<std::string, int> chip_t::moduleNameToIndex;

std::vector<module_t*> chip_t::modules;
std::vector<soft_module_t*> chip_t::soft_modules;
std::vector<fixed_module_t*> chip_t::fixed_modules;
std::vector<std::vector<int>> chip_t::connectionTable;

void chip_t::file_input(std::string fileName) {
    std::fstream file;
    file.open(fileName);

    int iTemp;
    std::string temp;

    while(file >> temp){
        if(temp == "CHIP"){
            file >> chip_t::width >> chip_t::height;
        }else if(temp == "SOFTMODULE"){
            file >> chip_t::softCount;
            chip_t::modules.resize(chip_t::softCount);
            chip_t::soft_modules.resize(chip_t::softCount);
            for (int i = 0; i < chip_t::softCount; ++i) {
                chip_t::soft_modules[i] = soft_module_t::fileInput(file);
                chip_t::modules[i] = chip_t::soft_modules[i];
                chip_t::moduleNameToIndex[chip_t::modules[i]->getName()] = i;
            }
        }else if(temp == "FIXEDMODULE"){
            file >> chip_t::fixedCount;
            chip_t::modules.resize(chip_t::softCount + chip_t::fixedCount);
            chip_t::fixed_modules.resize(chip_t::fixedCount);
            for (int i = chip_t::softCount; i < chip_t::softCount + chip_t::fixedCount; ++i) {
                chip_t::fixed_modules[i - chip_t::softCount] = fixed_module_t::fileInput(file);
                chip_t::modules[i] = chip_t::fixed_modules[i - chip_t::softCount];
                chip_t::moduleNameToIndex[chip_t::modules[i]->getName()] = i;
            }
        }else if(temp == "CONNECTION"){
            int connectionCount, index0, index1;
            file >> connectionCount;
            chip_t::connectionTable =
                    std::vector<std::vector<int>>(chip_t::softCount + chip_t::fixedCount,
                    std::vector<int>(chip_t::softCount + chip_t::fixedCount, 0));
            for (int i = 0; i < connectionCount; ++i) {
                file >> temp;
                index0 = chip_t::moduleNameToIndex[temp];
                file >> temp;
                index1 = chip_t::moduleNameToIndex[temp];
                file >> iTemp;

                chip_t::connectionTable[index0][index1] = iTemp;
                chip_t::modules[index0]->connections.emplace_back(chip_t::modules[index1], iTemp);
                chip_t::modules[index1]->connections.emplace_back(chip_t::modules[index0], iTemp);
            }
        }else {
            throw std::exception();
        }
    }
}

const int chip_t::getWidth() {
    return chip_t::width;
}

const int chip_t::getHeight() {
    return chip_t::height;
}

const std::vector<module_t *> &chip_t::getModules() {
    return chip_t::modules;
}

const std::vector<soft_module_t *> &chip_t::getSoftModules() {
    return chip_t::soft_modules;
}

const std::vector<fixed_module_t *> &chip_t::getFixedModules() {
    return chip_t::fixed_modules;
}
