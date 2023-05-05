//
// Created by 林士傑 on 2023/3/9.
//

#include <algorithm>
#include "polygon_t.h"

polygon_t::polygon_t(const bounding_rectangle_t& rect) : bounding_rect(rect.getRect()), poly_units({poly_unit_t(*this, rect)}), quadtree(rect.getRect()) {
}

const rect_t& polygon_t::get_bounding_rect() const {
    return bounding_rect;
}

std::vector<poly_unit_t> &polygon_t::get_rects() {
    return this->poly_units;
}

bool polygon_t::is_bounding_collision(const rect_t &rect) const {
    return this->bounding_rect.is_collision(rect);
}

bool polygon_t::is_bounding_collision(const bounding_rectangle_t &rect) const {
    return this->is_bounding_collision(rect.getRect());
}

bool polygon_t::is_collision(const rect_t &rect) const {
    //todo quad tree
    return std::any_of(poly_units.begin(), poly_units.end(), [&rect](const poly_unit_t& r){
        return r.get_bounding_rectangle().getRect().is_collision(rect);
    });
}

bool polygon_t::is_collision(const bounding_rectangle_t &rect) const {
    return this->is_collision(rect.getRect());
}

bool polygon_t::merge_polygon(const polygon_t &polygon) {
    auto coli = polygon.quadtree.collision_value(this->poly_units.front());
    if(/* valid*/false){
        return false;
    }

    this->bounding_rect = this->bounding_rect.merge_bounding_rect(polygon.bounding_rect);
    return false;
}
