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
#include <iomanip>
#include "process/components/timer.h"
#include "process/functional/random_helper.h"
#include "static_data/fp_rule_t.cpp" //how bad
#include <algorithm>
#include <limits.h>
#include "sp_ilp_settings_t.h"
#include "bounding/bounding_line_t.h"
#include <fstream>
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
    // for(auto& e:this->constraint_graph_v){
    //     cout<< e.from<<" "<<e.to<<" "<<endl;
    // }
    
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
    simplify_constraint_graph();
    constraint_n = this->constraint_graph_h.size() + this->constraint_graph_v.size() + chip_t::get_fixed_modules().size()*2 + 5*soft_n;
    constraint_i = 1; //constraint_counter
    variable_n = 2*sequence_n + 5*soft_n;
    if(minimize_wirelength){
        variable_n += 4*sequence_pair_t::connections.size();
        constraint_n += 2 * sequence_pair_t::connections.size() + 4 * sequence_pair_t::connection_graph_deg;
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
    sp_ilp_settings_find_position_t sp_ilp_settings(this);
    
    sp_ilp_settings.set_constraints_modules_overlap_h();
    //if vertical constraint graph got i->j them y_j - y_i >= h
    sp_ilp_settings.set_constraints_modules_overlap_v();
    //set constraints to fix module
    sp_ilp_settings.set_constraints_modules_fixed();
    
    if(minimize_wirelength){
        //set constraints to give the direction to the edges
        //x_e_r >= x_e_l
        sp_ilp_settings.set_constraints_net_direction();
        //set edge and block left(bottom) constraints
        sp_ilp_settings.set_constraints_net();
    }
    //w+h = 1
    sp_ilp_settings.set_constraints_ratio_equal_1();
    //set modules' boundaires
    sp_ilp_settings.set_constraints_boundaries();
    //set variables of modules
    sp_ilp_settings.set_variables_modules();
    if(minimize_wirelength){
        sp_ilp_settings.set_variables_connections();
    }
    sp_ilp_settings.set_variables_shapes();


    //set coefficient of the objective function
    if(minimize_wirelength){
        sp_ilp_settings.set_coef(coef);
    }
    //solve
    ILP_solver.set_obj_coef(coef);
    ILP_solver.load();
    ILP_result = ILP_solver.solve(false);


    if(ILP_result.legal){
        if(load_result){
            vector<vec2d_t> result_pos = get_LP_res_pos();
            auto[result_wh, result_wh_i] = get_LP_res_wh();
            this->modules_positions.resize(result_pos.size());
            for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
                if(seq_is_fix[i]==false){
                    this->modules_wh[i] = {static_cast<int>(result_wh[i].get_x()), static_cast<int>(result_wh[i].get_y())};
                }
                this->modules_positions[i] = {static_cast<int>(result_pos[i].get_x()), static_cast<int>(result_pos[i].get_y())};
            }
            this->modules_wh_i = result_wh_i;
            
            this->z = ILP_result.z;
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
    //visualizer_t::show_fp_rect_no_border(rects, "SQP");
    std::vector<std::pair<std::vector<vec2d_t>,std::string>> bounding_lines;
    for(auto rect:rects){
        bounding_lines.push_back({rect.first.to_bounding_point(), rect.second});
    }
    visualizer_t::draw_bounding_line_connection(bounding_lines);
}

void sequence_pair_t::sequence_pair_validation(int t) {
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
    string str = "SQP, t = "+std::to_string(t);
    visualizer_t::show_fp_rect_no_border(rects, str);
}

void sequence_pair_t::set_bounding_lines(){
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        if(this->carved[i]){continue;}
        else{
            string name;
            if(seq_is_fix[i]){name = "f";}
            else{name = "s";}
            this->bouding_lines[i] = {this->get_4_points(this->modules_positions[i], this->modules_wh[i]), name+std::to_string(i)};
        }
    }
    
}



