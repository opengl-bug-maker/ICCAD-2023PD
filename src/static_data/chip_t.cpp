//
// Created by 林士傑 on 2023/3/8.
//

#include "chip_t.h"
#include "soft_module_t.h"
#include "fixed_module_t.h"

void chip_t::file_input(std::string fileName) {
    std::fstream file;
    file.open(fileName);

    int iTemp;
    std::string temp;

    while(file >> temp){
        if(temp == "CHIP"){
            file >> this->width >> this->height;
        }else if(temp == "SOFTMODULE"){
            file >> softCount;
            modules.resize(softCount);
            for (int i = 0; i < softCount; ++i) {
                modules[i] = soft_module_t::fileInput(file);
                moduleNameToIndex[modules[i]->getName()] = i;
            }
        }else if(temp == "FIXEDMODULE"){
            file >> fixedCount;
            modules.resize(softCount + fixedCount);
            for (int i = softCount; i < softCount + fixedCount; ++i) {
                modules[i] = fixed_module_t::fileInput(file);
                moduleNameToIndex[modules[i]->getName()] = i;
            }
        }else if(temp == "CONNECTION"){
            int connectionCount, index0, index1;
            file >> connectionCount;
            connectionTable = std::vector<std::vector<int>>(this->softCount + this->fixedCount, std::vector<int>(this->softCount + this->fixedCount, 0));
            for (int i = 0; i < connectionCount; ++i) {
                file >> temp;
                index0 = moduleNameToIndex[temp];
                file >> temp;
                index1 = moduleNameToIndex[temp];
                file >> iTemp;

                connectionTable[index0][index1] = iTemp;
                modules[index0]->getConnections().emplace_back(modules[index1], iTemp);
                modules[index1]->getConnections().emplace_back(modules[index0], iTemp);
            }
        }else {
            throw std::exception();
        }
    }

    int i = 0;
}
