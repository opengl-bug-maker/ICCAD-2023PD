//
// Created by RayChang on 2023/5/6.
//

#ifndef ICCAD2023PD_SEQUENCE_PAIR_T_H
#define ICCAD2023PD_SEQUENCE_PAIR_T_H
#include <vector>
#include <iterator>
#include <unordered_map>
#include "static_data/chip_t.h"
#include "utilities/vec2d_t.h"
#include"static_data/soft_module_t.h"
#include"static_data/fixed_module_t.h"
#include "floorplanning_t.h"
using std::vector;
using std::unordered_map;
class sequence_pair_t{
    vector<uint16_t> h_sequence, v_sequence;
    vector<vec2d_t> modules_wh;
public:
    sequence_pair_t();
    static vector<bool> seq_is_fix;
    static vector<vector<vec2d_t>> soft_area_to_w_h_m; //area -> (w, h)
    static vector<vec2d_t> find_w_h(uint32_t area);
    static vector<soft_module_t*> seq_soft_map;
    static vector<fixed_module_t*> seq_fixed_map;

    static void init();

    //essential
    floorplanning_t to_fp();

    //get & set
    void set_v(std::vector<uint16_t>);
    void set_h(std::vector<uint16_t>);
    std::vector<uint16_t> get_v();
    std::vector<uint16_t> get_h();

    //debug
    void print();

};




#endif //ICCAD2023PD_SEQUENCE_PAIR_T_H
