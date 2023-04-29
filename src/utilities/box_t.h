//
// Created by 林士傑 on 2023/4/28.
//

#ifndef ICCAD2023PD_BOX_T_H
#define ICCAD2023PD_BOX_T_H

#include "utilities/rect_t.h"

class box_t {
public:
    virtual rect_t get_bounding_rect() const = 0;
};

#endif //ICCAD2023PD_BOX_T_H
