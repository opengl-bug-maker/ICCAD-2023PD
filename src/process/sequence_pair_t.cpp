//
// Created by RayChang on 2023/5/6.
//
#include "sequence_pair_t.h"
#include <glpk.h>
#include <iostream>
#include <string>
#include <plugin/visualizer_t.h>
#include <chrono>
#include <stack>
#include <queue>
#include "timer.h"
#include <iomanip>
#include "random_helper.h"
#include "static_data/fp_rule_t.cpp" //how bad
int sequence_pair_t::sequence_n;
int sequence_pair_t::fix_start_idx;
int sequence_pair_t::fix_n;
int sequence_pair_t::soft_n;
int sequence_pair_t::connection_graph_deg;
vector<std::vector<vec2d_t>> sequence_pair_t::soft_area_to_w_h_m_5;
vector<std::vector<vec2d_t>> sequence_pair_t::soft_area_to_w_h_m_9;
vector<bool> sequence_pair_t::seq_is_fix;
vector<soft_module_t*> sequence_pair_t::seq_soft_map;
vector<fixed_module_t*> sequence_pair_t::seq_fixed_map;
unordered_map<const module_t*, int> sequence_pair_t::soft_module_to_id_m;
unordered_map<const module_t*, int> sequence_pair_t::fix_module_to_id_m;
vector<net_t> sequence_pair_t::connections;
vector<vector<int>> sequence_pair_t::connections_VE;
vector<pair<int,int>> sequence_pair_t::deg_w;
vector<double> sequence_pair_t::modules_area;
using std::cout;
using std::endl;
using std::string;
void sequence_pair_t::init() {
    const std::vector<soft_module_t*>& soft_modules = chip_t::get_soft_modules();
    const std::vector<fixed_module_t*>& fixed_modules = chip_t::get_fixed_modules();
    sequence_pair_t::fix_n =  static_cast<int>(chip_t::get_fixed_modules().size());
    sequence_pair_t::soft_n = static_cast<int>(chip_t::get_soft_modules().size());
    sequence_pair_t::sequence_n = static_cast<int>(chip_t::get_total_module_n());
    sequence_pair_t::fix_start_idx = static_cast<int>(chip_t::get_soft_modules().size());

    soft_area_to_w_h_m_5.resize(soft_modules.size());
    soft_area_to_w_h_m_9.resize(soft_modules.size());
    sequence_pair_t::seq_soft_map.resize(chip_t::get_total_module_n());
    sequence_pair_t::seq_fixed_map.resize(chip_t::get_total_module_n());
    sequence_pair_t::seq_is_fix.resize(chip_t::get_total_module_n());
    sequence_pair_t::modules_area.resize(chip_t::get_total_module_n());

    connections_VE.resize(sequence_n);
    for(int i = 0; i<sequence_n; ++i){connections_VE[i].resize(sequence_n);}

    deg_w = vector<pair<int,int>>(sequence_n, {0, 0});



    //build up the map between module and sequence number
    int j = 0;
    for(int i = 0; i<soft_modules.size(); ++i){
        sequence_pair_t::seq_soft_map[j] = soft_modules[i];
        sequence_pair_t::soft_area_to_w_h_m_5[i] = sequence_pair_t::find_w_h(soft_modules[i]->get_area(), 5);
        sequence_pair_t::soft_area_to_w_h_m_9[i] = sequence_pair_t::find_w_h(soft_modules[i]->get_area(), 9);
        sequence_pair_t::modules_area[j] = soft_modules[i]->get_area();
        sequence_pair_t::seq_fixed_map[j] = nullptr;
        sequence_pair_t::seq_is_fix[j] = false;
        sequence_pair_t::soft_module_to_id_m[soft_modules[i]] = j;
        j++;
    }
    for(int i = 0; i<fixed_modules.size(); ++i){
        sequence_pair_t::seq_fixed_map[j] = fixed_modules[i];
        sequence_pair_t::modules_area[j] = fixed_modules[i]->get_area();
        sequence_pair_t::seq_soft_map[j] = nullptr;
        sequence_pair_t::seq_is_fix[j] = true;
        sequence_pair_t::fix_module_to_id_m[fixed_modules[i]] = j;
        j++;
    }


    //build adj list and adj matrix
    build_graph();
}

sequence_pair_t::sequence_pair_t() {
    init_modules_size();
    set_is_in_seq();
    set_only_fix();
    set_add_order();
}

void sequence_pair_t::set_v(std::vector<int> v_sequence) {
    this->v_sequence = v_sequence;
}

void sequence_pair_t::set_h(std::vector<int> h_sequence) {
    this->h_sequence = h_sequence;
}

std::vector<int> sequence_pair_t::get_v() {
    return this->v_sequence;
}

std::vector<int> sequence_pair_t::get_h() {
    return this->h_sequence;
}

vector<vec2d_t> sequence_pair_t::find_w_h(uint32_t area, int resol) {
    //select only 5 shapes
    if(area==1){return {{1,1}};}
    double p = sqrt(static_cast<double>(area)*fp_rule_t::get_lower_ratio());
    double b = pow(fp_rule_t::get_upper_ratio(), 1.00/static_cast<double>(resol-1));
    vector<vec2d_t> res;

    for(int i = 0; i<=resol-1; ++i){ //i will be the shorter edge
        double x = p*pow(b, i);
        double y = area/x;

        if(ceil(x)*fp_rule_t::get_upper_ratio()>=ceil(y)){
            double xx = ceil(x), yy = ceil(y);
            if(xx-1>=yy * fp_rule_t::get_lower_ratio()&& (xx-1)*yy>=area){
                xx-=1;
            }
            if(yy-1>=fp_rule_t::get_lower_ratio()*xx && (yy-1)*xx>=area){
                yy-=1;
            }
            res.push_back({xx+1, yy+1});
        }
    }
    return res;

}


