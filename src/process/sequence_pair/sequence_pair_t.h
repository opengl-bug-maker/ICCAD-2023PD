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
#include "process/floorplan/floorplan_t.h"
#include "process/components/edge_t.h"
#include "process/functional/ILP_solver_t.h"
#include "process/components/timer.h"
#include "process/components/net_t.h"


using std::vector;
using std::unordered_map;

class sequence_pair_t{
    friend class sp_ilp_settings_find_position_t;
    friend class sp_ilp_settings_find_position_with_area_t;
public: 
    //constructor
    sequence_pair_t();
    //essential
    bool find_position(bool,bool,int,int); // verify if the current sequence pair form the legal position
    bool find_position_with_area(bool,bool,int,int);
    void predict_wirelength(bool, bool);
    void to_rectilinear();
    void to_rectilinear_and_plot();
    floorplan_t to_fp();


    //get & set
    void swap_v(int,int);
    void swap_h(int,int);

    void set_v(std::vector<int>);
    void set_h(std::vector<int>);
    void set_vi(int,int);
    void set_hi(int,int);
    void set_module_size(int i, int j);

    int get_vi(int);
    int get_hi(int);
    double update_wirelength(bool minimize, bool with_area);
    std::vector<int> get_v();
    std::vector<int> get_h();


    vector<vec2d_t> modules_wh;
    vector<int> modules_wh_i;
    vector<vec2d_t> modules_positions;
    vector<int> h_sequence, v_sequence, fix_sequence_v, fix_sequence_h;
    
    static int sequence_n; //number of sequences number
        //static function
    static void init();
    static void build_graph();
    static vector<vec2d_t> find_w_h(uint32_t area, int); //calculate a legal shapes for a specific area


    //static variables
    static int fix_n;
    static int soft_n;
    
    static int connection_graph_deg;

    static int fix_start_idx; //the sequence number of the first fix module
    static vector<net_t> connections; //all edges (only one direction)
    static vector<vector<int>> connections_VE; //VE graph
    static vector<pair<int, int>> deg_w;
    static vector<bool> seq_is_fix; //if the module is a fixed module (so the array should be [0,0,...,0,1,...1]
    static vector<vector<vec2d_t>> soft_area_to_w_h_m_5; //area -> (w, h)
    static vector<vector<vec2d_t>> soft_area_to_w_h_m_9; //area -> (w, h)
    static vector<soft_module_t*> seq_soft_map; // an array with size equal to # of modules
    static vector<fixed_module_t*> seq_fixed_map; // an array with size equal to # of modules
    static vector<double> modules_area;
    static unordered_map<const module_t*, int> soft_module_to_id_m; //from the module to its seq# (for building the connections_VE graph)
    static unordered_map<const module_t*, int> fix_module_to_id_m;//from the module to its seq# (for building the connections_VE graph)
    double predicted_wirelength = -1;
    vector<int> add_soft_order;
    vector<int> is_in_seq;
    std::vector<std::pair<std::vector<vec2d_t>,std::string>> bouding_lines;
    vector<int> carved;
    vector<vec2d_t> get_4_points(vec2d_t, vec2d_t);

    void fill_near();
    void overlap_optimization();
    void carve();
    void change_size(int);
    void print_inline();
    void write_inline();
    void sequence_pair_validation();
    void print_result();
    void set_bounding_lines();

    double rectilinear_wirelength = 0;
private:

    


    //initialization
    void set_only_fix();
    void init_modules_size();
    void set_fix_sequence();
    void set_is_in_seq();
    void set_add_order();

    vector<vec2d_t> get_LP_res_pos();
    pair<vector<vec2d_t>, vector<int>> get_LP_res_wh();

    //subfunctions
    bool is_completed();
    void build_constraint_graph();
    void simplify_constraint_graph();
    void swap_seq_number(int a, int b,bool, bool);
    

    //debug
    void print();
    
    void print_v();
    void print_h();
    void print_logs();
    
    void print_shapes_i();
    void print_fix_sequence();
    void print_connections();
    
    
    void print_wirelength(bool,bool);



    //properties for LP
    int constraint_n, constraint_i, variable_n;
    int x_module_offset, y_module_offset, x_edge_offset_l, x_edge_offset_r, y_edge_offset_l, y_edge_offset_r;
    int near_x_offset, near_y_offset;
    vector<vector<int>> near_x_map,near_y_map;
    vector<vector<int>> near_x_id, near_y_id;
    vector<int> shape_types;
    vector<vector<int>> near_x, near_y;
    ILP_solver_t ILP_solver;
    ILP_result_t ILP_result;
    vector<vector<int>> result_carving_x, result_carving_y;
    
    //properties
    vector<edge_t> constraint_graph_h, constraint_graph_v;
    //debug properties
    vector<pair<double, double>> logs;

};




#endif //ICCAD2023PD_SEQUENCE_PAIR_T_H