void sequence_pair_t::build_graph() {
    connections.clear();
    sequence_pair_t::connection_graph_deg = 0;
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

bool sequence_pair_t::check_area_ratio(){
    vector<int> area_compensation = this->get_correct_area();
    //this function will check the area, 1:2 constraints, but no overlap and 80% constraint
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        if(sequence_pair_t::seq_is_fix[i]){continue;}
        double area = this->modules_wh[i].get_x() * this->modules_wh[i].get_y();
        double ratio = this->modules_wh[i].get_x()/this->modules_wh[i].get_y();
        if(ratio>2||ratio<0.5){
            return false;
        }
        double min = this->modules_area[i];
        
        if(area<min+area_compensation[i]){
            return false;
        }
    }
    return true;

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
//    this->print_shapes_i();
//    for(int i = 0; i<this->soft_n; ++i){
//        for(auto& shape:sequence_pair_t::soft_area_to_w_h_m_5[i]){
//            cout<<"{"<<shape.get_x()<<","<<shape.get_y()<<"} ";
//        }
//    }
//    cout<<"wirelength : "<<std::setprecision(16)<<this->update_wirelength(true, false)<<endl;
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

void sequence_pair_t::print_result(){
    vector<int> area_compensation = this->get_correct_area();
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        double area = this->modules_wh[i].get_x()*this->modules_wh[i].get_y();
        cout<<i<<", pos : "<<std::setprecision(16)<<this->modules_positions[i]<<endl;
        cout<<i<<", area: "<<std::setprecision(16)<<area<<endl;
        cout<<i<<", wh : "<<std::setprecision(16)<<this->modules_wh[i]<<endl;
        cout<<i<<", minimum area: "<<std::setprecision(16)<<this->modules_area[i]<<endl;
        cout<<i<<", area - minimum: "<<std::setprecision(16)<<area - this->modules_area[i]<<endl;
        cout<<i<<", area - compensation: "<<std::setprecision(16)<<area - area_compensation[i]<<endl;
        cout<<i<<", Ratio: "<<std::setprecision(3)<<(1-static_cast<double>(area_compensation[i])/this->modules_area[i])*100<<"%"<<endl;
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
    // bool success;
    // if(with_area){
    //     success = this->find_position_with_area(minimize_wirelength,true,0, 0); //need result to calculate wirelength
    // }
    // else{
    //     success = this->find_position_allow_illegal(minimize_wirelength,true,0, 0); //need result to calculate wirelength
    // }
    // if(success==false){
    //     this->predicted_wirelength = -1;
    //     return;
    // }
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
    // cout<< "Actual : "<<std::setprecision(16)<<this->predicted_wirelength<<endl;
    // cout<< "LP result : "<<std::setprecision(16)<<this->ILP_result.z<<endl;
}

void sequence_pair_t::to_rectilinear(){
    this->find_position(true, true, 0, 0);
    this->find_position_with_area(true, true, 0, 0);
    this->fill_near();
    this->overlap_optimization();
    //this->carve();
    //this->set_bounding_lines();
    //visualizer_t::draw_bounding_line(SP.bouding_lines);
    //visualizer_t::draw_bounding_line_connection(SP.bouding_lines);
    // SP.print_inline();
    // SP.print_result();
    // cout<<"Rectangle wirelength: "<<SP.predicted_wirelength<<endl;
    //cout<<"Rectilinear wirelength: "<<this->rectilinear_wirelength<<endl;
    //cout<<std::setprecision(2)<<(this->predicted_wirelength-this->rectilinear_wirelength)/this->predicted_wirelength*100<<"% optimization"<<endl;
    //SP.sequence_pair_validation();
    //SP.to_fp().GUI_validation();
}

void sequence_pair_t::to_rectilinear_and_plot(){

    //this->sequence_pair_validation(2);
    for(auto& e:this->is_in_seq){e = 1;}
    this->fill_near();
    this->overlap_optimization();
    this->carve();
    this->set_bounding_lines();
    this->update_wirelength_rectilinear();
    this->deal_bounding_line();

    int soft = chip_t::get_soft_modules().size();
    int n = chip_t::get_total_module_n();
    std::fstream file("/home/jrchang/projects/ICCAD-2023PD/outputpng/vaild_check_txt/case03.txt", std::fstream::out);
    int total_vertexes = 0;
    int total_edges = 0;
    int total_vertexes270 = 0;
    for(int i = 0; i < soft; ++i) {
        bounding_line_t bd = bounding_line_t(this->bouding_lines[i].first);
        double area = bd.get_area();
        double ratio = bd.get_bounding_rect().get_size().get_x() / bd.get_bounding_rect().get_size().get_y();
        double percent = bd.get_area() / bd.get_bounding_rect().get_area();
        int ver = bd.get_vertex_count(); total_vertexes += ver;
        int ed = bd.get_edge_count(); total_edges += ed;
        int ver270 = bd.get_vertex270_count(); total_vertexes270 += ver270;
        file << std::setw(3) << this->bouding_lines[i].second << ", ";
        file << ((ratio <= 2 && ratio >= 0.5 && percent >= 0.8) ? "TRUE" : "FALSE") << ", ";
        file << std::setprecision(16) << std::setw(8) << area << ", ";
        file << std::fixed << std::setw(5) << std::setprecision(3) << ratio << ", ";
        file << std::fixed << std::setw(5) << std::setprecision(3) << percent << ", ";
        file << std::defaultfloat << std::setprecision(16) << std::setw(2) << ver << ", ";
        file << std::setw(2) << ed << ", ";
        file << std::setw(2) << ver270 << ", ";
        for(auto pos : bd.get_nodes()) {
            file << pos << " | ";
        }
        file << endl;
    }
    file << this->rectilinear_wirelength << endl;
    file << total_vertexes << endl;
    file << total_edges << endl;
    file << total_vertexes270 << endl;
    file.close();

        

    visualizer_t::draw_bounding_line(this->bouding_lines);
    fgetc(stdin);
    //visualizer_t::draw_bounding_line_connection(this->bouding_lines);
    //this->print_inline();
    //this->print_result();
    cout<<"Rectangle wirelength: "<<std::setprecision(16)<<this->predicted_wirelength<<endl;
    cout<<"Rectilinear wirelength: "<<std::setprecision(16)<<this->rectilinear_wirelength<<endl;
    cout<<std::setprecision(2)<<(this->predicted_wirelength-this->rectilinear_wirelength)/this->predicted_wirelength*100<<"% optimization"<<endl;
    //this->sequence_pair_validation();
    //this->to_fp().GUI_validation();
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
    sp_ilp_settings_find_position_with_area_t sp_ilp_settings(this);
    //set constraints to place modules
    //if horizontal constraint graph got i->j them x_j - x_i >= w
    sp_ilp_settings.set_constraints_modules_overlap_h();
    //if vertical constraint graph got i->j them y_j - y_i >= h
    sp_ilp_settings.set_constraints_modules_overlap_v();
    //set constraints to fix module
    sp_ilp_settings.set_constraints_modules_fixed();
    if(minimize_wirelength){
        //set constraints to give the direction to the edges
        sp_ilp_settings.set_constraints_net_direction();
        //set edge and block left(bottom) constraints
        sp_ilp_settings.set_constraints_net();
        //set variables for connections
        sp_ilp_settings.set_variables_connections();
    }
    
    //set variables for modules
    sp_ilp_settings.set_variables_modules();

    sp_ilp_settings.set_coef(coef);
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
            result.push_back( {static_cast<int>(ILP_result.var_values[i]), static_cast<int>(ILP_result.var_values[i+sequence_n])});
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

bool sequence_pair_t::find_position_allow_illegal_fill(bool minimize_wirelength, bool load_result,int overlap_h, int overlap_v){

    build_constraint_graph();
    mark_transitive_edge();
    //simplify_constraint_graph();
    constraint_n = 3*this->constraint_graph_h.size() + 3*this->constraint_graph_v.size() + chip_t::get_fixed_modules().size()*2 + 5*soft_n + 2*sequence_pair_t::sequence_n;
    constraint_i = 1; //constraint_counter
    variable_n = 2*sequence_n + 5*soft_n + 2*this->constraint_graph_h.size()+this->constraint_graph_v.size();
    if(minimize_wirelength){
        variable_n += 4*sequence_pair_t::connections.size();
        constraint_n += 2 * sequence_pair_t::connections.size() + 4 * sequence_pair_t::connection_graph_deg;
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

    x_overlap = 1+2*sequence_n+5*soft_n+4*this->connections.size();
    y_overlap = 1+2*sequence_n+5*soft_n+4*this->connections.size() + this->constraint_graph_h.size();

    //apart from constraint graph, every fix module need 2 additional condition
    ILP_solver = ILP_solver_t();
    ILP_solver.init("ILP_solver", constraint_n, variable_n);
    if(ILP_solver.get_is_invalid()){return false;}
    ILP_solver.set_min();


    vector<double> coef(variable_n+1, 0);

    //set constraints to place modules
    //if horizontal constraint graph got i->j them x_j - x_i >= w
    sp_ilp_settings_find_position_t sp_ilp_settings(this);
    
    sp_ilp_settings.set_constraints_modules_allow_overlap_h();
    //if vertical constraint graph got i->j them y_j - y_i >= h
    sp_ilp_settings.set_constraints_modules_allow_overlap_v();
    sp_ilp_settings.set_constraints_modules_overlap_content();
    //set constraints to fix module
    sp_ilp_settings.set_constraints_modules_fixed();
    
    if(minimize_wirelength){
        //set constraints to give the direction to the edges
        //x_e_r >= x_e_l
        sp_ilp_settings.set_constraints_net_direction();
        //set edge and block left(bottom) constraints
        sp_ilp_settings.set_constraints_net();
    }
    //w+h = 1
    sp_ilp_settings.set_constraints_ratio_equal_1();
    //set modules' boundaires
    sp_ilp_settings.set_constraints_boundaries();
    
    //set variables of modules
    sp_ilp_settings.set_variables_modules();
    sp_ilp_settings.set_variables_allow_overlap();

    if(minimize_wirelength){
        sp_ilp_settings.set_variables_connections();
    }
    sp_ilp_settings.set_variables_shapes();


    //set coefficient of the objective function
    if(minimize_wirelength){
        sp_ilp_settings.set_coef_allow_overlap(coef);
        //sp_ilp_settings.set_coef(coef);
    }
    //solve
    ILP_solver.set_obj_coef(coef);
    ILP_solver.load();
    ILP_result = ILP_solver.solve(false);


    if(ILP_result.legal){
        if(load_result){
            vector<vec2d_t> result_pos = get_LP_res_pos();
            auto[result_wh, result_wh_i] = get_LP_res_wh();
            this->modules_positions.resize(result_pos.size());
            for(int i = 0; i<sequence_pair_t::soft_n; ++i){
                this->modules_wh[i] = {static_cast<int>(result_wh[i].get_x()), static_cast<int>(result_wh[i].get_y())};
                this->modules_positions[i] = {static_cast<int>(result_pos[i].get_x()), static_cast<int>(result_pos[i].get_y())};
            }
            this->modules_wh_i = result_wh_i;
            for(int i = 0; i < sequence_pair_t::sequence_n; ++i){
                //cout<< i<<" "<<seq_is_fix[i]<<", "<<"{"<<ILP_result.var_values[i+this->x_overlap]<<", "<<ILP_result.var_values[i+this->y_overlap]<<"}"<<endl;
                if(ILP_result.var_values[i+this->x_overlap]||ILP_result.var_values[i+this->y_overlap]){
                    this->allow_to_overlap[i] = 1;
                }
                else{
                    this->allow_to_overlap[i] = 0;
                }
            }
            this->z = ILP_result.z;
            //cout<<this->z<<endl;
        }
        return true;
    }
    else{
        this->z = -1;
        return false;
    }
}

bool sequence_pair_t::find_position_allow_illegal(bool minimize_wirelength, bool load_result,int overlap_h, int overlap_v)
{
    this->allow_to_overlap = vector<int>(sequence_pair_t::sequence_n, 1);
    bool first_attempt = this->find_position_allow_illegal_fill(minimize_wirelength, load_result, overlap_h, overlap_v);
    //this->print_result();
    if(first_attempt==false){
        return this->find_position(true, true, 0, 0);
    }
    //to adjust area here
    //this->sequence_pair_validation(1);
    vector<int> area_compensation = this->get_correct_compensation();
    vector<vector<vec2d_t>> ori_wh = sequence_pair_t::soft_area_to_w_h_m_5;
    vector<vector<vec2d_t>> new_shape_5;
    for(int i = 0; i<sequence_n; ++i){
        new_shape_5.push_back(sequence_pair_t::find_w_h(this->modules_area[i]+area_compensation[i], 5));
    }
    //cout<<endl;
    sequence_pair_t::soft_area_to_w_h_m_5 = new_shape_5;
    bool second_attempt = this->find_position_allow_illegal_fill(minimize_wirelength, load_result, overlap_h, overlap_v);
    sequence_pair_t::soft_area_to_w_h_m_5 = ori_wh;
    vector<int> area_compensation_after = this->get_correct_area();
    if(second_attempt==false){
        return this->find_position(true, true, 0, 0);
    }
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        if(area_compensation_after[i]>area_compensation[i]){
            return this->find_position(true, true, 0, 0);;
        }
    }
    return true;
}

bool sequence_pair_t::find_position_allow_illegal_process()
{
    bool a = this->find_position(true, true, 0, 0);
    bool b = this->find_position_with_area(true, true, 0, 0);
    double z1 = this->z;
    bool c = this->find_position_allow_illegal(true, true, 0, 0);
    double z2 = this->z;
    if(a==false&&c==false){
        return false;
    }
    if(a^c){
        return true;
    }
    if(z1<z2){
        bool a = this->find_position(true, true, 0, 0);
        bool b = this->find_position_with_area(true, true, 0, 0);
        int x = this->z;
        int y = 3;
    }
    return true;
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

vector<vec2d_t> sequence_pair_t::get_4_points(vec2d_t ll, vec2d_t wh){
    vec2d_t ul = vec2d_t(ll.get_x(), ll.get_y()+  wh.get_y());
    vec2d_t ur = vec2d_t(ll.get_x()+wh.get_x(), ll.get_y()+  wh.get_y());
    vec2d_t lr = vec2d_t(ll.get_x()+wh.get_x(), ll.get_y());
    return {ll, ul, ur, lr};
}

void sequence_pair_t::fill_near()
{
    this->near_x.clear(); this->near_y.clear();
    this->near_x_map.clear(); this->near_y_map.clear(); 
    this->near_y_id.clear();this->near_x_id.clear();

    this->near_x_map.resize(this->sequence_n); this->near_y_map.resize(this->sequence_n);
    
    near_y_id = near_x_id = vector<vector<int>>(sequence_pair_t::sequence_n, vector<int>(sequence_n, -1));
    int ix,iy;
    ix = iy = 0;
    //cout<<"Near x"<<endl;
    vector<int> area_comp = this->get_correct_area();
    for(int i = 0; i< this->sequence_n; ++i){
        if(area_comp[i]){continue;}
        for(int j = 0; j< this->sequence_n; ++j){
            if(i==j || seq_is_fix[i]||seq_is_fix[j]){continue;}
            if( (modules_positions[i]+modules_wh[i]).get_x() == modules_positions[j].get_x()){
                int lower_y_i = modules_positions[i].get_y();
                int upper_y_i = modules_positions[i].get_y()+modules_wh[i].get_y();
                int lower_y_j = modules_positions[j].get_y();
                int upper_y_j = modules_positions[j].get_y()+modules_wh[j].get_y();
                int lower = std::min(lower_y_i, lower_y_j);
                int upper = std::max(upper_y_i, upper_y_j);
                if((lower_y_i<=upper_y_j&&lower_y_i>=lower_y_j)||(upper_y_i<=upper_y_j&&upper_y_i>=lower_y_j)){
                    // i -> j 
                
                    int ih = std::min(modules_wh[i].get_x()/4.01, 1.949*modules_wh[i].get_y()-modules_wh[i].get_x());
                    int jh = std::min(modules_wh[j].get_x()/4.01, 1.949*modules_wh[j].get_y()-modules_wh[j].get_x());

                    int h = std::min(ih, jh);
                    //cout<< i<<" "<<j<<" "<<h<<endl;
                    near_x.push_back({i, j, h});
                    near_x_id[j][i] = near_x_id[i][j] = ix++;
                    near_x_map[i].push_back(j);
                    near_x_map[j].push_back(i);
                }
                
            }
        }
    }

    //cout<<"Near y"<<endl;
    for(int i = 0; i< this->sequence_n; ++i){
        if(area_comp[i]){continue;}
        for(int j = 0; j< this->sequence_n; ++j){
            if(i==j || seq_is_fix[i]||seq_is_fix[j]){continue;}
            
            if( (modules_positions[i]+modules_wh[i]).get_y() == modules_positions[j].get_y()){
                int lower_x_i = modules_positions[i].get_x();
                int upper_x_i = modules_positions[i].get_x()+modules_wh[i].get_x();
                int lower_x_j = modules_positions[j].get_x();
                int upper_x_j = modules_positions[j].get_x()+modules_wh[j].get_x();
                int lower = std::min(lower_x_i, lower_x_j);
                int upper = std::max(upper_x_i, upper_x_j);
                if((lower_x_i<=upper_x_j && lower_x_i>=lower_x_j)||(upper_x_i<=upper_x_j && upper_x_i>=lower_x_j)){
                    // i -> j 
                    
                    int ih = std::min(modules_wh[i].get_y()/4.01, 1.949*modules_wh[i].get_x()-modules_wh[i].get_y());
                    int jh = std::min(modules_wh[j].get_y()/4.01, 1.949*modules_wh[j].get_x()-modules_wh[j].get_y());
                    int h = std::min(ih, jh);

                    //cout<< i<<" "<<j<<" "<<h<<endl;
                    near_y_id[j][i] = near_y_id[i][j] = iy++;
                    near_y.push_back({i, j, h});
                    near_y_map[i].push_back(j);
                    near_y_map[j].push_back(i);
                }
                
            }
        }
    }
}

void sequence_pair_t::overlap_optimization(){
    constraint_n =  10*sequence_pair_t::connections.size() + 2*sequence_pair_t::sequence_n + 2*this->near_x.size() + 2*this->near_y.size();
    variable_n = 2*sequence_n+ 4*connections.size() + this->near_x.size() + this->near_y.size();
    constraint_i = 1; //constraint_counter

    x_module_offset = 1;
    y_module_offset = 1+sequence_n;
    x_edge_offset_l = 1+2*sequence_n;
    x_edge_offset_r = 1+2*sequence_n+this->connections.size(); //e_x_r
    y_edge_offset_l = 1+2*sequence_n+2*this->connections.size(); //e_x_l
    y_edge_offset_r = 1+2*sequence_n+3*this->connections.size();  //e_x_r
    near_x_offset = 1+2*sequence_n+4*this->connections.size();
    near_y_offset = 1+2*sequence_n+4*this->connections.size()+this->near_x.size();

    ILP_solver = ILP_solver_t();
    ILP_solver.init("ILP_solver", constraint_n, variable_n);

    sp_ilp_settings_find_position_t sp_ilp_settings(this);
    sp_ilp_settings.set_constraints_net_direction();
    sp_ilp_settings.set_constraints_opt_nets();
    sp_ilp_settings.set_constraints_opt_modules();
    sp_ilp_settings.set_constraints_only_one_hand();
    sp_ilp_settings.set_variables_connections();
    sp_ilp_settings.set_variables_modules();
    sp_ilp_settings.set_variables_hands();
    
    
    vector<double> coef(variable_n);
    sp_ilp_settings.set_coef(coef);
    ILP_solver.set_obj_coef(coef);
    ILP_solver.load();
    ILP_result = ILP_solver.solve(true);
    // cout<<"------------------"<<endl;
    // cout<< "Legal: "<< ILP_result.legal<<endl;
    // cout<<"Z after rectilinear optimization: "<<std::setprecision(16)<<ILP_result.z<<endl;
    //for(auto& e:ILP_result.var_values){cout<<e<<endl;}
    for(int i = 0; i<near_x.size(); ++i){
        if(ILP_result.var_values[this->near_x_offset+i]){
            //for(auto& e:near_x[i]){cout<<e<<" ";}cout<<endl;
            this->result_carving_x.push_back({near_x[i]});
        }
    }
    for(int i = 0; i<near_y.size(); ++i){
        if(ILP_result.var_values[this->near_y_offset+i]){
            //for(auto& e:near_y[i]){cout<<e<<" ";}cout<<endl;
            this->result_carving_y.push_back({near_y[i]});
        }
    }

    if(ILP_result.legal){
        this->rectilinear_wirelength = ILP_result.z;
    }
    else{
        this->rectilinear_wirelength = this->predicted_wirelength;
    }
    if(this->rectilinear_wirelength<0){ //glpk so bad==
        this->rectilinear_wirelength = 1e300+1;
    }
}

void sequence_pair_t::carve(){
    this->bouding_lines.clear();
    this->bouding_lines.resize(sequence_n);
    this->carved = vector<int>(sequence_n, false);
    this->result_carving_x_enable = vector<bool>(this->result_carving_x.size(), false);
    this->result_carving_y_enable = vector<bool>(this->result_carving_y.size(), false);
    for(int i = 0; i<this->result_carving_x.size(); ++i){
        int from = result_carving_x[i][0], to = result_carving_x[i][1], h = result_carving_x[i][2];
        vec2d_t ll_from = this->modules_positions[from];
        vec2d_t from_wh = this->modules_wh[from];
        vec2d_t ll_to = this->modules_positions[to];
        vec2d_t to_wh = this->modules_wh[to];
        int car_top_y = std::min(ll_from.get_y()+from_wh.get_y(), ll_to.get_y()+to_wh.get_y());
        int car_bot_y = std::max(ll_from.get_y(), ll_to.get_y());
        int car_mid_y = (car_top_y+car_bot_y)/2;
        if(car_top_y - car_bot_y < 2){
            continue;
        }
        this->result_carving_x_enable[i] = true;
        this->carved[from] = this->carved[to] = true;
        vector<vec2d_t> from_4_points = this->get_4_points(ll_from, from_wh);
        vector<vec2d_t> to_4_points = this->get_4_points(ll_to, to_wh);
        vec2d_t c1 = vec2d_t(ll_from.get_x()+from_wh.get_x(), car_top_y);
        vec2d_t c2 = vec2d_t(ll_from.get_x()+from_wh.get_x()-h, car_top_y);
        vec2d_t c3 = vec2d_t(ll_from.get_x()+from_wh.get_x()-h, car_mid_y);
        vec2d_t c4 = vec2d_t(ll_from.get_x()+from_wh.get_x()+h, car_mid_y);
        vec2d_t c5 = vec2d_t(ll_from.get_x()+from_wh.get_x()+h, car_bot_y);
        vec2d_t c6 = vec2d_t(ll_from.get_x()+from_wh.get_x(), car_bot_y);
        vec2d_t c7 = vec2d_t(ll_from.get_x()+from_wh.get_x(), car_mid_y);
        // bouding_lines[from] = {{from_4_points[0], from_4_points[1], from_4_points[2], c1, c2,c3,c4, c5, c6, from_4_points[3]}, "s"+std::to_string(from)};
        // bouding_lines[to] = {{to_4_points[0], c6, c5, c4, c3,c2,c1,to_4_points[1], to_4_points[2], to_4_points[3]}, "s"+std::to_string(to)};

        bounding_line_t bd_from = bounding_line_t(rect_t(this->modules_positions[from], this->modules_wh[from]).get_bounding_rect());
        bounding_line_t bd_from_add = bounding_line_t({c4, c5, c6, c7});
        bounding_line_t bd_from_minus = bounding_line_t({c7, c3, c2, c1}, false);
        bd_from = bounding_line_t::merge(bd_from, bd_from_add).difference_pos_line[0];
        bd_from = bounding_line_t::merge(bd_from, bd_from_minus).difference_pos_line[0];
        bounding_line_t bd_to   = bounding_line_t(rect_t(this->modules_positions[to]  , this->modules_wh[to]  ).get_bounding_rect());
        bounding_line_t bd_to_add = bounding_line_t({c7, c3, c2, c1});
        bounding_line_t bd_to_minus = bounding_line_t({c4, c5, c6, c7}, false);
        bd_to = bounding_line_t::merge(bd_to, bd_to_add).difference_pos_line[0];
        bd_to = bounding_line_t::merge(bd_to, bd_to_minus).difference_pos_line[0];
        this->bouding_lines[from] = {bd_from.get_nodes(), "s"+std::to_string(from)};
        this->bouding_lines[to] = {bd_to.get_nodes(), "s"+std::to_string(to)};
    }


    for(int i = 0; i<this->result_carving_y.size(); ++i){
        int from = result_carving_y[i][0], to = result_carving_y[i][1], h = result_carving_y[i][2];
        vec2d_t ll_from = this->modules_positions[from];
        vec2d_t from_wh = this->modules_wh[from];
        vec2d_t ll_to = this->modules_positions[to];
        vec2d_t to_wh = this->modules_wh[to];
        int car_right_x = std::min(ll_from.get_x()+from_wh.get_x(), ll_to.get_x()+to_wh.get_x());
        int car_left_x = std::max(ll_from.get_x(), ll_to.get_x());
        int car_mid_x = (car_right_x+car_left_x)/2;
        if(car_right_x - car_left_x < 2){
            continue;
        }
        this->result_carving_y_enable[i] = true;
        this->carved[from] = this->carved[to] = true;
        vector<vec2d_t> from_4_points = this->get_4_points(ll_from, from_wh);
        vector<vec2d_t> to_4_points = this->get_4_points(ll_to, to_wh);
        vec2d_t c1 = vec2d_t(car_left_x, ll_from.get_y()+from_wh.get_y());
        vec2d_t c2 = vec2d_t(car_left_x, ll_from.get_y()+from_wh.get_y()+h);
        vec2d_t c3 = vec2d_t(car_mid_x, ll_from.get_y()+from_wh.get_y()+h);
        vec2d_t c4 = vec2d_t(car_mid_x, ll_from.get_y()+from_wh.get_y()-h);
        vec2d_t c5 = vec2d_t(car_right_x, ll_from.get_y()+from_wh.get_y()-h);
        vec2d_t c6 = vec2d_t(car_right_x, ll_from.get_y()+from_wh.get_y());
        vec2d_t c7 = vec2d_t(car_mid_x, ll_from.get_y()+from_wh.get_y());
        // bouding_lines[from] = {{from_4_points[0], from_4_points[1], c1,c2,c3,c4,c5, c6, from_4_points[2], from_4_points[3]}, "s"+std::to_string(from)};
        // bouding_lines[to] = {{to_4_points[0], to_4_points[1], to_4_points[2],to_4_points[3], c6,c5,c4,c3,c2,c1}, "s"+std::to_string(to)};

        bounding_line_t bd_from = bounding_line_t(rect_t(this->modules_positions[from], this->modules_wh[from]).get_bounding_rect());
        bounding_line_t bd_from_add = bounding_line_t({c1, c2, c3, c7});
        bounding_line_t bd_from_minus = bounding_line_t({c7, c6, c5, c4}, false);
        bd_from = bounding_line_t::merge(bd_from, bd_from_add).difference_pos_line[0];
        bd_from = bounding_line_t::merge(bd_from, bd_from_minus).difference_pos_line[0];
        bounding_line_t bd_to   = bounding_line_t(rect_t(this->modules_positions[to]  , this->modules_wh[to]  ).get_bounding_rect());
        bounding_line_t bd_to_add = bounding_line_t({c7, c6, c5, c4});
        bounding_line_t bd_to_minus = bounding_line_t({c1, c2, c3, c7}, false);
        bd_to = bounding_line_t::merge(bd_to, bd_to_add).difference_pos_line[0];
        bd_to = bounding_line_t::merge(bd_to, bd_to_minus).difference_pos_line[0];
        this->bouding_lines[from] = {bd_from.get_nodes(), "s"+std::to_string(from)};
        this->bouding_lines[to] = {bd_to.get_nodes(), "s"+std::to_string(to)};
    }
}

void sequence_pair_t::deal_bounding_line() {
    int soft = chip_t::get_soft_modules().size();
    int n = chip_t::get_total_module_n();

    std::vector<bounding_line_t> ori_bounding_lines;
    std::vector<bounding_line_t> ori_neg_bounding_lines;
    for(int i = 0; i < n; ++i) {
        ori_bounding_lines.push_back(bounding_line_t(this->bouding_lines[i].first));
        ori_neg_bounding_lines.push_back(bounding_line_t(this->bouding_lines[i].first, false));
    }

    std::vector<std::vector<int>> collision_num(soft);
    for(int i = 0; i < soft; ++i) {
        for(int j = i + 1; j < soft; ++j) {
            if(ori_bounding_lines[i].collision(ori_neg_bounding_lines[j])) {
                if(ori_bounding_lines[i].get_area() < ori_bounding_lines[j].get_area()) {
                    collision_num[j].push_back(i);
                } else {
                    collision_num[i].push_back(j);
                }
            }
        }
    }
    for(int i = 0; i < soft; ++i) {
        bounding_line_t bd0 = bounding_line_t(this->bouding_lines[i].first);
        for(int j = soft; j < n; ++j) {
            bounding_line_t bd1 = bounding_line_t(this->bouding_lines[j].first, false);
            auto bds = bounding_line_t::merge(bd0, bd1);
            if(!bds.difference_pos_line.empty()) bd0 = bds.difference_pos_line[0];
        }
        for(auto num : collision_num[i]) {
            if(i == num) continue;
            bounding_line_t bd1 = bounding_line_t(this->bouding_lines[num].first, false);
            auto bds = bounding_line_t::merge(bd0, bd1);
            if(!bds.difference_pos_line.empty()) bd0 = bds.difference_pos_line[0];
        }
        this->bouding_lines[i] = {bd0.get_nodes(), this->bouding_lines[i].second};        
    }
}

double sequence_pair_t::update_wirelength(bool minimize, bool with_area) {
    this->predict_wirelength(minimize, with_area);
    return this->predicted_wirelength;
}

void sequence_pair_t::update_wirelength_rectilinear(){
    vector<vec2d_t> pos = this->modules_positions;
    vector<vec2d_t> centers;
    double sum = 0;
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        vec2d_t center = { this->modules_positions[i].get_x()+this->modules_wh[i].get_half_x(),
                        this->modules_positions[i].get_y()+this->modules_wh[i].get_half_y()};
        centers.push_back(center);
    }
    for(int i = 0; i<result_carving_x.size(); ++i){
        if(result_carving_x_enable[i]==false){continue;}
        int from = result_carving_x[i][0], to = result_carving_x[i][1], h = result_carving_x[i][2];
        centers[from] = {centers[from].get_x()+h/2, centers[from].get_y()};
        centers[to] = {centers[to].get_x()-h/2, centers[to].get_y()};
    }
    for(int i = 0; i<result_carving_y.size(); ++i){
        if(result_carving_y_enable[i]==false){continue;}
        int from = result_carving_y[i][0], to = result_carving_y[i][1], h = result_carving_y[i][2];
        centers[from] = {centers[from].get_x(), centers[from].get_y()+h/2};
        centers[to] = {centers[to].get_x(), centers[to].get_y()-h/2};
    }
    for(int i = 0; i<connections.size();++i){
        double x_min = 1e9, y_min = 1e9, x_max = -1, y_max = -1;
        for(int j = 0; j<connections[i].nodes.size(); ++j){
            int v = connections[i].nodes[j];
            vec2d_t center_v = centers[v]; 
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
    this->rectilinear_wirelength = sum;
    // cout<< "Actual : "<<std::setprecision(16)<<this->predicted_wirelength<<endl;
    // cout<< "LP result : "<<std::setprecision(16)<<this->ILP_result.z<<endl;
}

void sequence_pair_t::print_wirelength(bool minimize, bool with_area) {
    cout<<std::setprecision(16)<<this->update_wirelength(minimize,with_area)<<endl;
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
    fout<<"wirelength : "<<std::setprecision(16)<<this->update_wirelength(true, true)<<endl;
    fout.close();
}

void sequence_pair_t::simplify_constraint_graph() {
    int N = sequence_pair_t::sequence_n;
    this->is_transitive_v = this->is_transitive_h = vector<vector<int>>(sequence_pair_t::sequence_n, vector<int>(sequence_pair_t::sequence_n, 0));
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
                        this->is_transitive_v[i][j] = 1;
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
                        this->is_transitive_h[i][j] = 1;
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

void sequence_pair_t::mark_transitive_edge(){
    int N = sequence_pair_t::sequence_n;
    this->is_transitive_v = this->is_transitive_h = vector<vector<int>>(sequence_pair_t::sequence_n, vector<int>(sequence_pair_t::sequence_n, 0));
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
                        this->is_transitive_v[i][j] = 1;
                    }
                }
            }
        }
        for(auto& e: this->constraint_graph_v){
            if(G[e.from][e.to]){
                simplified.push_back(e);
            }
        }
        //this->constraint_graph_v = simplified;
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
                        this->is_transitive_h[i][j] = 1;
                    }
                }
            }
        }
        for(auto& e: this->constraint_graph_h){
            if(G[e.from][e.to]){
                simplified.push_back(e);
            }
        }
        //this->constraint_graph_h = simplified;
    }
    for(auto& e:this->constraint_graph_v){
        cout<< e.from<<" "<<e.to<<" "<<this->is_transitive_v[e.from][e.to]<<endl;
    }
}

