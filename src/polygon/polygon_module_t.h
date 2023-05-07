//
// Created by 林士傑 on 2023/4/22.
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

    const bounding_rectangle_t module_rect;

    //store overlapping areas for this polygon
    std::vector<std::reference_wrapper<polygon_overlap_area_t>> overlap_areas;

    const uint32_t demand_area;
    uint32_t free_to_share_area, shared_area;

public:
    explicit polygon_module_t(polygon_t& belong_polygon, bounding_rectangle_t bounding_rect);

    polygon_module_t(const polygon_module_t& polygon_module);
    polygon_module_t operator=(const polygon_module_t& r);

    const rect_t &get_bounding_rect() const override;

    const bounding_rectangle_t& get_module_bounding_rectangle() const;

    std::pair<bool, uint32_t> request_area(const uint32_t& required_area);
    std::pair<bool, uint32_t> request_area(const uint32_t&& required_area);

    void connect(polygon_module_t& module);

    //return self neighbor of specific overlapping area
    std::vector<std::reference_wrapper<polygon_module_t>> get_neighbor(const polygon_overlap_area_t&) const;
};


#endif //ICCAD2023PD_POLYGON_MODULE_T_H
