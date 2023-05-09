//
// Created by lkj on 2023/3/27.
//

#ifndef INC_2023PD_VISUALIZER_T_H
#define INC_2023PD_VISUALIZER_T_H

#include <vector>
#include "utilities/bounding_rectangle_t.h"

class visualizer_t {
public:
    static void show_fp(const std::vector<bounding_rectangle_t>&);
    static void show_fp_no_border(const std::vector<bounding_rectangle_t>&);
    static void show_fp_rect_no_border(const std::vector<std::pair<rect_t, std::string>> &);
private:
    static int max_x;
    static int max_y;
    static void join(const std::vector<std::pair<std::vector<vec2d_t>,std::string>>& polys_pts);
    static void gnup_th(const std::vector<std::pair<std::vector<vec2d_t>,std::string>> polys_pts);
};


#endif //INC_2023PD_VISUALIZER_T_H
