//
// Created by RayChang on 2023/4/27.
//
#include "fp_evaluator_t.h"
using std::cout;
using std::endl;
uint32_t fp_evaluator_t::get_score(floorplanning_t  fp) {
    size_t deg_c = 0;
    int wh = chip_t::get_width()+chip_t::get_height();
    for(size_t i = 0; i<fp.get_soft_rect_n(); ++i){
        if(fp.get_soft_is_placed()[i]==false){
            deg_c+= (fp.get_soft_deg()[i]*wh)+wh;
        }
    }
    int v = get_dead_space(fp);
    return fp.get_wirelength()+deg_c+v*v;
}

uint32_t fp_evaluator_t::get_dead_space(floorplanning_t fp) {
    vector<rect_t> empty_rects = fp.get_polygon_forest().get_empty_spaces();
    uint32_t dead_area = 0;
    for(auto& empty_rect:empty_rects){
        if(empty_rect.get_size().get_x()<floorplanning_t::min_w_h[0] || empty_rect.get_size().get_y()<floorplanning_t::min_w_h[1]){
            dead_area+=empty_rect.get_area();
        }
    }
    return dead_area;
}

