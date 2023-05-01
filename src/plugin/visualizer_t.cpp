//
// Created by lkj on 2023/4/7.
//

#include "visualizer_t.h"
#include "static_data/chip_t.h"
#include "static_data/module_t.h"
#include <thread>
#include <iostream>
#include <sstream>
#include <iomanip>

void my_out(const std::string& str){
#ifdef USING_VIS
    std::cout << str << std::endl;
#endif
}
    srand(time(NULL));
void visualizer_t::show_fp(const std::vector<bounding_rectangle_t>& bd_rect) {
    std::vector<std::pair<std::vector<vec2d_t>,std::string>> bd_rect_pt;
    for(auto & i : bd_rect){
        const double rx = i.getRect().get_right_upper().get_x();
        const double lx = i.getRect().get_left_lower().get_x();
        const double uy = i.getRect().get_right_upper().get_y();
        const double ly = i.getRect().get_left_lower().get_y();
        bd_rect_pt.push_back({std::vector<vec2d_t>{{lx, ly}, {rx, ly}, {rx, uy}, {lx,uy}},
                              i.getLinkModule()->getName()});
    }
    return ;
}
