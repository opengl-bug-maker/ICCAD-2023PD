//
// Created by 林士傑 on 2023/3/27.
//

#include "soft_module_t.h"

soft_module_t *soft_module_t::fileInput(std::fstream &file) {
    soft_module_t* soft_module = new soft_module_t();
    file >> soft_module->name >> soft_module->minArea;
    return soft_module;
}

int soft_module_t::getHalfWidth() {
    return std::sqrt(this->minArea) / 2;
}

int soft_module_t::getHalfHeight() {
    return std::sqrt(this->minArea) / 2;
}

int soft_module_t::getCenterPosX() {
    return 0;
}

int soft_module_t::getCenterPosY() {
    return 0;
}
