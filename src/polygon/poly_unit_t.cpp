//
// Created by 林士傑 on 2023/4/22.
//

#include "poly_unit_t.h"
#include "polygon/poly_union_unit_t.h"
#include "static_data/module_t.h"

#include <utility>

poly_unit_t::unit_link_t::unit_link_t(poly_union_unit_t &unionUnit) : union_unit(unionUnit), value(0.0) {}

const poly_union_unit_t &poly_unit_t::unit_link_t::getUnionUnit() const {
    return union_unit;
}

double poly_unit_t::unit_link_t::getValue() const {
    return value;
}

void poly_unit_t::unit_link_t::setValue(double value) {
    unit_link_t::value = value;
}

poly_unit_t::poly_unit_t(polygon_t& belong_polygon,
                         bounding_rectangle_t bd) : belong_polygon(belong_polygon),
                                                    module_rect(std::move(bd)) {}

const rect_t &poly_unit_t::get_bounding_rect() const {
    return module_rect.getRect();
}

const bounding_rectangle_t &poly_unit_t::get_bounding_rectangle() const {
    return module_rect;
}

void poly_unit_t::connect(poly_unit_t &poly_unit) {
}

bool poly_unit_t::is_valid() const {
    return false;
}

std::vector<poly_union_unit_t> poly_unit_t::get_neighbor(const poly_union_unit_t& poly_union_unit) {
    return std::vector<poly_union_unit_t>();
}
