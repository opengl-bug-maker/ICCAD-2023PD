//
// Created by 林士傑 on 2023/4/22.
//

#include "poly_union_unit_t.h"
#include "polygon/poly_unit_t.h"

#include <utility>

poly_union_unit_t::unit_connect_t::unit_connect_t(poly_unit_t &otherUnit) : other_unit(otherUnit),
                                                                            value(0.0) {}

poly_unit_t &poly_union_unit_t::unit_connect_t::getOtherUnit() const {
    return other_unit;
}

double poly_union_unit_t::unit_connect_t::getValue() const {
    return value;
}

void poly_union_unit_t::unit_connect_t::setValue(double value) {
    unit_connect_t::value = value;
}

poly_union_unit_t::poly_union_unit_t(polygon_t& belong_polygon,
                                     rect_t intersectRect,
                                     const std::vector<poly_unit_t>& connectPolyUnit) : belong_polygon(belong_polygon),
                                                                                        intersect_rect(std::move(intersectRect)),
                                                                                        connect_poly_unit({}) {

    for(auto poly : connectPolyUnit) this->connect_poly_unit.emplace_back(poly);
}

const rect_t &poly_union_unit_t::getIntersectRect() const {
    return intersect_rect;
}

bool poly_union_unit_t::is_valid() const {
    return false;
}

bool poly_union_unit_t::change_value(poly_unit_t *other_unit, double value) {
    return false;
}

std::vector<poly_unit_t> poly_union_unit_t::get_neighbor(const poly_unit_t &poly_unit) {
    return std::vector<poly_unit_t>();
}
