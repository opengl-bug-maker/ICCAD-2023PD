//
// Created by 林士傑 on 2023/3/27.
//

#include "fixed_module_t.h"


fixed_module_t *fixed_module_t::fileInput(std::fstream &file) {
    fixed_module_t* fixedModule = new fixed_module_t();
    file >> fixedModule->name >> fixedModule->xCoord >> fixedModule->yCoord >> fixedModule->width >> fixedModule->height;
    return fixedModule;
}

int fixed_module_t::getHalfWidth() {
    return this->width / 2;
}

int fixed_module_t::getHalfHeight() {
    return this->height / 2;
}

int fixed_module_t::getCenterPosX() {
    return this->xCoord + this->getHalfWidth();
}

int fixed_module_t::getCenterPosY() {
    return this->yCoord + this->getHalfHeight();
}