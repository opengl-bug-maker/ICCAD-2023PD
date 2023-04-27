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

polygon_forest_t::polygon_forest_t() {
    this->polygons = {
        polygon_t(bounding_rectangle_t(rect_t(vec2d_t(0, 0), vec2d_t(0, chip_t::get_height())))),
        polygon_t(bounding_rectangle_t(rect_t(vec2d_t(0, 0), vec2d_t(chip_t::get_width(), 0)))),
        polygon_t(bounding_rectangle_t(rect_t(vec2d_t(0, chip_t::get_height()), vec2d_t(chip_t::get_width(), chip_t::get_height())))),
        polygon_t(bounding_rectangle_t(rect_t(vec2d_t(chip_t::get_width(), 0), vec2d_t(chip_t::get_width(), chip_t::get_height()))))
    };
}

std::vector<polygon_t> polygon_forest_t::get_polygons() {
    return {polygons.begin() + 4, polygons.end()};
}

bool polygon_forest_t::add_rect(const bounding_rectangle_t &boundingRectangle) {
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

void polygon_forest_t::delete_rect(const bounding_rectangle_t& boundingRectangle) {
    int index = 0;
    polygon_t* poly = nullptr;
    for (int i = 0; i < polygons.size(); ++i) {
        if(polygons[i].has_bounding_rect(boundingRectangle)){
            index = i;
            poly = &polygons[i];
            break;
        }
    }
    if(poly == nullptr){
        //wtf want deleted not found.
        throw std::exception();
    }
    polygons.erase(polygons.begin() + index);
    for(auto po : poly->cut_polygon(boundingRectangle)){
        polygons.push_back(po);
    }
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
    auto bdd = fix[1]->make_bd();
    ree = this->add_rect(bdd);
//    this->delete_rect(bd);
//    this->delete_rect(bd);
    int a = 0;
}

bool polygon_forest_t::test_collision(const rect_t &rect) {
    // check any polygon collision new_rect
    for (int i = 0; i < polygons.size(); ++i) {
        if(polygons[i].is_bounding_collision(rect)){
            if(polygons[i].is_collision(rect)){
                return true;
            }
        }
    }
    return false;
}

std::vector<rect_t> polygon_forest_t::get_empty_spaces() {
    std::vector<rect_t> spaces;
    std::vector<bounding_rectangle_t> bounding;
    for (auto poly : polygons) {
        for(auto bd : poly.get_rects()){
            bounding.push_back(bd);
        }
    }

    for (int i = 0; i < bounding.size(); ++i) {
        for (int j = i + 1; j < bounding.size(); ++j) {
            rect_t r = bounding[i].getRect().intersect(bounding[j].getRect());
            if(r.get_size().get_x() >= 0 && r.get_size().get_y() >= 0) continue;

            if(r.get_size().get_x() < 0)
                r = rect_t(vec2d_t(r.get_left_lower().get_x() + r.get_size().get_x(), r.get_left_lower().get_y()), vec2d_t(abs(r.get_size().get_x()), r.get_size().get_y()));
            if(r.get_size().get_y() < 0)
                r = rect_t(vec2d_t(r.get_left_lower().get_x(), r.get_left_lower().get_y() + r.get_size().get_y()), vec2d_t(r.get_size().get_x(), abs(r.get_size().get_y())));


            for (auto & polygon : polygons) {
                if(polygon.is_bounding_collision(r)){
                    if(polygon.is_collision(r)){
                        continue;
                    }
                }
            }

            spaces.push_back(r);
        }
    }
    return spaces;
}
