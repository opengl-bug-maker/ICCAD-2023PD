//
// Modify by lkj on 2023/5/7.
//

#include "polygon_module_t.h"
#include "polygon/polygon_overlap_area_t.h"
#include "static_data/module_t.h"

#include <utility>

polygon_module_t::polygon_module_t(polygon_t& belong_polygon, const bounding_rectangle_t& bounding_rect) :
    belong_polygon(belong_polygon),
    module_rect(std::move(bounding_rect)),
    demand_area(bounding_rect.getLinkModule()->get_area()),
    free_to_share_area(bounding_rect.getRect().get_area() - demand_area),
    shared_area(0),
    overlap_areas(std::vector<std::reference_wrapper<polygon_overlap_area_t>>({})) {}

polygon_module_t::polygon_module_t(const polygon_module_t &r):
    belong_polygon(r.belong_polygon),
    module_rect(std::move(r.module_rect)),
    demand_area(r.demand_area),
    free_to_share_area(r.free_to_share_area),
    shared_area(r.shared_area),
    overlap_areas(r.overlap_areas) {}

polygon_module_t polygon_module_t::operator=(const polygon_module_t &r) {
    return polygon_module_t(r);
}

std::pair<bool, uint32_t> polygon_module_t::request_area(const uint32_t& required_area) {
    if(required_area <= this->free_to_share_area){
        this->shared_area += required_area;
        this->free_to_share_area -= required_area;
        return {true, required_area};
    } else{
        this->shared_area += free_to_share_area;
        this->free_to_share_area = 0;
        return {false, this->free_to_share_area};
    }
}

std::vector<std::reference_wrapper<polygon_module_t>> polygon_module_t::get_neighbor(const polygon_overlap_area_t&) const {
    return std::vector<std::reference_wrapper<polygon_module_t>>({});
}

const rect_t &polygon_module_t::get_bounding_rect() const {
    return module_rect.getRect();
}

const bounding_rectangle_t &polygon_module_t::get_module_bounding_rectangle() const {
    return module_rect;
}

bool polygon_module_t::connect(polygon_module_t &module) {
}
