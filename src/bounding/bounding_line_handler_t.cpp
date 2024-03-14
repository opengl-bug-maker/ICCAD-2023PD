//
// Created by 林士傑 on 2023/6/20.
//

#include "bounding_line_handler_t.h"

bounding_line_handler_t::bounding_line_handler_t(const vec2d_t &size) {
    this->bounding_lines.push_back(bounding_line_t({
        vec2d_t(0, 0),
        vec2d_t(0, size.get_y()),
        size,
        vec2d_t(size.get_x(), 0)
    },true));
}

const std::vector<bounding_line_t>& bounding_line_handler_t::get_bounding_lines() const {
    return this->bounding_lines;
}

void bounding_line_handler_t::add_bounding_line(const bounding_line_t &bounding_line) {
    for(auto bl = this->bounding_lines.end(); bl != this->bounding_lines.begin(); bl--){
        if(bl->touch(bounding_line)){
//            bounding_lin
        }
    }
}
