//
// Created by 林士傑 on 2023/11/22.
//

#ifndef ICCAD2023PD_EXTENDED_MODULE_T_H
#define ICCAD2023PD_EXTENDED_MODULE_T_H

#include <vector>
#include "utilities/vec2d_t.h"
#include "utilities/rect_t.h"

class extended_module_t {
public:
    //00 //x0, x1, x2, x3,
    //04 //xt0, xt1,
    //06 //xb0, xb1,
    //08 //y0, y1, y2, y3,
    //12 //yl0, yl1,
    //14 //yr0, yr1
    std::vector<double> lines;
    extended_module_t(std::vector<double> lines);
    extended_module_t(double* address);
    double get_left_x_extend();
    double get_right_x_extend();
    double get_top_y_extend();
    double get_bottom_y_extend();
    double get_area();
    vec2d_t get_center();
    rect_t get_inner_rect();
    rect_t get_outter_rect();
    std::vector<vec2d_t> to_vec2d();
};

#endif //ICCAD2023PD_EXTENDED_MODULE_T_H