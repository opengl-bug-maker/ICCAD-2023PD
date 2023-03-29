//
// Created by 林士傑 on 2023/3/27.
//

#include "fixed_module_t.h"
#include "utilities/rect_t.h"
#include "utilities/bounding_rectangle_t.h"

fixed_module_t::fixed_module_t() {

}

fixed_module_t *fixed_module_t::fileInput(std::fstream &file) {
    fixed_module_t* fixedModule = new fixed_module_t();
    file >> fixedModule->name >> fixedModule->xCoord >> fixedModule->yCoord >> fixedModule->width >> fixedModule->height;
    fixedModule->rect = new rect_t(vec2d_t(fixedModule->xCoord + fixedModule->width / 2.0, fixedModule->yCoord + fixedModule->height / 2.0),
                                   vec2d_t(fixedModule->width, fixedModule->height));
    return fixedModule;
}

const int fixed_module_t::getHalfWidth() const {
    return this->width / 2;
}

const int fixed_module_t::getHalfHeight() const {
    return this->height / 2;
}

const int fixed_module_t::getCenterPosX() const {
    return this->xCoord + this->getHalfWidth();
}

const int fixed_module_t::getCenterPosY() const {
    return this->yCoord + this->getHalfHeight();
}

bounding_rectangle_t fixed_module_t::make_bd() {
    return {this, *this->rect};
}

std::pair<bounding_rectangle_t, bool> fixed_module_t::make_bd(rect_t& rect) {
    if(*this->rect == rect)
        return {{this, *this->rect}, true};
    return {{this, *this->rect}, false};
}