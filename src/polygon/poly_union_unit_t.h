//
// Created by 林士傑 on 2023/4/22.
//

#ifndef ICCAD2023PD_POLY_UNION_UNIT_T_H
#define ICCAD2023PD_POLY_UNION_UNIT_T_H

#include <map>
#include "utilities/rect_t.h"

class polygon_t;
class poly_unit_t;

class poly_union_unit_t {
    class unit_connect_t {
        poly_unit_t& other_unit;
        double value;
    public:
        explicit unit_connect_t(poly_unit_t &otherUnit);

        poly_unit_t &getOtherUnit() const;

        double getValue() const;

        void setValue(double value);
    };
    polygon_t& belong_polygon;
    const rect_t intersect_rect;
    std::vector<unit_connect_t> connect_poly_unit;

public:
    explicit poly_union_unit_t(polygon_t& belong_polygon, rect_t intersectRect, const std::vector<poly_unit_t>& connectPolyUnit);

    const rect_t &getIntersectRect() const;

    bool is_valid() const;

    bool change_value(poly_unit_t *other_unit, double value);

    std::vector<poly_unit_t> get_neighbor(const poly_unit_t& poly_unit);
};


#endif //ICCAD2023PD_POLY_UNION_UNIT_T_H
