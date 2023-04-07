//
// Created by 林士傑 on 2023/3/27.
//

#include "soft_module_t.h"

soft_module_t::soft_module_t() {

}

soft_module_t *soft_module_t::fileInput(std::fstream &file) {
    soft_module_t* soft_module = new soft_module_t();
    file >> soft_module->name >> soft_module->minArea;
    return soft_module;
}

const uint32_t soft_module_t::get_area() const {
    return static_cast<const uint32_t>(this->minArea);
}

const int soft_module_t::getHalfWidth() const {
    return std::sqrt(this->minArea) / 2;
}

const int soft_module_t::getHalfHeight() const {
    return std::sqrt(this->minArea) / 2;
}

const int soft_module_t::getCenterPosX() const {
    return 0;
}

const int soft_module_t::getCenterPosY() const {
    return 0;
}

bounding_rectangle_t soft_module_t::make_bd() {
    return {this};
}

std::pair<bounding_rectangle_t, bool> soft_module_t::make_bd(rect_t &rect) {
    if(rect.get_size().get_area() >= this->minArea)
        return {{this, rect}, true};
    return {{this, rect}, false};
}
