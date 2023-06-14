//
// Modify by lkj on 2023/5/7.
//

#ifndef ICCAD2023PD_POLYGON_OVERLAP_AREA_T_H
#define ICCAD2023PD_POLYGON_OVERLAP_AREA_T_H

#include <map>
#include <vector>
#include <functional>
#include "utilities/rect_t.h"

class polygon_t;
class polygon_module_t;

class polygon_overlap_area_t {
    class module_info {
    public:
        polygon_module_t& polygon_module;
        uint32_t belong_area;
        explicit module_info(polygon_module_t &polygon_module);
    };
    polygon_t& belong_polygon;
    const rect_t area;
    std::vector<module_info> connect_module;

public:
    explicit polygon_overlap_area_t(polygon_t& belong_polygon, rect_t area,
                                    const std::vector<std::reference_wrapper<polygon_module_t>>& connect_module);

    const rect_t &get_area() const;

    std::pair<bool, uint32_t> request_area(const uint32_t& value);

    std::vector<polygon_module_t> get_neighbor(const polygon_module_t& polygon_module);
};


#endif //ICCAD2023PD_POLYGON_OVERLAP_AREA_T_H
