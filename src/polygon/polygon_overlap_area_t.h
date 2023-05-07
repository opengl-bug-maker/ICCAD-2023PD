//
// Modify by lkj on 2023/5/7.
//

#ifndef ICCAD2023PD_POLYGON_OVERLAP_AREA_T_H
#define ICCAD2023PD_POLYGON_OVERLAP_AREA_T_H

#include <map>
#include <vector>
#include "utilities/rect_t.h"

class polygon_t;
class polygon_module_t;

class polygon_overlap_area_t {
    class module_info {
    public:
        polygon_module_t& other_module;
        uint32_t belong_area;
        explicit module_info(polygon_module_t &other_module);
    };
    polygon_t& belong_polygon;
    const rect_t area;
    std::vector<module_info> connect_poly_unit;

public:
    explicit polygon_overlap_area_t(polygon_t& belong_polygon, rect_t intersectRect);//, const polygon_module_t& connectPolyUnit);

    const rect_t &get_area() const;

    bool change_value(polygon_module_t *other_module, double value);

    std::vector<polygon_module_t> get_neighbor(const polygon_module_t& poly_unit);
};


#endif //ICCAD2023PD_POLYGON_OVERLAP_AREA_T_H