void sequence_pair_t::build_constraint_graph() {
    this->constraint_graph_h.clear(); this->constraint_graph_v.clear();
    vector<int> v_map(sequence_pair_t::sequence_n),h_map(sequence_pair_t::sequence_n);
    for(int i = 0; i<v_sequence.size(); ++i){
        v_map[v_sequence[i]] = i;
        h_map[h_sequence[i]] = i;
    }
    for(int i = 0; i<sequence_n; ++i){
        for(int j = 0; j<sequence_n; ++j) {
            if(is_in_seq[i]==0||is_in_seq[j]==0){continue;}
            if(v_map[i]<v_map[j] && h_map[i]<h_map[j]){ //{a, b}, {a, b}
                this->constraint_graph_h.push_back({i, j, static_cast<int>(this->modules_wh[i].get_x())});
            }
            else if(v_map[i]<v_map[j] && h_map[j]<h_map[i]) { //{i, j}, {j,i}
                this->constraint_graph_v.push_back({i, j, static_cast<int>(this->modules_wh[i].get_y())});
            }
        }
    }
    simplify_constraint_graph();
}
void sequence_pair_t::set_constraints_modules_overlap_h(){
    for(int i = 0; i<this->constraint_graph_h.size(); ++i){
        int from = constraint_graph_h[i].from, to = constraint_graph_h[i].to,w = constraint_graph_h[i].w;
        string constraint_name = "h_c"+ std::to_string(i);

        if(this->seq_is_fix[from]){
            ILP_solver.set_constraint_upb(constraint_i, 2, {from+x_module_offset, to+x_module_offset}, {1, -1}, constraint_name, -w);
        }
        else{
            double area = this->modules_area[from];
            vector<double> w(5);
            for(int j = 0; j<5; ++j){
                w[j] = sequence_pair_t::soft_area_to_w_h_m_5[from][j].get_x();
            }
            ILP_solver.set_constraint_upb(constraint_i, 7,
                                  {from+x_module_offset, to+x_module_offset,
                                           shape_types[0] + from,
                                           shape_types[1] + from,
                                           shape_types[2] + from,
                                           shape_types[3] + from,
                                           shape_types[4] + from},
                                      {1, -1, w[0],w[1] ,w[2],w[3], w[4]}, constraint_name, -1e-5);
        }
        constraint_i++;
    }
}
void sequence_pair_t::set_constraints_modules_overlap_v(){
    for(int i = 0; i<this->constraint_graph_v.size(); ++i){
        int from = constraint_graph_v[i].from, to = constraint_graph_v[i].to, w = constraint_graph_v[i].w;
        string constraint_name = "v_c"+ std::to_string(i);

        if(this->seq_is_fix[from]){
            ILP_solver.set_constraint_upb(constraint_i, 2, {from+y_module_offset, to+y_module_offset}, {1, -1}, constraint_name, -w);
        }
        else{
            double area = this->modules_area[from];
            vector<double> h(5);
            for(int j = 0; j<5; ++j){
                h[j] = sequence_pair_t::soft_area_to_w_h_m_5[from][j].get_y();    
            }
            ILP_solver.set_constraint_upb(constraint_i, 7,
                              {from+y_module_offset, to+y_module_offset,
                                       shape_types[0] + from,
                                       shape_types[1] + from,
                                       shape_types[2] + from,
                                       shape_types[3] + from,
                                       shape_types[4] + from},
                               {1, -1,h[0],h[1],h[2],h[3], h[4]}, constraint_name, -1e-5);
        }
        constraint_i++;
    }
}
void sequence_pair_t::set_constraints_modules_fixed(){
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        if(sequence_pair_t::seq_is_fix[i]){
            string x_constraint_name = "fix_x_c"+ std::to_string(i);
            string y_constraint_name = "fix_y_c"+ std::to_string(i);
            vec2d_t ll_pos = chip_t::get_modules()[i]->get_left_lower();
            ILP_solver.set_constraint_fx(constraint_i, 1, {i+x_module_offset}, {1}, x_constraint_name, static_cast<int>(ll_pos.get_x()));
            constraint_i++;
            ILP_solver.set_constraint_fx(constraint_i, 1, {i+y_module_offset}, {1}, y_constraint_name, static_cast<int>(ll_pos.get_y()));
            constraint_i++;
        }
    }
}
void sequence_pair_t::set_constraints_net_direction(){
    //2 * connections
    for(int i = 0; i<this->connections.size(); ++i){
        string x_constraint_name = "x_r_e"+ std::to_string(i);
        string y_constraint_name = "y_r_e"+ std::to_string(i);
        ILP_solver.set_constraint_upb(constraint_i, 2, {x_edge_offset_l+i, x_edge_offset_r+i}, {1, -1}, x_constraint_name, 0.0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {y_edge_offset_l+i, y_edge_offset_r+i}, {1, -1}, y_constraint_name, 0.0);
        constraint_i++;
    }
}
void sequence_pair_t::set_constraints_net(){
    //nodes* 4
    for(int i = 0; i<connections.size(); ++i){
        for(int j = 0; j<connections[i].nodes.size(); ++j){
            int b = connections[i].nodes[j];
            double b_x = this->modules_wh[b].get_half_x();
            double b_y = this->modules_wh[b].get_half_y();
            string x_l_constraint_name = "x_e_b_l"+ std::to_string(i)+"_"+std::to_string(j);
            string y_l_constraint_name = "y_e_b_l"+ std::to_string(i)+"_"+std::to_string(j);
            string x_r_constraint_name = "x_e_b_r"+ std::to_string(i)+"_"+std::to_string(j);
            string y_r_constraint_name = "y_e_b_r"+ std::to_string(i)+"_"+std::to_string(j);


            if(seq_is_fix[b]){
                ILP_solver.set_constraint_upb(constraint_i, 2, {x_edge_offset_l+i,b+x_module_offset}, {1, -1}, x_l_constraint_name, b_x);
                constraint_i++;
                
                ILP_solver.set_constraint_upb(constraint_i, 2,{y_edge_offset_l+i,b+y_module_offset},{1, -1}, y_l_constraint_name, b_y);
                constraint_i++;

                ILP_solver.set_constraint_upb(constraint_i, 2,{b+x_module_offset,x_edge_offset_r+i},{1, -1}, x_r_constraint_name, -b_x);
                constraint_i++;
                ILP_solver.set_constraint_upb(constraint_i, 2,{b+y_module_offset,y_edge_offset_r+i},{1, -1}, y_r_constraint_name, -b_y);
                constraint_i++;
            }
            else{
                vector<double> b_w(5), b_h(5);
                for(int k = 0; k<5; ++k){
                    b_w[k] = sequence_pair_t::soft_area_to_w_h_m_5[b][k].get_half_x();
                    b_h[k] = sequence_pair_t::soft_area_to_w_h_m_5[b][k].get_half_y();
                }
                ILP_solver.set_constraint_upb(constraint_i, 7,
                                            {x_edge_offset_l+i,b+x_module_offset,b+shape_types[0], b+shape_types[1], b+shape_types[2], b+shape_types[3], b+shape_types[4]},
                                            {1, -1,-b_w[0],-b_w[1],-b_w[2],-b_w[3],-b_w[4]}, x_l_constraint_name, 0);
                constraint_i++;
                ILP_solver.set_constraint_upb(constraint_i, 7,
                                            {y_edge_offset_l+i,b+y_module_offset,b+shape_types[0], b+shape_types[1], b+shape_types[2], b+shape_types[3], b+shape_types[4]},
                                            {1, -1,-b_h[0],-b_h[1],-b_h[2],-b_h[3],-b_h[4]}, y_l_constraint_name, 0);
                constraint_i++;

                ILP_solver.set_constraint_upb(constraint_i, 7,
                                            {b+x_module_offset,x_edge_offset_r+i,b+shape_types[0], b+shape_types[1], b+shape_types[2], b+shape_types[3], b+shape_types[4]},
                                            {1, -1,b_w[0],b_w[1],b_w[2],b_w[3],b_w[4]}, x_r_constraint_name, 0);
                constraint_i++;
                ILP_solver.set_constraint_upb(constraint_i, 7,
                                            {b+y_module_offset,y_edge_offset_r+i,b+shape_types[0], b+shape_types[1], b+shape_types[2], b+shape_types[3], b+shape_types[4]},
                                            {1, -1,b_h[0],b_h[1],b_h[2],b_h[3],b_h[4]}, y_r_constraint_name, 0);
                constraint_i++;
            }
        }
    }
}
void sequence_pair_t::set_constraints_ratio_equal_1(){
    for(int i = 0; i<soft_n; ++i){
        string constraint_name1 = "w_h_1"+ std::to_string(i);
        ILP_solver.set_constraint_fx(constraint_i, 5,
        {shape_types[0] + i, shape_types[1] + i,shape_types[2] + i,shape_types[3]+i, shape_types[4]+i}, {1, 1,1,1,1}, constraint_name1, 1.001);
        constraint_i++;
    }
}
void sequence_pair_t::set_constraints_boundaries(){
    for(int i = 0; i<soft_n; ++i){
        string  constraint_name1 = "x_bound"+ std::to_string(i);
        string  constraint_name2 = "y_bound"+ std::to_string(i);
        vector<double> w(5), h(5);
        for(int j = 0; j<5; ++j){
            w[j] = sequence_pair_t::soft_area_to_w_h_m_5[i][j].get_x();
            h[j] = sequence_pair_t::soft_area_to_w_h_m_5[i][j].get_y();
        }


        ILP_solver.set_constraint_upb(constraint_i, 6,
            {x_module_offset+i, shape_types[0] + i, shape_types[1] + i,  shape_types[2] + i,shape_types[3] + i,shape_types[4] + i},
            {1, w[0], w[1],w[2],w[3],w[4]}, constraint_name1, chip_t::get_width());


        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 6,
            {y_module_offset+i, shape_types[0] + i,shape_types[1] + i, shape_types[2] + i,shape_types[3] + i,shape_types[4] + i},
            {1, h[0], h[1],h[2],h[3],h[4]}, constraint_name2, chip_t::get_height());
        constraint_i++;
    }
}
void sequence_pair_t::set_variables_modules(){
    for(int i = 1;i<=sequence_n; ++i){
        string var_name = "x"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, i, var_name.c_str());
        ILP_solver.set_variable_double_range(i, 0.0,chip_t::get_width());

    }
    for(int i = sequence_pair_t::sequence_n+1;i<=2*sequence_n; ++i){
        string var_name = "x"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, i, var_name.c_str());
        ILP_solver.set_variable_double_range(i, 0.0, chip_t::get_height());
    }
}
void sequence_pair_t::set_variables_connections(){
    for(int i = 0; i<connections.size(); ++i){
        string var_name1 = "x_e_l"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, x_edge_offset_l+i, var_name1.c_str());
        ILP_solver.set_variable_double_range(x_edge_offset_l+i, 0.0, chip_t::get_width());

        string var_name2 = "x_e_r"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, x_edge_offset_r+i, var_name2.c_str());
        ILP_solver.set_variable_double_range(x_edge_offset_r+i, 0.0, chip_t::get_width());

        string var_name3 = "y_e_l"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, y_edge_offset_l+i, var_name3.c_str());
        ILP_solver.set_variable_double_range(y_edge_offset_l+i, 0.0, chip_t::get_height());

        string var_name4 = "y_e_r"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, y_edge_offset_r+i, var_name4.c_str());
        ILP_solver.set_variable_double_range(y_edge_offset_r+i, 0.0, chip_t::get_height());
    }
}
void sequence_pair_t::set_variables_shapes(){
    for(int i = 0; i<soft_n; ++i){
        for(int j = 0; j<5; ++j){
            string var_name = "shape"+std::to_string(j)+ std::to_string(i);
            glp_set_col_name(ILP_solver.ILP, shape_types[j] + i, var_name.c_str());
            ILP_solver.set_variable_double_range(shape_types[j] + i, 0, 1);    
        }
    }
}
void sequence_pair_t::set_coef(vector<double>& coef){
    for(int i = 0; i<connections.size(); ++i){
        coef[i+x_edge_offset_r] = connections[i].w;
        coef[i+x_edge_offset_l] = -connections[i].w;
        coef[i+y_edge_offset_r] = connections[i].w;
        coef[i+y_edge_offset_l] = -connections[i].w;
    }
}

