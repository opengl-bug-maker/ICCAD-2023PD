//
// Modify by lkj on 2023/5/7.
//

#ifndef ICCAD2023PD_POLYGON_MODULE_T_H
#define ICCAD2023PD_POLYGON_MODULE_T_H

#include <functional>
#include <vector>
#include <map>
#include "polygon/polygon_overlap_area_t.h"
#include "utilities/bounding_rectangle_t.h"

class polygon_t;

class polygon_module_t : public box_t {

    polygon_t& belong_polygon;

    const bounding_rectangle_t& module_rect;

    //store overlapping areas for this polygon
    std::vector<std::reference_wrapper<polygon_overlap_area_t>> overlap_areas;

    const uint32_t demand_area;
    uint32_t free_to_share_area, shared_area;

    //return self neighbor of specific overlapping area
    std::vector<std::reference_wrapper<polygon_module_t>> get_neighbor(const polygon_overlap_area_t&) const;

    std::pair<bool, uint32_t> request_area(const uint32_t& required_area);

public:
    explicit polygon_module_t(polygon_t& belong_polygon, const bounding_rectangle_t& bounding_rect);

    polygon_module_t(const polygon_module_t& polygon_module);
    polygon_module_t operator=(const polygon_module_t& r);

    const rect_t &get_bounding_rect() const override;

    const bounding_rectangle_t& get_module_bounding_rectangle() const;

    bool connect(polygon_module_t& module);
};


#endif //ICCAD2023PD_POLYGON_MODULE_T_H
