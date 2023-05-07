//
// Created by 林士傑 on 2023/3/9.
//

#include <algorithm>
#include "polygon_t.h"

polygon_t::polygon_t(const bounding_rectangle_t& rect) :
    bounding_rect(rect.getRect()),
    polygon_modules({polygon_module_t(*this, rect)}),
    quadtree(rect.getRect()) {}

const rect_t& polygon_t::get_bounding_rect() const {
    return bounding_rect;
}

std::vector<polygon_module_t> &polygon_t::get_rects() {
    return this->polygon_modules;
}

bool polygon_t::is_bounding_collision(const rect_t &rect) const {
    return this->bounding_rect.is_collision(rect);
}

bool polygon_t::is_bounding_collision(const bounding_rectangle_t &rect) const {
    return this->is_bounding_collision(rect.getRect());
}

bool polygon_t::is_collision(const rect_t &rect) const {
    //todo quad tree
    return std::any_of(polygon_modules.begin(), polygon_modules.end(), [&rect](const polygon_module_t& r){
        return r.get_module_bounding_rectangle().getRect().is_collision(rect);
    });
}

bool polygon_t::is_collision(const bounding_rectangle_t &rect) const {
    return this->is_collision(rect.getRect());
}

bool polygon_t::merge_polygon(const polygon_t &polygon) {

    auto coli = polygon.quadtree.collision_value(this->polygon_modules.front());

    this->bounding_rect = this->bounding_rect.merge_bounding_rect(polygon.bounding_rect);

    for (const auto& i: polygon.polygon_modules) {
        this->quadtree.add_value(std::ref(i));
    }

    return false;
}