vector<int> sequence_pair_t::get_correct_area() {
    vector<rect_t> rects;
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        rects.push_back({this->modules_positions[i], this->modules_wh[i]});
    }
    int n = rects.size();
    int soft = chip_t::get_soft_modules().size();
    vector<int> result(n, 0);
    for(int i = 0; i < soft; ++i) {
        for(int j = i+1; j < n; ++j) {
            if(rects[i].is_collision(rects[j]) && this->modules_area[i]>this->modules_area[j])
                result[i] += rects[j].get_area();
        }
    }
    return result;
}
vector<int> sequence_pair_t::get_correct_compensation() {
    vector<rect_t> rects;
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        rects.push_back({this->modules_positions[i], this->modules_wh[i]});
    }
    int n = rects.size();
    int soft = chip_t::get_soft_modules().size();
    vector<int> result(n, 0);
    for(int i = 0; i < soft; ++i) {
        for(int j = i+1; j < n; ++j) {
            if(rects[i].is_collision(rects[j]) && this->modules_area[i]>this->modules_area[j])
                result[i] += rects[j].get_area();
        }
    }
    for(int i = 0; i < soft; ++i) {
        if(rects[i].get_area() < 4 * result[i])
            result[i] += (4 * result[i] - rects[i].get_area());
    }
    return result;
}