#pragma once
#include<vector>
#include "process/sequence_pair_t.h"
#include "static_data/chip_t.h"
#include "process/ILP_solver_t.h"
using namespace std;
class quad_sequence_t{
public:
    quad_sequence_t();
    //quad_sequence_t(sequence_pair_t);
    void to_polygon();
    void build_constraint_graph();
    void build_constraint_graph_from_SP();
    void set_constraints_modules_overlap();
    void set_constraints_extended_modules();
    void set_constraints_nets();
    void set_constraints_avoid_zero_ex_w_h();
    void set_variables_modules();
    void set_variables_nets();
    void set_coef();


    vector<vector<int>> QS;
    vector<std::pair<std::vector<vec2d_t>,std::string>> modules_res;
    vector<vector<vector<int>>> G;
    vector<vector<int>> connections;
    vector<double> coef;
    
    int sequence_n, connections_n;
    int constraint_n, constraint_i, variable_n;
    int board_width = 11267;
    int board_height = 10450;
    //int module_w = 10, module_h = 10, exm_w = 1, exm_h = 1;
    vector<vec2d_t> modules_wh;
    vector<vec2d_t> modules_ex_wh;
    static const double inf;
    ILP_solver_t ILP_solver;
    ILP_result_t ILP_result;
    vector<int> x_modules_offsets, y_modules_offsets;
    vector<int> x_t_modules_offsets, y_l_modules_offsets;
    vector<int> x_b_modules_offsets, y_r_modules_offsets;
    
    vector<int> x_l_nets_offsets, x_r_nets_offsets, y_t_nets_offsets, y_b_nets_offsets;
    sequence_pair_t modules_relations_SP;
};