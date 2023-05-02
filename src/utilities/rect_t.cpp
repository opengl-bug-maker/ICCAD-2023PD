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

bool rect_t::is_collision(const rect_t &rect) const {
    return (
            rect.get_left_lower().get_x() < this->get_right_upper().get_x() &&
            rect.get_right_upper().get_y() > this->get_left_lower().get_y() &&
            rect.get_right_upper().get_x() > this->get_left_lower().get_x() &&
            rect.get_left_lower().get_y() < this->get_right_upper().get_y()
    );
}

bool rect_t::is_contain(const rect_t &rect) const {
    return (
            this->get_right_upper().get_x() >= rect.get_right_upper().get_x() &&
            this->get_right_upper().get_y() >= rect.get_right_upper().get_y() &&
            this->get_left_lower().get_x() >= rect.get_left_lower().get_x() &&
            this->get_left_lower().get_y() >= rect.get_left_lower().get_y()
            );
}

std::pair<bool, rect_t> rect_t::intersect(const rect_t &rect) const {
    double r2l1x = rect.get_right_upper().get_x() - this->get_left_lower().get_x();
    double r1l2x = this->get_right_upper().get_x() - rect.get_left_lower().get_x();
    double u2b1y = rect.get_right_upper().get_y() - this->get_left_lower().get_y();
    double u1b2y = this->get_right_upper().get_y() - rect.get_left_lower().get_y();

    double wid;
    double hei;
    double le;
    double bo;
    if(r2l1x < r1l2x){
        wid = r2l1x;
        le = this->get_left_lower().get_x();
    }else{
        wid = r1l2x;
        le = rect.get_left_lower().get_x();
    }

    if(u2b1y < u1b2y){
        hei = u2b1y;
        bo = this->get_left_lower().get_y();
    }else{
        hei = u1b2y;
        bo = rect.get_left_lower().get_y();
    }
    if(wid >= 0 && hei >= 0){
        return {true, {{le, bo}, {wid, hei}}};
    }
    if(wid < 0){
        le += wid;
        wid -= wid + wid;
    }
    if(hei < 0){
        bo += hei;
        hei -= hei + hei;
    }
    return {false, {{le, bo}, {wid, hei}}};
}

const rect_t &rect_t::get_bounding_rect() const {
    return *this;
}

const rect_t rect_t::merge_bounding_rect(const rect_t &rect) const {
    double l = std::min(this->get_left_lower().get_x(), rect.get_left_lower().get_x());
    double r = std::max(this->get_right_upper().get_x(), rect.get_right_upper().get_x());
    double u = std::max(this->get_right_upper().get_y(), rect.get_right_upper().get_y());
    double b = std::min(this->get_left_lower().get_y(), rect.get_left_lower().get_y());
    return rect_t(vec2d_t(l, b), vec2d_t(r - l, u - b));
}

std::ostream &operator<<(std::ostream &os, const rect_t &vec) {
    os << "rect_t: {" << vec.get_left_lower().get_x() << ", " << vec.get_left_lower().get_y() << " | " << vec.get_size().get_x() << ", " << vec.get_size().get_y() << "}";
    return os;
}