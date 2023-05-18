//
// Modify by lkj on 2023/5/7.
//

#include "polygon_overlap_area_t.h"
#include "polygon/polygon_module_t.h"

#include <utility>

polygon_overlap_area_t::module_info::module_info(polygon_module_t &polygon_module) :
                                                 polygon_module(polygon_module),
                                                 belong_area(0.0) {}

polygon_overlap_area_t::polygon_overlap_area_t(polygon_t& belong_polygon, rect_t area,
                                               const std::vector<std::reference_wrapper<polygon_module_t>>& connect_module) :
                                               belong_polygon(belong_polygon),
                                               area(std::move(area)) {
    for(auto module : connect_module){
        this->connect_module.emplace_back(module);
    }
}

const rect_t &polygon_overlap_area_t::get_area() const {
    return area;
}

std::pair<bool, uint32_t> request_area(const uint32_t& value) {
    return {false,0};
}

std::vector<polygon_module_t> polygon_overlap_area_t::get_neighbor(const polygon_module_t &poly_unit) {
    return std::vector<polygon_module_t>();
}
