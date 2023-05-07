//
// Created by 林士傑 on 2023/4/22.
//

#include "polygon_overlap_area_t.h"
#include "polygon/polygon_module_t.h"

#include <utility>

polygon_overlap_area_t::module_info::module_info(polygon_module_t &other_module) : other_module(other_module),
                                                                                   belong_area(0.0) {}

//polygon_module_t &polygon_overlap_area_t::module_info::get_other_module() const {
//    return other_module;
//}
//
//double polygon_overlap_area_t::module_info::get_belong_area() const {
//    return belong_area;
//}
//
//void polygon_overlap_area_t::module_info::set_belong_area(double value) {
//    module_info::belong_area = value;
//}

polygon_overlap_area_t::polygon_overlap_area_t(polygon_t& belong_polygon,
                                               rect_t intersectRect): //,
                                           //const std::vector<polygon_module_t>& connectPolyUnit) :
                                           belong_polygon(belong_polygon),
                                           area(std::move(intersectRect)) { //,
//                                           connect_poly_unit({}) {
//
//    for(auto poly : connectPolyUnit) this->connect_poly_unit.emplace_back(poly);
}

const rect_t &polygon_overlap_area_t::get_area() const {
    return area;
}

bool polygon_overlap_area_t::change_value(polygon_module_t *other_module, double value) {
    return false;
}

std::vector<polygon_module_t> polygon_overlap_area_t::get_neighbor(const polygon_module_t &poly_unit) {
    return std::vector<polygon_module_t>();
}
