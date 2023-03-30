//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_QUADRATIC_T_H
#define INC_2023PD_QUADRATIC_T_H

#include <vector>
#include "utilities/vec2d_t.h"
#include "utilities/bounding_rectangle_t.h"


class quadratic_t {
public:
    static vec2d_t get_coor(int index);
    static void set_floopplanning(const std::vector<bounding_rectangle_t>& bounding_rectangles, const std::vector<bool> is_placed);
};


#endif //INC_2023PD_QUADRATIC_T_H
