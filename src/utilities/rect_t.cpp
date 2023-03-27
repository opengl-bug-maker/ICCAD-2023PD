//
// Created by 林士傑 on 2023/3/9.
//

#include "rect_t.h"

const position_t &rect_t::getLeftLower() const {
    return LeftLower;
}

const position_t &rect_t::getRightUpper() const {
    return RightUpper;
}

const position_t &rect_t::getCenter() const {
    return Center;
}

const position_t &rect_t::getSize() const {
    return Size;
}