vector<vec2d_t> sequence_pair_t::get_LP_res_pos(){
    vector<vec2d_t> result_pos;
    for(int i = 0; i<sequence_n; ++i){
        if(this->is_in_seq[i]==0){
            result_pos.push_back({-1, -1});
        }
        else {
            result_pos.push_back({static_cast<int>(ILP_result.var_values[i+x_module_offset]),
                                    static_cast<int>(ILP_result.var_values[i+y_module_offset])});
        }
    }
    return result_pos;
}
pair<vector<vec2d_t>, vector<int>> sequence_pair_t::get_LP_res_wh(){
    vector<vec2d_t> result_wh;
    vector<int> result_wh_i;
     for(int i = 0; i<soft_n; ++i){
        if(this->is_in_seq[i]==0){
            result_wh.push_back({-1, -1});
            result_wh_i.push_back(-1);
        }
        else{
            vector<double> shapes(5);
            for(int j = 0; j<5; ++j){
                shapes[j] = ILP_result.var_values[i + shape_types[j]];
            }
            double x = 0, y = 0;
            for(int j = 0; j<5; ++j){
                x+=shapes[j]*sequence_pair_t::soft_area_to_w_h_m_5[i][j].get_x();
                y+=shapes[j]*sequence_pair_t::soft_area_to_w_h_m_5[i][j].get_y();
            }
            x = floor(x);
            y = floor(y);
            result_wh.push_back({x, y});
            result_wh_i.push_back(-1); // the shape is determined by LP
        }
    }
    return {result_wh, result_wh_i};
}
bool sequence_pair_t::find_position(bool minimize_wirelength, bool load_result,int overlap_h, int overlap_v) {
    build_constraint_graph();
    constraint_n = this->constraint_graph_h.size() + this->constraint_graph_v.size() + chip_t::get_fixed_modules().size()*2 + 5*soft_n;
    constraint_i = 1; //constraint_counter
    variable_n = 2*sequence_n + 5*soft_n;
    if(minimize_wirelength){
        variable_n+=4*sequence_pair_t::connections.size();
        constraint_n+=(2+ 4 * sequence_pair_t::connection_graph_deg)*sequence_pair_t::connections.size();
        //constraint_n+=10*sequence_pair_t::connections.size();
    }
    x_module_offset = 1;
    y_module_offset = 1+sequence_n;
    shape_types.resize(5);
    for(int i = 0; i<5; ++i){
        shape_types[i] = 1+2*sequence_n+i*soft_n;
    }
    x_edge_offset_l = 1+2*sequence_n+5*soft_n;
    x_edge_offset_r = 1+2*sequence_n+5*soft_n+this->connections.size();
    y_edge_offset_l = 1+2*sequence_n+5*soft_n+2*this->connections.size();
    y_edge_offset_r = 1+2*sequence_n+5*soft_n+3*this->connections.size();

    //apart from constraint graph, every fix module need 2 additional condition
    ILP_solver = ILP_solver_t();
    ILP_solver.init("ILP_solver", constraint_n, variable_n);
    if(ILP_solver.get_is_invalid()){return false;}
    ILP_solver.set_min();


    vector<double> coef(variable_n+1, 0);

    //set constraints to place modules
    //if horizontal constraint graph got i->j them x_j - x_i >= w
    set_constraints_modules_overlap_h();
    //if vertical constraint graph got i->j them y_j - y_i >= h
    set_constraints_modules_overlap_v();
    //set constraints to fix module
    set_constraints_modules_fixed();
    
    if(minimize_wirelength){
        //set constraints to give the direction to the edges
        //x_e_r >= x_e_l
        set_constraints_net_direction();
        //set edge and block left(bottom) constraints
        set_constraints_net();
    }
    //w+h = 1
    set_constraints_ratio_equal_1();
    //set modules' boundaires
    set_constraints_boundaries();
    //set variables of modules
    set_variables_modules();
    if(minimize_wirelength){
        set_variables_connections();
    }
    set_variables_shapes();


    //set coefficient of the objective function
    if(minimize_wirelength){
        set_coef(coef);
    }
    //solve
    ILP_solver.set_obj_coef(coef);
    ILP_solver.load();
    ILP_result = ILP_solver.solve(false);


    if(ILP_result.legal){
        if(load_result){
            vector<vec2d_t> result_pos = get_LP_res_pos();
            auto[result_wh, result_wh_i] = get_LP_res_wh();
           
            for(int i = 0; i<sequence_pair_t::soft_n; ++i){
                this->modules_wh[i] = result_wh[i];
            }
            this->modules_wh_i = result_wh_i;
            this->modules_positions = result_pos;
        }
        return true;
    }
    else{
        return false;
    }

}
void sequence_pair_t::sequence_pair_validation() {
    vector<std::pair<rect_t, std::string>> rects;
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        if(this->modules_positions[i].get_x() < 0 || this->modules_positions[i].get_y() < 0){continue;}
        if(this->seq_is_fix[i]){
            rects.push_back({{this->modules_positions[i], this->modules_wh[i]}, "f" + std::to_string(i)});
        }
        else{
            rects.push_back({{this->modules_positions[i], this->modules_wh[i]}, "s" + std::to_string(i)});
        }
    }
    visualizer_t::show_fp_rect_no_border(rects, "SQP");

}

