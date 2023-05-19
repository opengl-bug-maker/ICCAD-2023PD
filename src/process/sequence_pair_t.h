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
#include "edge_t.h"
#include "ILP_solver_t.h"
using std::vector;
using std::unordered_map;

class sequence_pair_t{
    vector<int> h_sequence, v_sequence;

    vector<edge_t> constraint_graph_h, constraint_graph_v;
    static vector<edge_t> connections;
    static vector<vector<int>> connectivities;
public:
    floorplanning_t fp;
    vector<vec2d_t> modules_wh;
    vector<vec2d_t> modules_pos;
    pair<bool, vector<vec2d_t>> find_position(int);
    void build_constraint_graph();
    sequence_pair_t();
    static vector<bool> seq_is_fix;
    static vector<vector<vec2d_t>> soft_area_to_w_h_m; //area -> (w, h)
    static vector<vec2d_t> find_w_h(uint32_t area);
    static vector<soft_module_t*> seq_soft_map;
    static vector<fixed_module_t*> seq_fixed_map;
    static void init();
    static const int inf = 1e9+10;
    static int sequence_n;
    static unordered_map<const module_t*, int> soft_module_to_id_m;
    static unordered_map<const module_t*, int> fix_module_to_id_m;


    //essential
    bool to_fp();
    void seq_randomize();
    void set_fix_sequence();

    //get & set
    void set_v(std::vector<int>);
    void set_h(std::vector<int>);
    std::vector<int> get_v();
    std::vector<int> get_h();

    //debug
    void print();
    void sequence_pair_validation(pair<bool, vector<vec2d_t>>);

};




#endif //ICCAD2023PD_SEQUENCE_PAIR_T_H
