//
// Created by 林士傑 on 2023/3/9.
//

#ifndef INC_2023PD_RECT_H
#define INC_2023PD_RECT_H

#include "vec2d_t.h"

class rect_t {
    vec2d_t LeftLower;
    vec2d_t RightUpper;
    vec2d_t Center;
    vec2d_t Size;

public:

    rect_t(const vec2d_t &left_lower, const vec2d_t &size);

    const vec2d_t &get_left_lower() const;

    const vec2d_t &get_right_upper() const;

    const vec2d_t &get_center() const;

    const vec2d_t &get_size() const;

    double get_area() const;

    bool operator==(const rect_t& rect) const;

    bool operator!=(const rect_t& rect) const;
};


#endif //INC_2023PD_RECT_H