void sequence_pair_t::build_graph() {
    connections.clear();
    sequence_pair_t::connection_graph_deg = 2;
    const std::vector<multi_net_t*> input_multi_nets = chip_t::get_multi_nets();
    for(int i = 0; i<sequence_n; ++i){
        deg_w[i].second = i;
    }
    for(multi_net_t* input_multi_net:input_multi_nets){
        vector<int> connected_id;
        for(pin_t* pin:input_multi_net->pins){
            connected_id.push_back(pin->module_index);
            deg_w[pin->module_index].first+=input_multi_net->weight;
        }
        sequence_pair_t::connection_graph_deg+=connected_id.size();
        sequence_pair_t::connections.push_back({connected_id, input_multi_net->weight});
    }
    // for(int i = 0; i<sequence_n; ++i) {
    //     deg_w[i].second = i;
    //     if(seq_is_fix[i]){
    //         const vector<std::pair<const module_t* const, const int>> neighbors
    //                 = (seq_fixed_map[i])->getConnections();
    //         for(const auto neighbor:neighbors){
    //             int neighbor_i = -1;
    //             if(sequence_pair_t::fix_module_to_id_m.count(neighbor.first)){
    //                 neighbor_i = fix_module_to_id_m[neighbor.first];
    //             }
    //             if(sequence_pair_t::soft_module_to_id_m.count(neighbor.first)) {
    //                 neighbor_i = soft_module_to_id_m[neighbor.first];
    //             }
    //             connections_VE[neighbor_i][i] = connections_VE[i][neighbor_i] = neighbor.second;
    //             deg_w[i].first+=neighbor.second;
    //             if(i>neighbor_i){
    //                 connections.push_back({{i, neighbor_i}, neighbor.second});
    //             }

    //         }
    //     }
    //     else{
    //         const vector<std::pair<const module_t* const, const int>> neighbors
    //                 = (seq_soft_map[i])->getConnections();
    //         for(const auto neighbor:neighbors){
    //             int neighbor_i = -1;
    //             if(sequence_pair_t::fix_module_to_id_m.count(neighbor.first)){
    //                 neighbor_i = fix_module_to_id_m[neighbor.first];
    //             }
    //             if(sequence_pair_t::soft_module_to_id_m.count(neighbor.first)) {
    //                 neighbor_i = soft_module_to_id_m[neighbor.first];
    //             }
    //             connections_VE[neighbor_i][i] = connections_VE[i][neighbor_i] = neighbor.second;
    //             deg_w[i].first+=neighbor.second;
    //             if(i>neighbor_i){
    //                 connections.push_back({{i, neighbor_i}, neighbor.second});
    //             }
    //         }
    //     }
    // }

    sort(deg_w.begin(), deg_w.end());
    std::reverse(deg_w.begin(), deg_w.end()); //incremental
}

