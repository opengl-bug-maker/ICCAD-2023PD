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
#include "floorplan_t.h"
#include "edge_t.h"
#include "ILP_solver_t.h"
using std::vector;
using std::unordered_map;

class sequence_pair_enumerator_t;
class sequence_pair_t{


public:
    static vector<edge_t> connections;
    static vector<vector<int>> connectivities;
    static void build_graph();
    static int fix_start_idx;
    static void init();
    static vector<bool> seq_is_fix; //if the module is a fixed module
    static vector<vector<vec2d_t>> soft_area_to_w_h_m; //area -> (w, h)
    static vector<vec2d_t> find_w_h(uint32_t area);
    static vector<soft_module_t*> seq_soft_map; // an array with size equal to # of modules
    static vector<fixed_module_t*> seq_fixed_map; // an array with size equal to # of modules
    static int sequence_n;
    static unordered_map<const module_t*, int> soft_module_to_id_m;
    static unordered_map<const module_t*, int> fix_module_to_id_m;
    sequence_pair_t();
    vector<int> h_sequence, v_sequence;
    vector<edge_t> constraint_graph_h, constraint_graph_v;
    vector<edge_t> constraint_graph_h_soft, constraint_graph_v_soft;
    vector<int> soft_seq_h, soft_seq_v;
    vector<int> is_in_seq;
    int max_overlap;
    int max_distance;
    //floorplan_t fp;
    vector<vec2d_t> modules_wh;
    vector<vec2d_t> modules_pos;
    pair<bool, vector<vec2d_t>> find_position(int,int);
    void build_constraint_graph();


    //essential
    pair<bool, floorplan_t> get_fp();
    bool place_8d(floorplan_t&,int i, vec2d_t,vec2d_t);
    pair<bool, floorplan_t> place_all_modules(vector<vec2d_t>);
    bool add_soft_process(int);

    //get & set
    void set_v(std::vector<int>);
    void set_h(std::vector<int>);
    void set_vi(int,int);
    void set_hi(int,int);
    void swap_v(int,int);
    void swap_h(int,int);

    std::vector<int> get_v();
    std::vector<int> get_h();
    int get_vi(int);
    int get_hi(int);

    //debug
    void print();
    void print_inline();
    void sequence_pair_validation(vector<vec2d_t>);
    void print_shapes();

    void print_soft_inline();

    pair<bool, vector<vec2d_t>> find_soft_position(int overlap_h, int overlap_v);

    void build_constraint_graph_soft();
};


class sequence_pair_enumerator_t{
    int seed_need_n = 0; //how many seed do we need
    int fix_n = 0;
    int soft_n = 0;

public:
    sequence_pair_enumerator_t();
    vector<int> seed; //for permutation
    //vector<sequence_pair_t> valid_seq; // (# of seeds)^2
    vector<int> fix_sequence_v, fix_sequence_h;
    vector<vector<int>> seeds; //all seeds

    sequence_pair_t seq; //pop one sequence

    void fill_seeds(int n);
    void set_seed_need_n(int n);
    void generate_valid_seq(int x);
    void print_all_valid_seq();
    void seq_randomize();
    void seq_init();
    void change_size_for_i(int);
    void set_fix_sequence();
    void find_illegal_pair_for_i(int);
    void set_only_fix();
    vector<vector<int>> soft_seq_interval;
    vector<vector<vector<int>>> legal_pairs; // 0->left, 1->right, 2 ->upper, 3->bottom //soft->fix  illegal
    std::set<int> random_choose(int upb, int x);
    //set get
    vector<sequence_pair_t> get_all_valid_seq();

    void search_legal_perm_in_fix(int);
    int sample_from_interval(int,int);
    void randomize(vector<int>&);
};

#endif //ICCAD2023PD_SEQUENCE_PAIR_T_H
