//
// Created by 林士傑 on 2023/6/9.
//

#include <algorithm>
#include "lenf_pf_t.h"
#include "static_data/chip_t.h"
#include "static_data/module_t.h"
#include "static_data/fixed_module_t.h"
#include "static_data/soft_module_t.h"
#include "utilities/rect_t.h"
#include "utilities/vec2d_t.h"

lenf_pf_t::lenf_pf_t() : quadtree(quadtree_t<polygon_t>(rect_t(vec2d_t(0, 0), vec2d_t(chip_t::get_width(), chip_t::get_height())))){
    this->polygons.reserve(200);
    this->polygons.push_back(std::make_shared<polygon_t>(polygon_t(bounding_rectangle_t(&soft_module_t::void_module,rect_t(vec2d_t(0, 0), vec2d_t(0, chip_t::get_height()))))));
    this->polygons.push_back(std::make_shared<polygon_t>(polygon_t(bounding_rectangle_t(&soft_module_t::void_module,rect_t(vec2d_t(0, 0), vec2d_t(chip_t::get_width(), 0))))));
    this->polygons.push_back(std::make_shared<polygon_t>(polygon_t(bounding_rectangle_t(&soft_module_t::void_module,rect_t(vec2d_t(0, chip_t::get_height()), vec2d_t(chip_t::get_width(), 0))))));
    this->polygons.push_back(std::make_shared<polygon_t>(polygon_t(bounding_rectangle_t(&soft_module_t::void_module,rect_t(vec2d_t(chip_t::get_width(), 0), vec2d_t(0, chip_t::get_height()))))));
}

std::vector<polygon_t> lenf_pf_t::get_polygons() {
    std::vector<polygon_t> vecs;
    for(int i = 4; i < this->polygons.size(); i++){
        vecs.push_back(*polygons[i].get());
    }
    return vecs;
}

bool lenf_pf_t::test_collision(const rect_t &rect) {
    // check any polygon collision new_rect
    for (int i = 0; i < polygons.size(); ++i) {
        if(polygons[i]->is_bounding_collision(bounding_rectangle_t(rect))){
            if(polygons[i]->is_collision(bounding_rectangle_t(rect))){
                return true;
            }
        }
    }
    return false;
}

bool lenf_pf_t::add_rect(const bounding_rectangle_t& boundingRectangle) {
    if( boundingRectangle.getRect().get_left_lower().get_x() < 0 ||
        boundingRectangle.getRect().get_left_lower().get_y() < 0 ||
        boundingRectangle.getRect().get_right_upper().get_x() > chip_t::get_width() ||
        boundingRectangle.getRect().get_right_upper().get_y() > chip_t::get_height()){
        //haha got you!
        throw std::exception();
    }
    std::shared_ptr<polygon_t> new_poly = std::make_shared<polygon_t>(boundingRectangle);
//    polygon_t new_poly(boundingRectangle);
    std::vector<int> merging_poly;
    // check any polygon collision new_rect
    auto collision_polygons = quadtree.collision_value(*new_poly.get());
    for (int i = 0; i < polygons.size(); ++i) {
        if(polygons[i]->is_bounding_collision(boundingRectangle)){
            if(polygons[i]->is_collision(boundingRectangle)){
                merging_poly.push_back(i);
            }
        }
    }
    // add new polygon
    if(collision_polygons.empty()){
        //no one touch
        quadtree.add_value(*new_poly.get());
        polygons.push_back(std::move(new_poly));
        return true;
    }
    // merge all collision polygon
    for (auto poly : merging_poly) {
        if(!new_poly->merge_polygon(*polygons[poly].get())){
            //merge fail
            return false;
        }
    }
    for (int i = (int)merging_poly.size() - 1; i >= 0; --i) {
        polygons.erase(polygons.begin() + merging_poly[i]);
    }
    polygons.push_back(std::move(new_poly));
    return true;
}

std::vector<rect_t> lenf_pf_t::get_empty_spaces() {
    std::vector<rect_t> spaces;
    std::vector<bounding_rectangle_t> bounding;
    for (auto poly : polygons) {
        for(auto bd : poly->get_rects()){
            bounding.push_back(bd.get_module_bounding_rectangle());
        }
    }

    for (int i = 0; i < bounding.size(); ++i) {
        for (int j = i + 1; j < bounding.size(); ++j) {
            auto inter = bounding[i].getRect().intersect(bounding[j].getRect());
            if(inter.first) continue;

            if(std::any_of(polygons.begin(), polygons.end(), [&inter](std::shared_ptr<polygon_t> poly){
                return poly->is_bounding_collision(inter.second) && poly->is_collision(inter.second);
            })){
                continue;
            }

            spaces.push_back(inter.second);
        }
    }
    return spaces;
}