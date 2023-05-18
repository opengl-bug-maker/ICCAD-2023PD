//
// Created by 林士傑 on 2023/3/9.
//

#include <algorithm>
#include "polygon_t.h"

polygon_t::polygon_t(const bounding_rectangle_t& rect) :
                     bounding_rect(rect.getRect()),
                     quadtree(rect.getRect()) {
    quadtree.add_value(polygon_module_t(rect));
}

const rect_t& polygon_t::get_bounding_rect() const {
    return bounding_rect;
}

const std::vector<polygon_module_t>& polygon_t::get_rects() const{
    return this->quadtree.get_values();
}

bool polygon_t::is_bounding_collision(const rect_t &rect) const {
    return this->bounding_rect.is_collision(rect);
}

bool polygon_t::is_bounding_collision(const bounding_rectangle_t &rect) const {
    return this->is_bounding_collision(rect.getRect());
}

bool polygon_t::is_collision(const rect_t &rect) const {
    //todo quad tree

    return std::any_of(this->quadtree.get_values().begin(), this->quadtree.get_values().end(), [&rect](const polygon_module_t& r){
        return r.get_module_bounding_rectangle().getRect().is_collision(rect);
    });
}

bool polygon_t::is_collision(const bounding_rectangle_t &rect) const {
    return this->is_collision(rect.getRect());
}

bool polygon_t::merge_polygon(polygon_t& polygon) {

    auto coli = polygon.quadtree.collision_value(this->quadtree.get_values().front());

    for(auto& module : coli){
        if(this->quadtree.get_values_ref().front().connect(module) == false) {
            return false;
        }
    }

    return false;

    this->bounding_rect = this->bounding_rect.merge_bounding_rect(polygon.bounding_rect);

    for (auto& i: polygon.quadtree.get_values_ref()) {
        this->quadtree.add_value(i);
    }

    return true;
}
