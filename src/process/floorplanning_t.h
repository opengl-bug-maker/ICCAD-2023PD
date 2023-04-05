//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_FLOORPLANNING_T_H
#define INC_2023PD_FLOORPLANNING_T_H
#include <vector>
#include <unordered_map>
#include "polygon/polygon_forest_t.h"
#include "utilities/bounding_rectangle_t.h"
#include "static_data/chip_t.h"
#include "polygon/polygon_forest_t.h"
#include"static_data/fixed_module_t.h"
#include"static_data/soft_module_t.h"


//for debug
#include<iostream>
using std::vector;
using std::unordered_map;
using std::pair;
enum fp_status_t {
    in_progress = 0,
    fail_on_placing_fixed_modules = 1,
    fail_on_placing_soft_modules = 2,
    success_to_place_all_modules = 3
};
class floorplanning_t {
    //status
    fp_status_t fp_status = in_progress;
    //status(structure)
    const size_t module_n = chip_t::get_total_module_n();
    size_t soft_rect_n;
    size_t fixed_rect_n;
    vector<bounding_rectangle_t> soft_rects;
    vector<bounding_rectangle_t> fixed_rects;
    vector<bool> soft_is_placed;
  
    unordered_map<const module_t*, int> module_to_bd_soft_rect_i_m; // for evaluation
    unordered_map<const module_t*, int> module_to_bd_fixed_rect_i_m; // for evaluation

    // to keep fp valid (the area restriction)
    polygon_forest_t polygons;
    
    // evaluation & output 
    float wirelength;
    void calculate_wirelength();
    float bd_distance(const bounding_rectangle_t& a, const bounding_rectangle_t& b);
    uint64_t VE_calculator(const bounding_rectangle_t& bd_rect, pair<const module_t* const, const int> neighbor);

    
public:
    void print_info();
    floorplanning_t();
    bool place_soft_module(size_t i, vec2d_t center, vec2d_t size);
    const size_t& get_soft_rect_n();
    
    //polygon_forest_t& get_polygon_forest();
    float get_wirelength();
};


#endif //INC_2023PD_FLOORPLANNING_T_H
