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
    //static function
    static void init();
    static void build_graph();
    static vector<vec2d_t> find_w_h(uint32_t area); //calculate a legal shapes for a specific area


    //static variables
    static int fix_n;
    static int soft_n;
    static int sequence_n; //number of sequences number

    static int fix_start_idx; //the sequence number of the first fix module
    static vector<edge_t> connections; //all edges (only one direction)
    static vector<vector<int>> connections_VE; //VE graph
    static vector<bool> seq_is_fix; //if the module is a fixed module (so the array should be [0,0,...,0,1,...1]
    static vector<vector<vec2d_t>> soft_area_to_w_h_m; //area -> (w, h)
    static vector<soft_module_t*> seq_soft_map; // an array with size equal to # of modules
    static vector<fixed_module_t*> seq_fixed_map; // an array with size equal to # of modules
    static vector<double> modules_area;
    static unordered_map<const module_t*, int> soft_module_to_id_m; //from the module to its seq# (for building the connections_VE graph)
    static unordered_map<const module_t*, int> fix_module_to_id_m;//from the module to its seq# (for building the connections_VE graph)

    //constructor
    sequence_pair_t();


    //initialization
    void set_only_fix();
    void set_modules_size();
    void set_fix_sequence();
    void set_is_in_seq();

    //essential
    bool find_position(bool,bool,int,int); // verify if the current sequence pair form the legal position
    pair<bool, floorplan_t> place_all_modules(vector<vec2d_t>);
    bool add_soft_process(int);
    void wire_length_predict(bool);
    floorplan_t to_fp();


    //subfunctions
    bool is_completed();
    bool place_8d(floorplan_t&,int i, vec2d_t,vec2d_t,int);
    void build_constraint_graph();
    void change_size(int);

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
    void print_fix_sequence();
    void print_connections();


    //properties
    long long predicted_wirelength = 1e13;
    vector<int> h_sequence, v_sequence, fix_sequence_v, fix_sequence_h;
    vector<edge_t> constraint_graph_h, constraint_graph_v;
    vector<int> is_in_seq;
    vector<vec2d_t> modules_wh;
    vector<vec2d_t> modules_pos;
    vector<vec2d_t> positions;


    void set_size(int i, int j);
};




#endif //ICCAD2023PD_SEQUENCE_PAIR_T_H
