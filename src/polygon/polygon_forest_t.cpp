//
// Created by 林士傑 on 2023/3/27.
//

#include "polygon_forest_t.h"
#include "static_data/chip_t.h"
#include "static_data/module_t.h"
#include "static_data/fixed_module_t.h"
#include "static_data/soft_module_t.h"
#include "utilities/rect_t.h"
#include "utilities/vec2d_t.h"

bool polygon_forest_t::add_rect(bounding_rectangle_t &boundingRectangle) {
    if( boundingRectangle.getRect().get_left_lower().get_x() < 0 ||
        boundingRectangle.getRect().get_left_lower().get_y() < 0 ||
        boundingRectangle.getRect().get_right_upper().get_x() > chip_t::get_width() ||
        boundingRectangle.getRect().get_right_upper().get_y() > chip_t::get_height()){
        //haha got you!
        throw std::exception();
    }
    polygon_t new_poly(boundingRectangle);
    std::vector<int> merging_poly;
    // check any polygon collision new_rect
    for (int i = 0; i < polygons.size(); ++i) {
        if(polygons[i].is_bounding_collision(boundingRectangle)){
            if(polygons[i].is_collision(boundingRectangle)){
                merging_poly.push_back(i);
            }
        }
    }
    // add new polygon
    if(merging_poly.empty()){
        //no one touch
        polygons.push_back(new_poly);
        return true;
    }
    // merge all collision polygon
    for (auto poly : merging_poly) {
        if(!new_poly.merge_polygon(polygons[poly])){
            //merge fail
            return false;
        }
    }
    for (int i = (int)merging_poly.size() - 1; i >= 0; --i) {
        polygons.erase(polygons.begin() + i);
    }
    return true;
}

void polygon_forest_t::test_without_collision() {
    auto fix = chip_t::get_fixed_modules();
    auto bd = fix[0]->make_bd();
    rect_t r = rect_t(vec2d_t(4, 9), vec2d_t(6, 6));
    auto bd1 = chip_t::get_soft_modules()[0]->make_bd(r);
    auto tt = &r;
    auto ttt = &bd1.first.getRect();
    auto re = this->add_rect(bd);
    auto ree =this->add_rect(bd1.first);
    int a = 0;
}