void sequence_pair_t::print() {
    for(int i = 0; i<chip_t::get_total_module_n(); ++i){
        cout<< "seq# "<<i<<":";
        if(sequence_pair_t::seq_is_fix[i]){
            cout<<"fixed, ";
        }
        else{
            cout<< "soft, ";
        }
        cout<< "shape : ";
        cout<< this->modules_wh[i]<<endl;
    }
    cout<<"V: {";
    for(int i = 0; i<this->v_sequence.size(); ++i){
        cout<< v_sequence[i]<<" ";
    }
    cout<<"}"<<endl;
    cout<<"H: {";
    for(int i = 0; i<this->h_sequence.size(); ++i){
        cout<< h_sequence[i]<<" ";
    }
    cout<<"}"<<endl;

}

void sequence_pair_t::swap_v(int a,  int b) {
    std::swap(this->v_sequence[a], this->v_sequence[b]);
}

void sequence_pair_t::swap_h(int a, int b) {
    std::swap(this->h_sequence[a], this->h_sequence[b]);
}

void sequence_pair_t::print_inline() {
    cout<<"v: {";
    for(int i = 0; i<this->v_sequence.size(); ++i){
        cout<< v_sequence[i];
        if(i!=this->v_sequence.size()-1){cout<<", ";}
    }
    cout<<"} "<<endl;
    cout<<"h: {";
    for(int i = 0; i<this->h_sequence.size(); ++i){
        cout<< h_sequence[i];
        if(i!=this->h_sequence.size()-1){cout<<", ";}
    }
    cout<<"}"<<endl;
    this->print_shapes_i();
//    for(int i = 0; i<this->soft_n; ++i){
//        for(auto& shape:sequence_pair_t::soft_area_to_w_h_m_5[i]){
//            cout<<"{"<<shape.get_x()<<","<<shape.get_y()<<"} ";
//        }
//    }
    cout<<"wirelength : "<<std::setprecision(16)<<this->get_wirelength(true, true)<<endl;
}
void sequence_pair_t::print_shapes_i() {
    cout<<"shapes of the SP : {";
    for(int i = 0; i<soft_n; ++i){
        cout<<this->modules_wh_i[i];
        if(i!=soft_n-1){cout<<", ";}
        else{cout<<"}";}
    }
    cout<<endl;
}

void sequence_pair_t::set_vi(int i, int x) {
    this->v_sequence[i]  = x;
}

void sequence_pair_t::set_hi(int i , int x) {
    this->h_sequence[i]  =x;
}

int sequence_pair_t::get_vi(int i) {
    return this->v_sequence[i];
}

int sequence_pair_t::get_hi(int i) {
    return this->h_sequence[i];
}

void sequence_pair_t::change_size(int i) {
    if(sequence_pair_t::seq_is_fix[i]){
        this->modules_wh[i] = sequence_pair_t::seq_fixed_map[i]->get_size();
    }
    else{
        int id = static_cast<int>(random_helper::get_rand())%sequence_pair_t::soft_area_to_w_h_m_5[i].size();
        this->modules_wh[i] = soft_area_to_w_h_m_5[i][id];
        this->modules_wh_i[i] = id;
    }
}
void sequence_pair_t::set_module_size(int i, int j){
    this->modules_wh[i] = sequence_pair_t::soft_area_to_w_h_m_5[i][j];
    this->modules_wh_i[i] = j;
}



