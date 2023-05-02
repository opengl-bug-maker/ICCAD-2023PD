//
// Created by 林士傑 on 2023/4/22.
//

#ifndef ICCAD2023PD_POLY_UNIT_T_H
#define ICCAD2023PD_POLY_UNIT_T_H

#include <vector>
#include <map>
#include "polygon/poly_union_unit_t.h"
#include "utilities/bounding_rectangle_t.h"

class polygon_t;

class poly_unit_t : public box_t {
    class unit_link_t {
        poly_union_unit_t& union_unit;
        double value;
    public:
        explicit unit_link_t(poly_union_unit_t &unionUnit);

        const poly_union_unit_t &getUnionUnit() const;

        double getValue() const;

        void setValue(double value);
    };
    polygon_t& belong_polygon;
    const bounding_rectangle_t module_rect;
    std::vector<unit_link_t> link_poly_units;

public:
    const rect_t &get_bounding_rect() const override;

    explicit poly_unit_t(polygon_t& belong_polygon, bounding_rectangle_t bd);

    const bounding_rectangle_t& get_bounding_rectangle() const;

    bool is_valid() const;

    void connect(poly_unit_t& poly_unit);

    std::vector<poly_union_unit_t> get_neighbor(const poly_union_unit_t& poly_union_unit);
};


#endif //ICCAD2023PD_POLY_UNIT_T_H
