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
#include<iomanip>
int sequence_pair_t::sequence_n;
int sequence_pair_t::fix_start_idx;
int sequence_pair_t::fix_n;
int sequence_pair_t::soft_n;
vector<std::vector<vec2d_t>> sequence_pair_t::soft_area_to_w_h_m;
vector<bool> sequence_pair_t::seq_is_fix;
vector<soft_module_t*> sequence_pair_t::seq_soft_map;
vector<fixed_module_t*> sequence_pair_t::seq_fixed_map;
unordered_map<const module_t*, int> sequence_pair_t::soft_module_to_id_m;
unordered_map<const module_t*, int> sequence_pair_t::fix_module_to_id_m;
vector<edge_t> sequence_pair_t::connections;
vector<vector<int>> sequence_pair_t::connections_VE;
vector<int> sequence_pair_t::deg_w;
vector<double> sequence_pair_t::modules_area;
using std::cout;
using std::endl;
using std::string;
void sequence_pair_t::init() {
    srand(time(NULL));

    const std::vector<soft_module_t*>& soft_modules = chip_t::get_soft_modules();
    const std::vector<fixed_module_t*>& fixed_modules = chip_t::get_fixed_modules();
    sequence_pair_t::fix_n =  static_cast<int>(chip_t::get_fixed_modules().size());
    sequence_pair_t::soft_n = static_cast<int>(chip_t::get_soft_modules().size());
    sequence_pair_t::sequence_n = static_cast<int>(chip_t::get_total_module_n());
    sequence_pair_t::fix_start_idx = static_cast<int>(chip_t::get_soft_modules().size());

    soft_area_to_w_h_m.resize(soft_modules.size());
    sequence_pair_t::seq_soft_map.resize(chip_t::get_total_module_n());
    sequence_pair_t::seq_fixed_map.resize(chip_t::get_total_module_n());
    sequence_pair_t::seq_is_fix.resize(chip_t::get_total_module_n());
    sequence_pair_t::modules_area.resize(chip_t::get_total_module_n());

    connections_VE.resize(sequence_n);
    deg_w.resize(sequence_n);
    for(int i = 0; i<sequence_n; ++i){connections_VE[i].resize(sequence_n);}


    //build up the map between module and sequence number
    int j = 0;
    for(int i = 0; i<soft_modules.size(); ++i){
        sequence_pair_t::seq_soft_map[j] = soft_modules[i];
        sequence_pair_t::soft_area_to_w_h_m[i] = sequence_pair_t::find_w_h( soft_modules[i]->get_area());
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

vector<vec2d_t> sequence_pair_t::find_w_h(uint32_t area) {
    //select only 5 shapes
    if(area==1){return {{1,1}};}
    double p = sqrt(static_cast<double>(area)/1.95);
    double b = pow(1.95, 0.25);
    vector<vec2d_t> res;

    for(int i = 0; i<=4; ++i){ //i will be the short edge
        //cout<< area<<" "<<i<<" "<<ceil(static_cast<float>(area)/static_cast<float>(i))<<endl;
        double x = p*pow(b, i);
        double y = area/x;

        if(ceil(x)*2>=ceil(y)){
            double xx = ceil(x), yy = ceil(y);
            if(xx-1>=yy *0.5&& (xx-1)*yy>=area){
                xx-=1;
            }
            if(yy-1>=0.5*xx && (yy-1)*xx>=area){
                yy-=1;
            }
            double diff = xx*yy-area;
//            cout<<"Area : "<<area<<", "<<"diff = "<<diff<<" "<<"ratio : "<<xx/yy<<endl;
//            cout<<"x : "<<xx<<"y : "<<yy<<endl;
            res.push_back({xx, yy});
        }
    }
    return res;
////TODO: select only 5 shapes
//    if(area==1){return {{1,1}};}
//    double p = sqrt(static_cast<double>(area)*0.5);
//    double b = pow(2, 0.25);
//    vector<vec2d_t> res;
//
//    for(int i = 0; i<=4; ++i){ //i will be the short edge
//
//        double x = floor(p*pow(b, i));
//        double y = ceil(area/x);
//        if(y/x>2){x = ceil(p*pow(b, i));}
//        if(y/x<0.5){y = floor(area/x);}
//        double diff = x*y-area;
//        if(diff<0||x/y>2||x/y<0.5){
//            cout<<"Area : "<<area<<", "<<"diff = "<<diff<<" "<<"ratio : "<<x/y<<endl;
//            int a = 5;
//        }
//
//        res.push_back({x, y});
//    }
//    return res;
}


void sequence_pair_t::build_constraint_graph() {
    vector<edge_t> updated_constraint_graph_h, updated_constraint_graph_v;
    vector<int> v_map(sequence_pair_t::sequence_n),h_map(sequence_pair_t::sequence_n);
    for(int i = 0; i<v_sequence.size(); ++i){
        v_map[v_sequence[i]] = i;
        h_map[h_sequence[i]] = i;
    }
    for(int i = 0; i<sequence_n; ++i){
        for(int j = 0; j<sequence_n; ++j) {
            if(is_in_seq[i]==0||is_in_seq[j]==0){continue;}
            if(v_map[i]<v_map[j] && h_map[i]<h_map[j]){ //{a, b}, {a, b}
                updated_constraint_graph_h.push_back({i, j, static_cast<int>(this->modules_wh[i].get_x())});
            }
            else if(v_map[i]<v_map[j] && h_map[j]<h_map[i]) { //{i, j}, {j,i}
                updated_constraint_graph_v.push_back({i, j, static_cast<int>(this->modules_wh[i].get_y())});
            }
        }
    }
    this->constraint_graph_h = updated_constraint_graph_h;
    this->constraint_graph_v = updated_constraint_graph_v;
//    cout<<"horizontal constraint graph : "<<endl;
//    for(auto& e:constraint_graph_h){
//        cout<< "{"<<e.from<<"->"<<e.to<<", "<<e.w<<"}"<<endl;
//    }
//    cout<<"vertical constraint graph : "<<endl;
//    for(auto& e:constraint_graph_v){
//        cout<< "{"<<e.from<<"->"<<e.to<<", "<<e.w<<"}"<<endl;
//    }
}

bool sequence_pair_t::find_position(bool minimize_wirelength, bool load_result,int overlap_h, int overlap_v) {
    build_constraint_graph();
    int constraint_n = this->constraint_graph_h.size() + this->constraint_graph_v.size() + chip_t::get_fixed_modules().size()*2 + 5*soft_n;
    int constraint_i = 1; //constraint_counter
    int variable_n = 2*sequence_n + 5*soft_n;
    if(minimize_wirelength){
        variable_n+=4*sequence_pair_t::connections.size();
        constraint_n+=10*sequence_pair_t::connections.size();
    }
    int x_module_offset = 1;
    int y_module_offset = 1+sequence_n;
    int shape_type_1_offset = 1 + 2 * sequence_n;
    int shape_type_2_offset = 1 + 2 * sequence_n + soft_n;
    int shape_type_3_offset = 1 + 2 * sequence_n + 2*soft_n;
    int shape_type_4_offset = 1 + 2 * sequence_n + 3*soft_n;
    int shape_type_5_offset = 1 + 2 * sequence_n + 4*soft_n;
    int x_edge_offset_l = 1+2*sequence_n+5*soft_n;
    int x_edge_offset_r = 1+2*sequence_n+5*soft_n+this->connections.size();
    int y_edge_offset_l = 1+2*sequence_n+5*soft_n+2*this->connections.size();
    int y_edge_offset_r = 1+2*sequence_n+5*soft_n+3*this->connections.size();



    //apart from constraint graph, every fix module need 2 additional condition
    ILP_solver_t ILP_solver;
    ILP_solver.init("ILP_solver", constraint_n, variable_n);
    if(ILP_solver.get_is_invalid()){return false;}
    ILP_solver.set_min();


    vector<int> coef(variable_n+1, 0);

    //set constraints to place modules
    //if horizontal constraint graph got i->j them x_j - x_i >= w
    for(int i = 0; i<this->constraint_graph_h.size(); ++i){
        int from = constraint_graph_h[i].from, to = constraint_graph_h[i].to,w = constraint_graph_h[i].w;
        string constraint_name = "h_c"+ std::to_string(i);

        if(this->seq_is_fix[from]){
            ILP_solver.set_constraint_upb(constraint_i, 2, {from+x_module_offset, to+x_module_offset}, {1, -1}, constraint_name, -w);
        }
        else{
            double area = this->modules_area[from];
            int w1 = sequence_pair_t::soft_area_to_w_h_m[from][0].get_x();
            int w2 = sequence_pair_t::soft_area_to_w_h_m[from][1].get_x();
            int w3 = sequence_pair_t::soft_area_to_w_h_m[from][2].get_x();
            int w4 = sequence_pair_t::soft_area_to_w_h_m[from][3].get_x();
            int w5 = sequence_pair_t::soft_area_to_w_h_m[from][4].get_x();
            ILP_solver.set_constraint_upb(constraint_i, 7,
                                  {from+x_module_offset, to+x_module_offset,
                                           shape_type_1_offset + from,
                                           shape_type_2_offset + from,
                                           shape_type_3_offset + from,
                                           shape_type_4_offset + from,
                                           shape_type_5_offset + from},
                                      {1, -1, w1,w2 ,w3,w4, w5}, constraint_name, -1e-5);
        }
        constraint_i++;
    }
    //if vertical constraint graph got i->j them y_j - y_i >= h
    for(int i = 0; i<this->constraint_graph_v.size(); ++i){
        int from = constraint_graph_v[i].from, to = constraint_graph_v[i].to, w = constraint_graph_v[i].w;
        string constraint_name = "v_c"+ std::to_string(i);

        if(this->seq_is_fix[from]){
            ILP_solver.set_constraint_upb(constraint_i, 2, {from+y_module_offset, to+y_module_offset}, {1, -1}, constraint_name, -w);
        }
        else{
            double area = this->modules_area[from];
            int h1 = static_cast<int>(sequence_pair_t::soft_area_to_w_h_m[from][0].get_y());
            int h2 = static_cast<int>(sequence_pair_t::soft_area_to_w_h_m[from][1].get_y());
            int h3 = static_cast<int>(sequence_pair_t::soft_area_to_w_h_m[from][2].get_y());
            int h4 = static_cast<int>(sequence_pair_t::soft_area_to_w_h_m[from][3].get_y());
            int h5 = static_cast<int>(sequence_pair_t::soft_area_to_w_h_m[from][4].get_y());
            ILP_solver.set_constraint_upb(constraint_i, 7,
                              {from+y_module_offset, to+y_module_offset,
                                       shape_type_1_offset + from,
                                       shape_type_2_offset + from,
                                       shape_type_3_offset + from,
                                       shape_type_4_offset + from,
                                       shape_type_5_offset + from},
                               {1, -1,h1,h2,h3,h4, h5}, constraint_name, -1e-5);
        }
        constraint_i++;
    }
    //set constraints to fix module
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        if(sequence_pair_t::seq_is_fix[i]){
            string x_constraint_name = "fix_x_c"+ std::to_string(i);
            string y_constraint_name = "fix_y_c"+ std::to_string(i);
            vec2d_t ll_pos = sequence_pair_t::seq_fixed_map[i]->get_left_lower();
            ILP_solver.set_constraint_fx(constraint_i, 1, {i+x_module_offset}, {1}, x_constraint_name, static_cast<int>(ll_pos.get_x()));
            constraint_i++;
            ILP_solver.set_constraint_fx(constraint_i, 1, {i+y_module_offset}, {1}, y_constraint_name, static_cast<int>(ll_pos.get_y()));
            constraint_i++;
        }
    }
    //set constraints to give the direction to the edges
    //x_e_r >= x_e_l
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
        int b1 = connections[i].from;
        int b2 = connections[i].to;
        if(this->is_in_seq[b1]==0||this->is_in_seq[b2]==0){
            continue;
        }
        string x_l_1_constraint_name = "x_e_1_b_l"+ std::to_string(i);
        string y_l_1_constraint_name = "y_e_1_b_l"+ std::to_string(i);
        string x_l_2_constraint_name = "x_e_2_b_l"+ std::to_string(i);
        string y_l_2_constraint_name = "y_e_2_b_l"+ std::to_string(i);
        ILP_solver.set_constraint_upb(constraint_i, 2, {x_edge_offset_l + i, b1 + x_module_offset}, {1, -1},
                                      x_l_1_constraint_name, 0.0);
        constraint_i++;

        ILP_solver.set_constraint_upb(constraint_i, 2, {x_edge_offset_l + i, b2 + x_module_offset}, {1, -1},
                                      x_l_2_constraint_name, 0.0);
        constraint_i++;

        ILP_solver.set_constraint_upb(constraint_i, 2, {y_edge_offset_l + i, b1 + y_module_offset}, {1, -1},
                                      y_l_1_constraint_name, 0.0);
        constraint_i++;

        ILP_solver.set_constraint_upb(constraint_i, 2, {y_edge_offset_l + i, b2 + y_module_offset}, {1, -1},
                                      y_l_2_constraint_name, 0.0);
        constraint_i++;
    }
    //set edge and block right(top) constraints
    for(int i = 0; i<this->connections.size()&&minimize_wirelength; ++i){
        int b1 = connections[i].from;
        int b2 = connections[i].to;
        if(this->is_in_seq[b1]==0||this->is_in_seq[b2]==0){
            continue;
        }
        string x_r_1_constraint_name = "x_e_1_b_r"+ std::to_string(i);
        string y_r_1_constraint_name = "y_e_1_b_r"+ std::to_string(i);
        string x_r_2_constraint_name = "x_e_2_b_r"+ std::to_string(i);
        string y_r_2_constraint_name = "y_e_2_b_r"+ std::to_string(i);

        ILP_solver.set_constraint_upb(constraint_i, 2, {b1+x_module_offset,x_edge_offset_r+i}, {1, -1}, x_r_1_constraint_name, 0.0);
        constraint_i++;

        ILP_solver.set_constraint_upb(constraint_i, 2, {b2+x_module_offset,x_edge_offset_r+i}, {1, -1}, x_r_2_constraint_name, 0.0);
        constraint_i++;

        ILP_solver.set_constraint_upb(constraint_i, 2, {b1+y_module_offset,y_edge_offset_r+i}, {1, -1}, y_r_1_constraint_name, 0.0);
        constraint_i++;

        ILP_solver.set_constraint_upb(constraint_i, 2, {b2+y_module_offset,y_edge_offset_r+i}, {1, -1}, y_r_2_constraint_name, 0.0);
        constraint_i++;
    }
    //set constraints to keep the ratio 0.5 <= wh <=  2
//    for(int i = 0; i<soft_n; ++i){
//        string constraint_name1 = "w_h_1"+ std::to_string(i);
//        string constraint_name2 = "w_h_2"+ std::to_string(i);
//        ILP_solver.set_constraint_upb(constraint_i, 2, {shape_type_2_offset+i,shape_type_1_offset+i}, {1, -2}, constraint_name1, 0.0);
//        constraint_i++;
//
//        ILP_solver.set_constraint_upb(constraint_i, 2, {shape_type_1_offset+i,shape_type_2_offset+i}, {1, -2}, constraint_name2, 0.0);
//        constraint_i++;
//    }
     //w+h = 1
    for(int i = 0; i<soft_n; ++i){
        string constraint_name1 = "w_h_11"+ std::to_string(i);
        ILP_solver.set_constraint_fx(constraint_i, 5,
        {shape_type_1_offset + i, shape_type_2_offset + i,shape_type_3_offset + i,shape_type_4_offset+i, shape_type_5_offset+i}, {1, 1,1,1,1}, constraint_name1, 1.001);
        constraint_i++;
    }
    //set boundaires
    for(int i = 0; i<soft_n; ++i){

        string  constraint_name1 = "x_bound"+ std::to_string(i);
        string  constraint_name2 = "y_bound"+ std::to_string(i);
        int w1 = sequence_pair_t::soft_area_to_w_h_m[i][0].get_x();
        int w2 = sequence_pair_t::soft_area_to_w_h_m[i][1].get_x();
        int w3 = sequence_pair_t::soft_area_to_w_h_m[i][2].get_x();
        int w4 = sequence_pair_t::soft_area_to_w_h_m[i][3].get_x();
        int w5 = sequence_pair_t::soft_area_to_w_h_m[i][4].get_x();

        int h1 = sequence_pair_t::soft_area_to_w_h_m[i][0].get_y();
        int h2 = sequence_pair_t::soft_area_to_w_h_m[i][1].get_y();
        int h3 = sequence_pair_t::soft_area_to_w_h_m[i][2].get_y();
        int h4 = sequence_pair_t::soft_area_to_w_h_m[i][3].get_y();
        int h5 = sequence_pair_t::soft_area_to_w_h_m[i][4].get_y();

        ILP_solver.set_constraint_upb(constraint_i, 6,
{x_module_offset+i, shape_type_1_offset + i, shape_type_2_offset + i,  shape_type_3_offset + i,shape_type_4_offset + i,shape_type_5_offset + i},
{1, w1, w2,w3,w4,w5}, constraint_name1, chip_t::get_width());

        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 6,
        {y_module_offset+i, shape_type_1_offset + i,
             shape_type_2_offset + i, shape_type_3_offset + i,
             shape_type_4_offset + i,shape_type_5_offset + i},
{1, h1, h2,h3,h4,h5}, constraint_name2, chip_t::get_height());

        constraint_i++;
    }
    //set variables
    for(int i = 1;i<=sequence_n; ++i){
        string var_name = "x"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, i, var_name.c_str());
        ILP_solver.set_variable_double_range(i, 0.0,chip_t::get_width());
        //ILP_solver.set_variable_double_range(i, 0.0,chip_t::get_width()-this->modules_wh[i-1].get_x());

    }
    for(int i = sequence_pair_t::sequence_n+1;i<=2*sequence_n; ++i){
        string var_name = "x"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, i, var_name.c_str());
        ILP_solver.set_variable_double_range(i, 0.0, chip_t::get_height());
        //ILP_solver.set_variable_double_range(i, 0.0, chip_t::get_height()-this->modules_wh[ i-1-sequence_n].get_y());
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
    for(int i = 0; i<soft_n; ++i){
        string var_name1 = "shape1"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, shape_type_1_offset + i, var_name1.c_str());
        ILP_solver.set_variable_double_range(shape_type_1_offset + i, 0, 1);
        //ILP_solver.set_variable_double_range_int(shape_type_1_offset + i, 0, 1);

        string var_name2 = "shape2"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, shape_type_2_offset + i, var_name2.c_str());
        ILP_solver.set_variable_double_range(shape_type_2_offset + i, 0, 1);
        //ILP_solver.set_variable_double_range_int(shape_type_2_offset + i, 0, 1);

        string var_name3 = "shape3"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, shape_type_3_offset + i, var_name3.c_str());
        ILP_solver.set_variable_double_range(shape_type_3_offset + i, 0, 1);
        //ILP_solver.set_variable_double_range_int(shape_type_3_offset + i, 0, 1);

        string var_name4 = "shape4"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, shape_type_4_offset + i, var_name4.c_str());
        ILP_solver.set_variable_double_range(shape_type_4_offset + i, 0, 1);
        //ILP_solver.set_variable_double_range_int(shape_type_4_offset + i, 0, 1);

        string var_name5 = "shape5"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, shape_type_5_offset + i, var_name5.c_str());
        ILP_solver.set_variable_double_range(shape_type_5_offset + i, 0, 1);
        //ILP_solver.set_variable_double_range_int(shape_type_5_offset + i, 0, 1);


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


    if(ILP_result.legal){
        if(load_result){
//            cout<<"value : ";
//            for(auto& e:ILP_result.var_values){
//                cout<<e<<" ";
//            }
//            cout<<endl;
            vector<vec2d_t> result_pos, result_wh;
            vector<int> result_wh_i; //zero-index
            for(int i = 0; i<sequence_n; ++i){
                if(this->is_in_seq[i]==0){
                    result_pos.push_back({-1, -1});
                }
                else {
                    result_pos.push_back({static_cast<int>(ILP_result.var_values[i+x_module_offset]),
                                          static_cast<int>(ILP_result.var_values[i + y_module_offset])});
                }
            }
            for(int i = 0; i<soft_n; ++i){
                if(this->is_in_seq[i]==0){
                    result_wh.push_back({-1, -1});
                    result_wh_i.push_back(-1);
                }
                else{
                    double shape1 = ILP_result.var_values[i + shape_type_1_offset];
                    double shape2 = ILP_result.var_values[i + shape_type_2_offset];
                    double shape3 = ILP_result.var_values[i + shape_type_3_offset];
                    double shape4 = ILP_result.var_values[i + shape_type_4_offset];
                    double shape5 = ILP_result.var_values[i + shape_type_5_offset];
                    double shapes[5]  = {shape1, shape2, shape3, shape4, shape5};
                    //cout<< "s : "<<shape1<<" "<<shape2<<" "<<shape3<<" "<<shape4<<" "<<shape5<<endl;
                    double x = 0, y = 0;
                    for(int j = 0; j<5; ++j){
                        x+=shapes[j]*sequence_pair_t::soft_area_to_w_h_m[i][j].get_x();
                        y+=shapes[j]*sequence_pair_t::soft_area_to_w_h_m[i][j].get_y();
                    }
//                    x = ceil(x);
//                    y = ceil(y);
                    x = floor(x);
                    y = floor(y);
                    double area = x*y;
                    double ratio = x/y;
                    if(area<sequence_pair_t::modules_area[i] || ratio<0.5 || ratio >2){
//                        cout<< "Area ::::"<<sequence_pair_t::modules_area[i]<<" "<<"x : "<<x<<" "<<"y : "<<y<<endl;
//                        cout<<"xy : "<<area<<" "<<"ratio : " <<ratio<<endl;
//                        int a = 5;
                    }


                    result_wh.push_back({x, y});
                    result_wh_i.push_back(-1);
                }
            }
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
    for(int i = 0; i<sequence_n; ++i) {
        if(seq_is_fix[i]){
            const vector<std::pair<const module_t* const, const int>> neighbors
                    = (seq_fixed_map[i])->getConnections();
            for(const auto neighbor:neighbors){
                int neighbor_i = -1;
                if(sequence_pair_t::fix_module_to_id_m.count(neighbor.first)){
                    neighbor_i = fix_module_to_id_m[neighbor.first];
                }
                if(sequence_pair_t::soft_module_to_id_m.count(neighbor.first)) {
                    neighbor_i = soft_module_to_id_m[neighbor.first];
                }
                connections_VE[neighbor_i][i] = connections_VE[i][neighbor_i] = neighbor.second;
                deg_w[i]+=neighbor.second;
                if(i>neighbor_i){
                    connections.push_back({i, neighbor_i, neighbor.second});
                }

            }
        }
        else{
            const vector<std::pair<const module_t* const, const int>> neighbors
                    = (seq_soft_map[i])->getConnections();
            for(const auto neighbor:neighbors){
                int neighbor_i = -1;
                if(sequence_pair_t::fix_module_to_id_m.count(neighbor.first)){
                    neighbor_i = fix_module_to_id_m[neighbor.first];
                }
                if(sequence_pair_t::soft_module_to_id_m.count(neighbor.first)) {
                    neighbor_i = soft_module_to_id_m[neighbor.first];
                }
                connections_VE[neighbor_i][i] = connections_VE[i][neighbor_i] = neighbor.second;
                deg_w[i]+=neighbor.second;
                if(i>neighbor_i){
                    connections.push_back({i, neighbor_i, neighbor.second});
                }
            }
        }
    }
}

pair<bool, floorplan_t> sequence_pair_t::place_all_modules(vector<vec2d_t> res) {

    floorplan_t fp;
    bool all_module_placed = true;
    for(int i = 0; i<res.size(); ++i){
        if(seq_is_fix[i]){ continue;}
        //bool success = fp.place_soft_module(i, res[i], modules_wh[i]);
        bool success = place_8d(fp,i, res[i], modules_wh[i], 0);
        if(success==false){
            all_module_placed = false;
            break;
        }
    }
    return {all_module_placed, fp};
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

bool sequence_pair_t::place_8d( floorplan_t& fp,int id,vec2d_t res, vec2d_t wh, int offset) {

    const int dir_x[8] = {0,0,1,-1,1,1,-1,-1};
    const int dir_y[8] = {1,-1,0,0,1,-1,1,-1};
    bool finish = false;
    for(int i = 0; i<=offset&&!finish; ++i){
        for(int j = 0; j<8 && !finish; ++j){
            for(int k = 0; k<8 && !finish; ++k){
                int nx = res.get_x()+dir_x[j]*i, ny = res.get_y()+dir_y[k]*i;
                if(nx<0||nx>=chip_t::get_width()||ny<0||ny>=chip_t::get_height()){break;}
                bool success = fp.place_soft_module(id, {nx, ny}, wh);
                if(success){finish = true;}
            }
        }
    }
    return finish;
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
//        for(auto& shape:sequence_pair_t::soft_area_to_w_h_m[i]){
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
        int id = static_cast<int>(rand())%sequence_pair_t::soft_area_to_w_h_m[i].size();
        this->modules_wh[i] = soft_area_to_w_h_m[i][id];
        this->modules_wh_i[i] = id;
    }
}
void sequence_pair_t::set_module_size(int i, int j){
    this->modules_wh[i] = sequence_pair_t::soft_area_to_w_h_m[i][j];
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
        for(int j = 0; j<upd_v.size(); ++j){
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
            vector<vec2d_t> shapes = find_w_h(seq_soft_map[i]->get_area());
            int id = 0;
            this->modules_wh[i] = shapes[id];
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
        std::cout<<edge.from<<" -> "<<edge.to<<", w = "<<edge.w<<std::endl;
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
        int from = connections[i].from, to = connections[i].to;
        double w = connections[i].w;
        double delta_x, delta_y;
        vec2d_t center_from = {pos[from].get_x()+this->modules_wh[from].get_half_x(), pos[from].get_y()+this->modules_wh[from].get_half_y()};
        vec2d_t center_to = {pos[to].get_x()+this->modules_wh[to].get_half_x(), pos[to].get_y()+this->modules_wh[to].get_half_y()};
        delta_x = fabs(center_from.get_x()-center_to.get_x());
        delta_y = fabs(center_from.get_y()-center_to.get_y());
        sum+= (delta_x+delta_y)*w;
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
        bool ok = ret.place_soft_module(i, {this->modules_positions[i]}, {this->modules_wh[i]});
        if(ok){placed_n++;}
    }
    cout<<"placed "<<placed_n<<" modules"<<endl;
    return ret;
}



void sequence_pair_t::set_add_order() {
    this->add_soft_order.resize(sequence_pair_t::soft_n);
    vector<double> areas = this->modules_area;
    for(int i = 0; i<sequence_pair_t::soft_n; ++i){
        this->add_soft_order[i] = i;
    }
//    for(int i = 0; i<sequence_pair_t::soft_n; ++i){
//        int x =rand()%soft_n;
//        std::swap(add_soft_order[i], add_soft_order[x]);
//    }
    std::sort(this->add_soft_order.begin(), this->add_soft_order.end(), [&](int&a, int& b){
        return this->modules_area[a] > this->modules_area[b];
        //return this->deg_w[a] > this->deg_w[b];
    });
}

void sequence_pair_t::print_logs() {
    for(auto& log:this->logs){
        cout<<"After "<<log.first<<"s, got wire length"<<log.second<<endl;
    }
}

bool sequence_pair_t::find_position_with_area(bool minimize_wirelength, bool load_result,int overlap_h, int overlap_v) {
    build_constraint_graph();
    int constraint_n = this->constraint_graph_h.size() + this->constraint_graph_v.size() + chip_t::get_fixed_modules().size()*2;
    int constraint_i = 1; //constraint_counter
    int variable_n = 2*sequence_n;
    if(minimize_wirelength){
        variable_n+=4*sequence_pair_t::connections.size();
        constraint_n+=10*sequence_pair_t::connections.size();
    }
    int x_module_offset = 1;
    int y_module_offset = 1+sequence_n;
    int x_edge_offset_l = 1+2*sequence_n;
    int x_edge_offset_r = 1+2*sequence_n+this->connections.size();
    int y_edge_offset_l = 1+2*sequence_n+2*this->connections.size();
    int y_edge_offset_r = 1+2*sequence_n+3*this->connections.size();
    //apart from constraint graph, every fix module need 2 additional condition
    ILP_solver_t ILP_solver;
    ILP_solver.init("ILP_solver", constraint_n, variable_n);
    if(ILP_solver.get_is_invalid()){return false;}
    ILP_solver.set_min();


    vector<int> coef(variable_n+1, 0);

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
        int b1 = connections[i].from;
        double b1_x = this->modules_wh[b1].get_half_x();
        double b1_y = this->modules_wh[b1].get_half_y();
        int b2 = connections[i].to;
        double b2_x = this->modules_wh[b2].get_half_x();
        double b2_y = this->modules_wh[b2].get_half_y();
        if(this->is_in_seq[b1]==0||this->is_in_seq[b2]==0){
            continue;
        }
        string x_l_1_constraint_name = "x_e_1_b_l"+ std::to_string(i);
        string y_l_1_constraint_name = "y_e_1_b_l"+ std::to_string(i);
        string x_l_2_constraint_name = "x_e_2_b_l"+ std::to_string(i);
        string y_l_2_constraint_name = "y_e_2_b_l"+ std::to_string(i);
        ILP_solver.set_constraint_upb(constraint_i, 2, {x_edge_offset_l+i,b1+x_module_offset}, {1, -1}, x_l_1_constraint_name, b1_x);
        constraint_i++;

        ILP_solver.set_constraint_upb(constraint_i, 2, {x_edge_offset_l+i,b2+x_module_offset}, {1, -1}, x_l_2_constraint_name, b2_x);
        constraint_i++;

        ILP_solver.set_constraint_upb(constraint_i, 2, {y_edge_offset_l+i,b1+y_module_offset}, {1, -1}, y_l_1_constraint_name, b1_y);
        constraint_i++;

        ILP_solver.set_constraint_upb(constraint_i, 2, {y_edge_offset_l+i,b2+y_module_offset}, {1, -1}, y_l_2_constraint_name, b2_y);
        constraint_i++;
    }
    //set edge and block right(top) constraints
    for(int i = 0; i<this->connections.size()&&minimize_wirelength; ++i){
        int b1 = connections[i].from;
        double b1_x = this->modules_wh[b1].get_half_x();
        double b1_y = this->modules_wh[b1].get_half_y();
        int b2 = connections[i].to;
        double b2_x = this->modules_wh[b2].get_half_x();
        double b2_y = this->modules_wh[b2].get_half_y();
        if(this->is_in_seq[b1]==0||this->is_in_seq[b2]==0){
            continue;
        }
        string x_r_1_constraint_name = "x_e_1_b_r"+ std::to_string(i);
        string y_r_1_constraint_name = "y_e_1_b_r"+ std::to_string(i);
        string x_r_2_constraint_name = "x_e_2_b_r"+ std::to_string(i);
        string y_r_2_constraint_name = "y_e_2_b_r"+ std::to_string(i);
        ILP_solver.set_constraint_upb(constraint_i, 2, {b1+x_module_offset,x_edge_offset_r+i}, {1, -1}, x_r_1_constraint_name, -b1_x);
        constraint_i++;

        ILP_solver.set_constraint_upb(constraint_i, 2, {b2+x_module_offset,x_edge_offset_r+i}, {1, -1}, x_r_2_constraint_name, -b2_x);
        constraint_i++;

        ILP_solver.set_constraint_upb(constraint_i, 2, {b1+y_module_offset,y_edge_offset_r+i}, {1, -1}, y_r_1_constraint_name, -b1_y);
        constraint_i++;

        ILP_solver.set_constraint_upb(constraint_i, 2, {b2+y_module_offset,y_edge_offset_r+i}, {1, -1}, y_r_2_constraint_name, -b2_y);
        constraint_i++;
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
    timer a1("get wirelength");
    a1.timer_start();
    this->predict_wirelength(minimize, with_area);
    a1.timer_end();
    //a1.print_time_elapsed();
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