void sequence_pair_t::set_fix_sequence() {
    if(sequence_pair_t::fix_n==0){return;}
    for(int i =0; i < sequence_pair_t::fix_n; ++i){
        this->fix_sequence_h[i] = this->fix_sequence_v[i] = i+sequence_pair_t::fix_start_idx;
    }
    vector<vector<int>> fix_constraint_v(sequence_pair_t::sequence_n, vector<int>(sequence_pair_t::sequence_n, 0));
    vector<vector<int>> fix_constraint_h(sequence_pair_t::sequence_n, vector<int>(sequence_pair_t::sequence_n, 0));
    for(int i = sequence_pair_t::fix_start_idx; i<sequence_pair_t::sequence_n; ++i) {
        for (int j = sequence_pair_t::fix_start_idx; j < sequence_pair_t::sequence_n; ++j) {
            if (i == j) { continue; }
            int ix = this->seq_fixed_map[i]->get_left_lower().get_x();
            int iy = this->seq_fixed_map[i]->get_left_lower().get_y();
            int jx = this->seq_fixed_map[j]->get_left_lower().get_x();
            int jy = this->seq_fixed_map[j]->get_left_lower().get_y();
            int iw = this->seq_fixed_map[i]->get_size().get_x();
            int ih = this->seq_fixed_map[i]->get_size().get_y();
            if (ix + iw <= jx) {
                fix_constraint_h[i][j] = 1;
            }
            if (iy + ih <= jy) {
                fix_constraint_v[i][j] = 1;
            }
        }
    }
    //now insert fix module to sequence
    vector<int> upd_v, upd_h, v_map(sequence_pair_t::sequence_n), h_map(sequence_pair_t::sequence_n);
    for(int i = sequence_pair_t::fix_start_idx; i<sequence_pair_t::sequence_n; ++i) {
        for(int j = 0; j<std::min(upd_v.size(),upd_h.size()); ++j){
            v_map[upd_v[j]] = h_map[upd_h[j]] = j;
        }
        bool inserted = false;
        for(int j = 0; j<=upd_v.size()&&!inserted; ++j){
            for(int k = 0; k<=upd_h.size()&&!inserted; ++k){
                bool ok = true;
                for(int p = sequence_pair_t::fix_start_idx; p<i; ++p){ // check all elements in squence
                    int p_id_v = v_map[p], p_id_h = h_map[p];
                    if(j<=p_id_v&&k<=p_id_h){ //V[i, p], H[i, p] then need i->p in horizontal constraints
                        if(fix_constraint_h[i][p]){continue;}
                        else{ok = false; break;}
                    }

                    if(j>p_id_v&&k>p_id_h){ //V[p, i], H[p, i] then need p->i in horizontal constraints
                        if(fix_constraint_h[p][i]){continue;}
                        else{ok = false; break;}
                    }

                    if(j<=p_id_v&&k>p_id_h){ //V[i, p], H[p, i] then need i->p in vertical constraints
                        if(fix_constraint_v[i][p]){continue;}
                        else{ok = false; break;}
                    }

                    if(j>p_id_v&&k<=p_id_h){ //V[p,i], H[i, p] then need p->i in vertical constraints
                        if(fix_constraint_v[p][i]){continue;}
                        else{ok = false; break;}
                    }
                }
                if(ok){
                    upd_v.insert(upd_v.begin()+j, i);
                    upd_h.insert(upd_h.begin()+k, i);
                    inserted = true;
                }
            }
        }
    }
    this->fix_sequence_v = upd_v;
    this->fix_sequence_h = upd_h;
}
void sequence_pair_t::set_only_fix() {
    this->fix_sequence_v.clear(); this->fix_sequence_h.clear();
    this->v_sequence.clear(); this->h_sequence.clear();
    this->fix_sequence_v.resize(fix_n);
    this->fix_sequence_h.resize(fix_n);

    this->set_fix_sequence();

    this->v_sequence = fix_sequence_v;
    this->h_sequence = fix_sequence_h;
    for(int i = sequence_pair_t::fix_start_idx; i<sequence_pair_t::sequence_n; ++i){
        this->is_in_seq[i] = 1;
    }
}

void sequence_pair_t::init_modules_size() {
    this->modules_wh.resize(sequence_pair_t::sequence_n);
    //this->modules_wh_i.resize(sequence_pair_t::sequence_n);
    this->modules_wh_i.resize(sequence_pair_t::soft_n);
    //determine the shape of the modules
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        if(sequence_pair_t::seq_is_fix[i]){
            this->modules_wh[i] = seq_fixed_map[i]->get_size();
            //this->modules_wh_i[i] = -1;
        }
        else{
            //find all legal shapes and choose one randomly
            this->modules_wh[i] = soft_area_to_w_h_m_5[i][0];
            this->modules_wh_i[i] =0;
        }
    }
}

void sequence_pair_t::set_is_in_seq() {
    is_in_seq.resize(sequence_pair_t::sequence_n);
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        is_in_seq[i] = 0;
    }
}

void sequence_pair_t::print_fix_sequence() {
    cout<<"v: [";
    for(int i = 0; i<this->fix_sequence_v.size(); ++i){
        cout<< this->fix_sequence_v[i];
        if(i!=this->fix_sequence_v.size()-1){cout<<", ";}
    }
    cout<<"] ";
    cout<<"h: [";
    for(int i = 0; i<this->fix_sequence_h.size(); ++i){
        cout<< this->fix_sequence_h[i];
        if(i!=this->fix_sequence_h.size()-1){cout<<", ";}
    }

    cout<<"]"<<endl;
}

void sequence_pair_t::print_connections() {
    for(auto& edge:this->connections){
        cout<<"net : "<<endl;
        for(auto& e:edge.nodes){
            cout<<e<<" ";
        }
        cout<<endl;
    }
}

bool sequence_pair_t::is_completed() {
    //first, check all soft modules are in the sequence
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        if(this->is_in_seq[i]==0){return false;}
    }
    //then find the position of each module
    bool success = this->find_position_with_area(false,false,0, 0);
    //bool success = this->find_position(false,false,0, 0);
    if(success){return true;}
    return false;
}

void sequence_pair_t::predict_wirelength(bool minimize_wirelength, bool with_area) {
    bool success;
    if(with_area){
        success = this->find_position_with_area(minimize_wirelength,true,0, 0); //need result to calculate wirelength
    }
    else{
        success = this->find_position(minimize_wirelength,true,0, 0); //need result to calculate wirelength
    }
    if(success==false){
        this->predicted_wirelength = -1;
        return;
    }
    vector<vec2d_t> pos = this->modules_positions;
    double sum = 0;
    for(int i = 0; i<connections.size();++i){
        double x_min = 1e9, y_min = 1e9, x_max = -1, y_max = -1;
        for(int j = 0; j<connections[i].nodes.size(); ++j){
            int v = connections[i].nodes[j];
            vec2d_t center_v = {pos[v].get_x()+this->modules_wh[v].get_half_x(), pos[v].get_y()+this->modules_wh[v].get_half_y()}; 
            double v_x = center_v.get_x(), v_y = center_v.get_y();
            x_min = std::min(x_min, v_x);
            x_max = std::max(x_max, v_x);
            y_min = std::min(y_min, v_y);
            y_max = std::max(y_max, v_y);
        }
        double delta_x = x_max-x_min;
        double delta_y = y_max-y_min;
        sum+= (delta_x+delta_y)*connections[i].w;
    }
    this->predicted_wirelength = sum;
}

