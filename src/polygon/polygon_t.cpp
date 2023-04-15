//
// Created by 林士傑 on 2023/3/9.
//

#include <algorithm>
#include "polygon_t.h"

polygon_t::polygon_t(bounding_rectangle_t rect) : bounding_rect(rect.getRect()) {
    this->rects.push_back(rect);
}

std::vector<bounding_rectangle_t> &polygon_t::get_rects() {
    return this->rects;
}

bool polygon_t::is_bounding_collision(const bounding_rectangle_t &rect) const {
    return !(
            rect.getRect().get_center().get_x() >= this->bounding_rect.get_center().get_x() + this->bounding_rect.get_size().get_x() ||
            rect.getRect().get_center().get_y() + rect.getRect().get_size().get_y() <= this->bounding_rect.get_center().get_y() ||
            rect.getRect().get_center().get_x() + rect.getRect().get_size().get_x() <= this->bounding_rect.get_center().get_x() ||
            rect.getRect().get_center().get_y() >= this->bounding_rect.get_center().get_y() + this->bounding_rect.get_size().get_y()
            );
}

bool polygon_t::is_collision(bounding_rectangle_t &rect) {
    return true;
}

bool polygon_t::merge_polygon(const polygon_t &polygon) const {
    return false;
}

bool polygon_t::has_bounding_rect(const bounding_rectangle_t &rect) const {
    if(!is_bounding_collision(rect)){
        return false;
    }
    return std::any_of(rects.begin(), rects.end(), [&rect](const bounding_rectangle_t& r){
        return rect.getLinkModule() == r.getLinkModule();
    });
}

std::vector<polygon_t> polygon_t::cut_polygon(const bounding_rectangle_t &rect) const {
    return std::vector<polygon_t>();
}