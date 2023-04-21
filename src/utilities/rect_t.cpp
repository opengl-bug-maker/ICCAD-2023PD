//
// Created by 林士傑 on 2023/3/9.
//

#include "rect_t.h"

rect_t::rect_t(const vec2d_t &left_lower, const vec2d_t &size) : Center(left_lower + size / 2), Size(size), LeftLower(left_lower),
                                                                 RightUpper(left_lower + size) {
}

const vec2d_t &rect_t::get_left_lower() const {
    return LeftLower;
}

const vec2d_t &rect_t::get_right_upper() const {
    return RightUpper;
}

const vec2d_t &rect_t::get_center() const {
    return Center;
}

const vec2d_t &rect_t::get_size() const {
    return Size;
}

double rect_t::get_area() const {
    return get_size().get_area();
}

bool rect_t::operator==(const rect_t &rect) const {
    return
            this->get_center() == rect.get_center() &&
            this->get_size() == rect.get_size() &&
            this->get_left_lower() == rect.get_left_lower() &&
            this->get_right_upper() == rect.get_right_upper();
}

bool rect_t::operator!=(const rect_t &rect) const {
    return
            this->get_center() != rect.get_center() ||
            this->get_size() != rect.get_size() ||
            this->get_left_lower() != rect.get_left_lower() ||
            this->get_right_upper() != rect.get_right_upper();
}