floorplan_t sequence_pair_t::to_fp() {
    floorplan_t ret;
    int placed_n = 0;
//    if(this->is_completed()==false){
//        return ret;
//    }
    for(int i = 0; i<soft_n; ++i){
        bool ok = ret.place_soft_module(i, {
                                                vec2d_t(static_cast<int32_t>(this->modules_positions[i].get_x()),static_cast<int32_t>(this->modules_positions[i].get_y()))},
                                        {vec2d_t(static_cast<int32_t>(this->modules_wh[i].get_x()),static_cast<int32_t>(this->modules_wh[i].get_y()))});
        if(ok){placed_n++;}
    }
    cout<<"placed "<<placed_n<<" modules"<<endl;
    if(placed_n==chip_t::get_soft_modules().size()){
        cout<< "Done!"<<endl;
    }
    return ret;
}



void sequence_pair_t::set_add_order() {
    this->add_soft_order.resize(sequence_pair_t::soft_n);
    vector<double> areas = this->modules_area;
    for(int i = 0; i<sequence_pair_t::soft_n; ++i){
        this->add_soft_order[i] = i;
    }
    std::sort(this->add_soft_order.begin(), this->add_soft_order.end(), [&](int&a, int& b){
        return this->modules_area[a] > this->modules_area[b];
    });
}

void sequence_pair_t::print_logs() {
    for(auto& log:this->logs){
        cout<<"After "<<log.first<<"s, got wire length"<<log.second<<endl;
    }
}

bool sequence_pair_t::find_position_with_area(bool minimize_wirelength, bool load_result,int overlap_h, int overlap_v) {
    build_constraint_graph();
    constraint_n = this->constraint_graph_h.size() + this->constraint_graph_v.size() + chip_t::get_fixed_modules().size()*2;
    constraint_i = 1; //constraint_counter
    variable_n = 2*sequence_n;
    if(minimize_wirelength){
        variable_n+=4*sequence_pair_t::connections.size();
        constraint_n+=(2+ 4 * sequence_pair_t::connection_graph_deg)*sequence_pair_t::connections.size();
    }
    x_module_offset = 1;
    y_module_offset = 1+sequence_n;
    x_edge_offset_l = 1+2*sequence_n;
    x_edge_offset_r = 1+2*sequence_n+this->connections.size();
    y_edge_offset_l = 1+2*sequence_n+2*this->connections.size();
    y_edge_offset_r = 1+2*sequence_n+3*this->connections.size();
    //apart from constraint graph, every fix module need 2 additional condition
    ILP_solver = ILP_solver_t();
    ILP_solver.init("ILP_solver", constraint_n, variable_n);
    if(ILP_solver.get_is_invalid()){return false;}
    ILP_solver.set_min();


    vector<double> coef(variable_n+1, 0);

    //set constraints to place modules
    //if horizontal constraint graph got i->j them x_j - x_i >= w
    for(int i = 0; i<this->constraint_graph_h.size(); ++i){
        int from = constraint_graph_h[i].from, to = constraint_graph_h[i].to, w = constraint_graph_h[i].w;
        string constraint_name = "h_c"+ std::to_string(i);
        ILP_solver.set_constraint_upb(constraint_i, 2, {from+1, to+1}, {1, -1}, constraint_name, -w+overlap_h);
        constraint_i++;
    }
    //if vertical constraint graph got i->j them y_j - y_i >= h
    for(int i = 0; i<this->constraint_graph_v.size(); ++i){
        int from = constraint_graph_v[i].from, to = constraint_graph_v[i].to, w = constraint_graph_v[i].w;
        string constraint_name = "v_c"+ std::to_string(i);
        ILP_solver.set_constraint_upb(constraint_i, 2, {from+1+sequence_n, to+1+sequence_n}, {1, -1}, constraint_name, -w+overlap_v);
        constraint_i++;
    }
    //set constraints to fix module
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        if(sequence_pair_t::seq_is_fix[i]){
            string x_constraint_name = "fix_x_c"+ std::to_string(i);
            string y_constraint_name = "fix_y_c"+ std::to_string(i);
            vec2d_t ll_pos = sequence_pair_t::seq_fixed_map[i]->get_left_lower();
            ILP_solver.set_constraint_fx(constraint_i, 1, {i+1}, {1}, x_constraint_name, static_cast<int>(ll_pos.get_x()));
            constraint_i++;
            ILP_solver.set_constraint_fx(constraint_i, 1, {i+1+sequence_n}, {1}, y_constraint_name, static_cast<int>(ll_pos.get_y()));
            constraint_i++;
        }
    }
    //set constraints to give the direction to the edges
    for(int i = 0; i<this->connections.size()&&minimize_wirelength; ++i){
        string x_constraint_name = "x_r_e"+ std::to_string(i);
        string y_constraint_name = "y_r_e"+ std::to_string(i);
        ILP_solver.set_constraint_upb(constraint_i, 2, {x_edge_offset_l+i, x_edge_offset_r+i}, {1, -1}, x_constraint_name, 0.0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {y_edge_offset_l+i, y_edge_offset_r+i}, {1, -1}, y_constraint_name, 0.0);
        constraint_i++;
    }
    //set edge and block left(bottom) constraints
    for(int i = 0; i<this->connections.size()&&minimize_wirelength; ++i){
        for(int j = 0; j<connections[i].nodes.size(); ++j){
            int b = connections[i].nodes[j];
            double b_x = this->modules_wh[b].get_half_x();
            double b_y = this->modules_wh[b].get_half_y();    
            string x_l_constraint_name = "x_e_b_l"+ std::to_string(i)+"_"+std::to_string(j);
            string y_l_constraint_name = "y_e_b_l"+ std::to_string(i)+"_"+std::to_string(j);
            string x_r_constraint_name = "x_e_b_r"+ std::to_string(i)+"_"+std::to_string(j);
            string y_r_constraint_name = "y_e_b_r"+ std::to_string(i)+"_"+std::to_string(j);
            ILP_solver.set_constraint_upb(constraint_i, 2, {x_edge_offset_l+i,b+x_module_offset}, {1, -1}, x_l_constraint_name, b_x);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {y_edge_offset_l+i,b+y_module_offset}, {1, -1}, y_l_constraint_name, b_y);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {b+x_module_offset,x_edge_offset_r+i}, {1, -1}, x_r_constraint_name, -b_x);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {b+y_module_offset,y_edge_offset_r+i}, {1, -1}, y_r_constraint_name, -b_y);
            constraint_i++;
        }
    }

    //set variables
    for(int i = 1;i<=sequence_n; ++i){
        string var_name = "x"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, i, var_name.c_str());
        //ILP_solver.set_variable_double_range(i, 0.0,chip_t::get_width()-this->modules_wh[i-1].get_x());
        ILP_solver.set_variable_double_range(i, 0.0,chip_t::get_width()-this->modules_wh[i-1].get_x());

    }
    for(int i = sequence_pair_t::sequence_n+1;i<=2*sequence_n; ++i){
        string var_name = "x"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, i, var_name.c_str());
        //ILP_solver.set_variable_double_range(i, 0.0, chip_t::get_height()-this->modules_wh[ i-1-sequence_n].get_y());
        ILP_solver.set_variable_double_range(i, 0.0, chip_t::get_height()-this->modules_wh[ i-1-sequence_n].get_y());
    }
    for(int i = 0; i<connections.size()&&minimize_wirelength; ++i){
        string var_name1 = "x_e_l"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, x_edge_offset_l+i, var_name1.c_str());
        ILP_solver.set_variable_double_range(x_edge_offset_l+i, 0.0, chip_t::get_width());

        string var_name2 = "x_e_r"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, x_edge_offset_r+i, var_name2.c_str());
        ILP_solver.set_variable_double_range(x_edge_offset_r+i, 0.0, chip_t::get_width());

        string var_name3 = "y_e_l"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, y_edge_offset_l+i, var_name3.c_str());
        ILP_solver.set_variable_double_range(y_edge_offset_l+i, 0.0, chip_t::get_height());

        string var_name4 = "y_e_r"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, y_edge_offset_r+i, var_name4.c_str());
        ILP_solver.set_variable_double_range(y_edge_offset_r+i, 0.0, chip_t::get_height());
    }
    //set coefficient of the objective function
    for(int i = 0; i<connections.size()&&minimize_wirelength; ++i){
        coef[i+x_edge_offset_r] = connections[i].w;
        coef[i+x_edge_offset_l] = -connections[i].w;
        coef[i+y_edge_offset_r] = connections[i].w;
        coef[i+y_edge_offset_l] = -connections[i].w;
    }
    //solve
    ILP_solver.set_obj_coef(coef);
    ILP_solver.load();
    ILP_result_t ILP_result = ILP_solver.solve(false);
    vector<vec2d_t> result; //zero-index
    //cout<< "got z : "<<ILP_result.z<<endl;
    for(int i = 1; i<=sequence_n; ++i){
        if(this->is_in_seq[i-1]==0){
            result.push_back({-1, -1});
        }
        else {
            result.push_back( {ILP_result.var_values[i], ILP_result.var_values[i+sequence_n]});
        }

    }
    if(load_result){
        this->modules_positions = result;
    }
    if(ILP_result.legal){
        return true;
    }
    else{return false;}


}

void sequence_pair_t::swap_seq_number(int a, int b,bool h,bool v) {
    vector<int> v_map(sequence_pair_t::sequence_n), h_map(sequence_pair_t::sequence_n);
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        v_map[this->v_sequence[i]] = h_map[this->h_sequence[i]] = i;
    }
    if(v){
        std::swap(this->v_sequence[v_map[a]], this->v_sequence[v_map[b]]);
    }
    if(h){
        std::swap(this->h_sequence[h_map[a]], this->h_sequence[h_map[b]]);
    }

}

double sequence_pair_t::get_wirelength(bool minimize, bool with_area) {
    this->predict_wirelength(minimize, with_area);
    return this->predicted_wirelength;
}

void sequence_pair_t::print_wirelength(bool minimize, bool with_area) {
    cout<<std::setprecision(16)<<this->get_wirelength(minimize,with_area)<<endl;
}

void sequence_pair_t::print_v() {
    cout<<"v: {";
    for(int i = 0; i<this->v_sequence.size(); ++i){
        cout<< v_sequence[i];
        if(i!=this->v_sequence.size()-1){cout<<", ";}
    }
    cout<<"} "<<endl;
}
void sequence_pair_t::print_h(){
    cout<<"h: {";
    for(int i = 0; i<this->h_sequence.size(); ++i){
        cout<< h_sequence[i];
        if(i!=this->h_sequence.size()-1){cout<<", ";}
    }
    cout<<"}"<<endl;
}

void sequence_pair_t::write_inline() {
    std::fstream fout;
    fout.open("../../output_SP.txt", std::ios::out | std::ios::trunc);
    fout<<"v: {";
    for(int i = 0; i<this->v_sequence.size(); ++i){
        fout<< v_sequence[i];
        if(i!=this->v_sequence.size()-1){fout<<", ";}
    }
    fout<<"} "<<endl;
    fout<<"h: {";
    for(int i = 0; i<this->h_sequence.size(); ++i){
        fout<< h_sequence[i];
        if(i!=this->h_sequence.size()-1){fout<<", ";}
    }
    fout<<"}"<<endl;
    //this->print_shapes_i();
    fout<<"wirelength : "<<std::setprecision(16)<<this->get_wirelength(true, true)<<endl;
    fout.close();
}

void sequence_pair_t::simplify_constraint_graph() {
    int N = sequence_pair_t::sequence_n;
    {
        vector<vector<int>> G(N, vector<int>(N, 0));
        vector<edge_t> simplified;
        for(auto& e:this->constraint_graph_v){
            G[e.from][e.to] = 1;
        }
        for(int i = 0; i<N; ++i){
            for(int j = 0; j<N; ++j){
                for(int k = 0; k<N; ++k){
                    if(G[i][k]&&G[k][j]){
                        G[i][j] = 0;
                    }
                }
            }
        }
        for(auto& e: this->constraint_graph_v){
            if(G[e.from][e.to]){
                simplified.push_back(e);
            }
        }
        this->constraint_graph_v = simplified;
    }
    {
        vector<vector<int>> G(N, vector<int>(N, 0));
        vector<edge_t> simplified;
        for(auto& e:this->constraint_graph_h){
            G[e.from][e.to] = 1;
        }
        for(int i = 0; i<N; ++i){
            for(int j = 0; j<N; ++j){
                for(int k = 0; k<N; ++k){
                    if(G[i][k]&&G[k][j]){
                        G[i][j] = 0;
                    }
                }
            }
        }
        for(auto& e: this->constraint_graph_h){
            if(G[e.from][e.to]){
                simplified.push_back(e);
            }
        }
        this->constraint_graph_h = simplified;
    }
}


