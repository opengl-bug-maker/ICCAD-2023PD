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
int sequence_pair_t::sequence_n;
int sequence_pair_t::fix_start_idx;
vector<std::vector<vec2d_t>> sequence_pair_t::soft_area_to_w_h_m;
vector<bool> sequence_pair_t::seq_is_fix;
vector<soft_module_t*> sequence_pair_t::seq_soft_map;
vector<fixed_module_t*> sequence_pair_t::seq_fixed_map;
unordered_map<const module_t*, int> sequence_pair_t::soft_module_to_id_m;
unordered_map<const module_t*, int> sequence_pair_t::fix_module_to_id_m;
vector<edge_t> sequence_pair_t::connections;
vector<vector<int>> sequence_pair_t::connectivities;
using std::cout;
using std::endl;
using std::string;
void sequence_pair_t::init() {
    srand(time(NULL));

    const std::vector<soft_module_t*>& soft_modules = chip_t::get_soft_modules();
    const std::vector<fixed_module_t*>& fixed_modules = chip_t::get_fixed_modules();
    sequence_pair_t::sequence_n = static_cast<int>(chip_t::get_total_module_n());
    sequence_pair_t::fix_start_idx = chip_t::get_soft_modules().size();
    soft_area_to_w_h_m.resize(soft_modules.size());
    sequence_pair_t::seq_soft_map.resize(chip_t::get_total_module_n());
    sequence_pair_t::seq_fixed_map.resize(chip_t::get_total_module_n());
    sequence_pair_t::seq_is_fix.resize(chip_t::get_total_module_n());

    connectivities.resize(sequence_n);
    for(int i = 0; i<sequence_n; ++i){
        connectivities[i].resize(sequence_n);
    }
    int j = 0;
    for(int i = 0; i<soft_modules.size(); ++i){
        sequence_pair_t::seq_soft_map[j] = soft_modules[i];
        sequence_pair_t::soft_area_to_w_h_m[i] = sequence_pair_t::find_w_h( soft_modules[i]->get_area());

        sequence_pair_t::seq_fixed_map[j] = nullptr;
        sequence_pair_t::seq_is_fix[j] = false;

        sequence_pair_t::soft_module_to_id_m[soft_modules[i]] = j;
        j++;
    }
    for(int i = 0; i<fixed_modules.size(); ++i){
        sequence_pair_t::seq_fixed_map[j] = fixed_modules[i];
        sequence_pair_t::seq_soft_map[j] = nullptr;
        sequence_pair_t::seq_is_fix[j] = true;

        sequence_pair_t::fix_module_to_id_m[fixed_modules[i]] = j;
        j++;
    }


    //build adj list and adj matrix
    build_graph();
}

sequence_pair_t::sequence_pair_t() {
    //max_overlap = static_cast<int>(static_cast<float>(std::max(chip_t::get_height(), chip_t::get_width()))*0.3);
    max_overlap = 0;
    max_distance = static_cast<int>(static_cast<float>(std::max(chip_t::get_height(), chip_t::get_width()))*0,2);
    v_sequence.resize(chip_t::get_total_module_n());
    h_sequence.resize(chip_t::get_total_module_n());
    soft_seq_h.resize(chip_t::get_soft_modules().size());
    soft_seq_v.resize(chip_t::get_soft_modules().size());
    modules_wh.resize(chip_t::get_total_module_n());
    is_in_seq.resize(chip_t::get_total_module_n());
    //determine the shape of the modules
    for(int i = 0; i<chip_t::get_total_module_n(); ++i){
        if(sequence_pair_t::seq_is_fix[i]){
            this->modules_wh[i] = seq_fixed_map[i]->get_size();
        }
        else{
            vector<vec2d_t> shapes = find_w_h(seq_soft_map[i]->get_area());
            int id = static_cast<int>(rand())%shapes.size();
            this->modules_wh[i] = shapes[id];
        }

        is_in_seq[i] = 0;
    }

    //set sequence to [1, n] by default
    for(int i = 0; i<v_sequence.size(); ++i){
        h_sequence[i] = v_sequence[i] = i;
    }
    for(int i = 0; i<chip_t::get_soft_modules().size(); ++i){
        soft_seq_h[i] = soft_seq_v[i] = i;
    }

}

void sequence_pair_t::set_v(std::vector<int> v_sequence) {
    this->v_sequence.resize(v_sequence.size());
    for(int i = 0; i<v_sequence.size(); ++i){
        this->v_sequence[i] = v_sequence[i];
    }
    //this->v_sequence = v_sequence;
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
    if(area==1){return {{1,1}};}
    uint32_t  from = sqrt(area/2);
    vector<vec2d_t> ret;
    for(uint32_t i = from; i<= sqrt(area); ++i){ //i will be the short edge
        //cout<< area<<" "<<i<<" "<<ceil(static_cast<float>(area)/static_cast<float>(i))<<endl;
//        int x = i, y = ceil(static_cast<float>(area)/static_cast<float>(i));
//        if(x*2>=y){
//            ret.push_back({x, y});
//            if(x!=y){
//                ret.push_back({y, x});
//            }
//        }
        int x = i, y = ceil(static_cast<float>(area)/static_cast<float>(i));
        if(x*1.35>=y){
            ret.push_back({x, y});
            if(x!=y){
                ret.push_back({y, x});
            }
        }
    }
    if(ret.size()==0){
        int x = ceil(sqrt(area));
        return {{x,x}};
    }
    return ret;
}

pair<bool, floorplan_t> sequence_pair_t::get_fp() {
    build_constraint_graph();

    bool success = false;
    pair<bool, vector<vec2d_t>> presolve_res = {false, {}};
    int overlap_v = 0, overlap_h = 0;
    for(int i = 0; i<=max_overlap; ++i){
        presolve_res = this->find_position(overlap_h, overlap_v);
        if(presolve_res.first){
            success=true;
            break;
        }
        if(i&1){
            overlap_v++;
        }
        else{
            overlap_h++;
        }
    }

    if(success){
        //this->sequence_pair_validation(presolve_res.second);
        //fgetc(stdin);
        //cout<< "SUCCESS in ILP"<<endl;
        pair<bool, floorplan_t> valid_fp = this->place_all_modules(presolve_res.second);
        return valid_fp;
    }
    else{
        return {false, floorplan_t()};
    }
}

void sequence_pair_t::build_constraint_graph() {
    vector<edge_t> updated_constraint_graph_h, updated_constraint_graph_v;
    vector<int> v_map(sequence_pair_t::sequence_n),h_map(sequence_pair_t::sequence_n);
    for(int i = 0; i<v_sequence.size(); ++i){
        v_map[v_sequence[i]] = i;
        h_map[h_sequence[i]] = i;
    }
    for(int i = 0; i<v_sequence.size(); ++i){
        for(int j = 0; j<v_sequence.size(); ++j) {
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
    cout<<"horizontal constraint graph : "<<endl;
    for(auto& e:constraint_graph_h){
        cout<< "{"<<e.from<<"->"<<e.to<<", "<<e.w<<"}"<<endl;
    }
    cout<<"vertical constraint graph : "<<endl;
    for(auto& e:constraint_graph_v){
        cout<< "{"<<e.from<<"->"<<e.to<<", "<<e.w<<"}"<<endl;
    }
}
void sequence_pair_t::build_constraint_graph_soft() {
    vector<edge_t> updated_constraint_graph_h, updated_constraint_graph_v;
    vector<int> v_map(this->soft_seq_v.size()),h_map(this->soft_seq_h.size());
    for(int i = 0; i<this->soft_seq_v.size(); ++i){
        v_map[soft_seq_v[i]] = i;
        h_map[soft_seq_h[i]] = i;
    }
    for(int i = 0; i<this->soft_seq_v.size(); ++i){
        for(int j = 0; j<this->soft_seq_v.size(); ++j) {
            if(v_map[i]<v_map[j] && h_map[i]<h_map[j]){ //{a, b}, {a, b}
                updated_constraint_graph_h.push_back({i, j, static_cast<int>(this->modules_wh[i].get_x())});
            }
            else if(v_map[i]<v_map[j] && h_map[j]<h_map[i]) { //{a, b}, {b,a}
                updated_constraint_graph_v.push_back({i, j, static_cast<int>(this->modules_wh[i].get_y())});
            }
        }
    }
    this->constraint_graph_h_soft = updated_constraint_graph_h;
    this->constraint_graph_v_soft = updated_constraint_graph_v;
    cout<<"horizontal constraint graph : "<<endl;
    for(auto& e:constraint_graph_h_soft){
        cout<< "{"<<e.from<<"->"<<e.to<<", "<<e.w<<"}"<<endl;
    }
    cout<<"vertical constraint graph : "<<endl;
    for(auto& e:constraint_graph_v_soft){
        cout<< "{"<<e.from<<"->"<<e.to<<", "<<e.w<<"}"<<endl;
    }
}
pair<bool, vector<vec2d_t>> sequence_pair_t::find_position(int overlap_h, int overlap_v) {
    build_constraint_graph();
    //int constraint_n = this->constraint_graph_h.size()+this->constraint_graph_v.size()+chip_t::get_fixed_modules().size()*2;
    int constraint_n = this->constraint_graph_h.size()+this->constraint_graph_v.size()+chip_t::get_fixed_modules().size()*2;
    int constraint_i = 1; //constraint_counter
    int set_n = 2*(this->constraint_graph_h.size()+this->constraint_graph_v.size())+2*chip_t::get_fixed_modules().size();
    //apart from constraint graph, every fix module need 2 additional condition
    ILP_solver_t ILP_solver;
    ILP_solver.init("ILP_solver", constraint_n, 2*sequence_n);
    if(ILP_solver.get_is_invalid()){return{false, {}};};
    ILP_solver.set_min();


    vector<int> set_i(set_n+1),set_j(set_n+1),set_val(set_n+1); //due to 1-index
    vector<int> coef_h(sequence_n,0), coef_v(sequence_n,0),coef(2*sequence_n+1);


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
    for(int i = 0; i<sequence_n; ++i){
        coef_v[i] = coef_h[i] = 1;
    }
    //prepare coefficients
    for(int i = 1; i<2*sequence_n; ++i){
        if(i<sequence_n){
            coef[i] = coef_h[i-1];
        }
        else{
            coef[i] = coef_v[i-sequence_n-1];
        }
    }
    //solve
    ILP_solver.set_obj_coef(coef);
    ILP_solver.load();
    ILP_result_t ILP_result = ILP_solver.solve();
    vector<vec2d_t> result; //zero-index
    cout<< "z : "<< ILP_result.z<<endl;
    for(int i = 1; i<=sequence_n; ++i){
        if(this->is_in_seq[i-1]==0){
            result.push_back({-1, -1});
        }
        else {
            result.push_back( {ILP_result.var_values[i], ILP_result.var_values[i+sequence_n]});
        }

    }
    if(ILP_result.legal){
        return {true, result};
    }
    else{return {false, {}};}


}

pair<bool, vector<vec2d_t>> sequence_pair_t::find_soft_position(int overlap_h, int overlap_v) {
    build_constraint_graph_soft();
    //int constraint_n = this->constraint_graph_h.size()+this->constraint_graph_v.size()+chip_t::get_fixed_modules().size()*2;
    int constraint_n = this->constraint_graph_h_soft.size()+this->constraint_graph_v_soft.size();
    int constraint_i = 1; //constraint_counter
    //int set_n = 2*(this->constraint_graph_h_soft.size()+this->constraint_graph_v_soft.size())+2*chip_t::get_fixed_modules().size();
    //apart from constraint graph, every fix module need 2 additional condition
    ILP_solver_t ILP_solver;
    const int var_n = 2*this->soft_seq_v.size()+(chip_t::get_soft_modules().size()*chip_t::get_fixed_modules().size());
    const double M = 1e15;
    ILP_solver.init("ILP_solver", constraint_n, var_n); //for each module, (x, y, t,b,l,r)
    if(ILP_solver.get_is_invalid()){return{false, {}};};
    ILP_solver.set_min();


    //vector<int> set_i(set_n+1),set_j(set_n+1),set_val(set_n+1); //due to 1-index
    vector<int> coef_h(sequence_n,0), coef_v(sequence_n,0),coef(2*sequence_n+1);


    //set constraints to place modules
    //if horizontal constraint graph got i->j them x_j - x_i >= w
    for(int i = 0; i<this->constraint_graph_h_soft.size(); ++i){
        int from = constraint_graph_h_soft[i].from, to = constraint_graph_h_soft[i].to, w = constraint_graph_h_soft[i].w;
        string constraint_name = "h_c"+ std::to_string(i);
        ILP_solver.set_constraint_upb(constraint_i, 2, {from+1, to+1}, {1, -1}, constraint_name, -w+overlap_h);
        constraint_i++;
    }
    //if vertical constraint graph got i->j them y_j - y_i >= h
    for(int i = 0; i<this->constraint_graph_v_soft.size(); ++i){
        int from = constraint_graph_v_soft[i].from, to = constraint_graph_v_soft[i].to, w = constraint_graph_v_soft[i].w;
        string constraint_name = "v_c"+ std::to_string(i);
        int offset = soft_seq_v.size();
        ILP_solver.set_constraint_upb(constraint_i, 2, {from+1+offset, to+1+offset}, {1, -1}, constraint_name, -w+overlap_v);
        constraint_i++;
    }
    //set constraints to fix module
    for(int i = 0; i<sequence_pair_t::soft_seq_v.size(); ++i){
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
    //set variables
    for(int i = 1;i<=soft_seq_v.size(); ++i){
        string var_name = "x"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, i, var_name.c_str());
        //ILP_solver.set_variable_double_range(i, 0.0,chip_t::get_width()-this->modules_wh[i-1].get_x());
        ILP_solver.set_variable_double_range(i, 0.0,chip_t::get_width()-this->modules_wh[i-1].get_x());

    }
    for(int i = soft_seq_v.size()+1;i<=2*soft_seq_v.size(); ++i){
        string var_name = "x"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, i, var_name.c_str());
        //ILP_solver.set_variable_double_range(i, 0.0, chip_t::get_height()-this->modules_wh[ i-1-sequence_n].get_y());
        ILP_solver.set_variable_double_range(i, 0.0, chip_t::get_height()-this->modules_wh[ i-1-soft_seq_v.size()].get_y());
    }
    for(int i = 0; i<soft_seq_v.size(); ++i){
        coef_v[i] = coef_h[i] = 0;
    }
    //prepare coefficients
    for(int i = 1; i<2*soft_seq_v.size(); ++i){
        if(i<soft_seq_v.size()){
            coef[i] = coef_h[i-1];
        }
        else{
            coef[i] = coef_v[i-soft_seq_v.size()-1];
        }
    }
    //solve
    ILP_solver.set_obj_coef(coef);
    ILP_solver.load();
    ILP_result_t ILP_result = ILP_solver.solve();
    vector<vec2d_t> result; //zero-index
    cout<< "z : "<< ILP_result.z<<endl;
    for(int i = 1; i<=soft_seq_v.size(); ++i){
        result.push_back( {ILP_result.var_values[i], ILP_result.var_values[i+soft_seq_v.size()]});
    }
    if(ILP_result.legal){
        return {true, result};
    }
    else{return {false, {}};}


}


void sequence_pair_t::sequence_pair_validation(vector<vec2d_t> res) {
    vector<std::pair<rect_t, std::string>> rects;
    for(int i = 0; i<res.size(); ++i){
        if(res[i].get_x()<0||res[i].get_y()<0){continue;}
        if(this->seq_is_fix[i]){
            rects.push_back({{res[i], this->modules_wh[i]}, "f"+ std::to_string(i)});
        }
        else{
            rects.push_back({{res[i], this->modules_wh[i]}, "s"+std::to_string(i)});
        }
    }
    visualizer_t::show_fp_rect_no_border(rects, "SQP");

}

void sequence_pair_t::build_graph() {
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
                connections.push_back({i, neighbor_i, neighbor.second});
                connectivities[neighbor_i][i] = connectivities[i][neighbor_i] = neighbor.second;
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
                connections.push_back({i, neighbor_i, neighbor.second});
                connectivities[neighbor_i][i] = connectivities[i][neighbor_i] = neighbor.second;
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
        bool success = place_8d(fp,i, res[i], modules_wh[i]);
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
    cout<<"H: {";
    for(int i = 0; i<this->h_sequence.size(); ++i){
        cout<< h_sequence[i]<<" ";
    }
    cout<<"}"<<endl;
    cout<<"V: {";
    for(int i = 0; i<this->v_sequence.size(); ++i){
        cout<< v_sequence[i]<<" ";
    }
    cout<<"}"<<endl;
}

bool sequence_pair_t::place_8d(floorplan_t& fp,int id,vec2d_t res, vec2d_t wh) {

    const int dir_x[8] = {0,0,1,-1,1,1,-1,-1};
    const int dir_y[8] = {1,-1,0,0,1,-1,1,-1};
    bool finish = false;
    for(int i = 0; i<=max_distance&&!finish; ++i){
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
    cout<<"v: [";
    for(int i = 0; i<this->v_sequence.size(); ++i){
        cout<< v_sequence[i];
        if(i!=this->v_sequence.size()-1){cout<<", ";}
    }
    cout<<"] ";
    cout<<"h: [";
    for(int i = 0; i<this->h_sequence.size(); ++i){
        cout<< h_sequence[i];
        if(i!=this->h_sequence.size()-1){cout<<", ";}
    }

    cout<<"]"<<endl;
}
void sequence_pair_t::print_soft_inline() {
    cout<<"v: [";
    for(int i = 0; i<this->soft_seq_v.size(); ++i){
        cout<< soft_seq_v[i];
        if(i!=this->soft_seq_v.size()-1){cout<<", ";}
    }
    cout<<"] ";
    cout<<"h: [";
    for(int i = 0; i<this->soft_seq_h.size(); ++i){
        cout<< soft_seq_h[i];
        if(i!=this->soft_seq_h.size()-1){cout<<", ";}

    }

    cout<<"]"<<endl;
}
void sequence_pair_t::print_shapes() {
    for(int i = 0; i<sequence_n; ++i){
        cout<< "i: "<< i<<" "<<modules_wh[i].get_x()<<" "<<modules_wh[i].get_y()<<endl;
    }
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

bool sequence_pair_t::add_soft_process(int i) {
    if(i>=fix_start_idx){return true;}
    for(int j = 0; j<=v_sequence.size(); ++j){
        for(int k = 0; k<=h_sequence.size(); ++k){
            v_sequence.insert(v_sequence.begin()+j, i);
            h_sequence.insert(h_sequence.begin()+k, i);
            pair<bool, vector<vec2d_t>> res = this->find_position(0, 0);
            if(res.first){
                add_soft_process(i+1);
            }
            v_sequence.erase(v_sequence.begin()+j);
            h_sequence.erase(h_sequence.begin()+k);
        }
    }
    return false;
}


void sequence_pair_enumerator_t::set_seed_need_n(int n) {
    this->seed_need_n = n;
}

void sequence_pair_enumerator_t::fill_seeds(int x) {
    if(seeds.size()>=seed_need_n){return;}
    if(x==seed.size()){
        this->seeds.push_back(this->seed);
        return;
    }
    for (int i = x; i < seed.size(); ++i) {
        std::swap(seed[x], seed[i]);
        fill_seeds(x + 1);
        std::swap(seed[x], seed[i]);
    }
}

sequence_pair_enumerator_t::sequence_pair_enumerator_t() {
    this->fix_n = chip_t::get_fixed_modules().size();
    this->soft_n = chip_t::get_soft_modules().size();
    this->seed.resize(sequence_pair_t::sequence_n);
    this->legal_pairs.resize(sequence_pair_t::sequence_n);
    this->fix_sequence_v.resize(fix_n);
    this->fix_sequence_h.resize(fix_n);

    for(auto& e:legal_pairs){
        e.resize(sequence_pair_t::sequence_n);
    }
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        seed[i] = i;
    }
    seed_need_n = 0;
    this->soft_seq_interval.resize(soft_n);
    set_fix_sequence();

    cout<< "got fix sequence: "<<endl;
    cout<<"V: ";for(auto& e:this->fix_sequence_v){cout<<e<<" ";}cout<<endl;
    cout<<"H: ";for(auto& e:this->fix_sequence_h){cout<<e<<" ";}cout<<endl;

}

void sequence_pair_enumerator_t::generate_valid_seq(int x) {

    //this->valid_seq.clear();
    for(int i = 0; i<seeds.size(); ++i){
        bool continuous_fail = false;
        for(int j = 0; j<seeds.size(); ++j) {
            seq.set_h(seeds[i]);
            seq.set_v(seeds[j]);
            pair<bool, floorplan_t> success_fp = seq.get_fp(); //try if the sequence pair could be a legal fp
            if (success_fp.first == false) { continue; }

//            this->valid_seq.push_back(seq);
//            if (valid_seq.size() >= x) { return; }

        }
    }
}

void sequence_pair_enumerator_t::print_all_valid_seq() {
//    for(auto& valid_sequence:this->valid_seq){
//        valid_sequence.print();
//    }
}

vector<sequence_pair_t> sequence_pair_enumerator_t::get_all_valid_seq() {
    //return this->valid_seq;
    return {};
}

void sequence_pair_enumerator_t::change_size_for_i(int i){
    if(sequence_pair_t::seq_is_fix[i]){
        seq.modules_wh[i] = sequence_pair_t::seq_fixed_map[i]->get_size();
    }
    else{
        vector<vec2d_t> shapes = seq.find_w_h(sequence_pair_t::seq_soft_map[i]->get_area());
        int id = static_cast<int>(rand())%shapes.size();
        seq.modules_wh[i] = shapes[id];
    }
}

void sequence_pair_enumerator_t::seq_randomize() {
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        int x = rand()%sequence_pair_t::sequence_n;
        int y = rand()%sequence_pair_t::sequence_n;
        this->seq.swap_h(i, x);
        this->seq.swap_v(i, y);
    }
}

void sequence_pair_enumerator_t::seq_init() {
    vector<int> v_init_sequence(sequence_pair_t::sequence_n), h_init_sequence(sequence_pair_t::sequence_n);
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        v_init_sequence[i] = h_init_sequence[i] = i;
    }
    this->seq.set_v(v_init_sequence);
    this->seq.set_h(h_init_sequence);
}


void sequence_pair_enumerator_t::set_fix_sequence() {
    if(this->fix_n==0){return;}
    //fix module sort by x and y
//    vector<int> test_v = {};
//    vector<int> test_h = {};
   for(int i =0; i<this->fix_n; ++i){
       this->fix_sequence_h[i] = this->fix_sequence_v[i] = i+sequence_pair_t::fix_start_idx;
   }
    vector<vector<int>> fix_constraint_v(sequence_pair_t::sequence_n, vector<int>(sequence_pair_t::sequence_n, 0));
    vector<vector<int>> fix_constraint_h(sequence_pair_t::sequence_n, vector<int>(sequence_pair_t::sequence_n, 0));
    for(int i = sequence_pair_t::fix_start_idx; i<sequence_pair_t::sequence_n; ++i) {
        for (int j = sequence_pair_t::fix_start_idx; j < sequence_pair_t::sequence_n; ++j) {
            if (i == j) { continue; }
            int ix = seq.seq_fixed_map[i]->get_left_lower().get_x();
            int iy = seq.seq_fixed_map[i]->get_left_lower().get_y();
            int jx = seq.seq_fixed_map[j]->get_left_lower().get_x();
            int jy = seq.seq_fixed_map[j]->get_left_lower().get_y();
            int iw = seq.seq_fixed_map[i]->get_size().get_x();
            int ih = seq.seq_fixed_map[i]->get_size().get_y();
            if (ix + iw <= jx) {
                cout<<"fix h : "<< i<<" "<<j<<endl;
                fix_constraint_h[i][j] = 1;
            }
            if (iy + ih <= jy) {
                cout<<"fix v : "<< i<<" "<<j<<endl;
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

    cout<<"V :";for(auto& e:upd_v){cout<<e<<" ";}cout<<endl;
    cout<<"H :";for(auto& e:upd_h){cout<<e<<" ";}cout<<endl;
    this->fix_sequence_v = upd_v;
    this->fix_sequence_h = upd_h;
}

std::set<int> sequence_pair_enumerator_t::random_choose(int upb, int n) {
    std::set<int> selected_fix_id;
    while(selected_fix_id.size()<n){
        int id = rand()%upb;
        selected_fix_id.insert(id);
    }
    return selected_fix_id;
}

void sequence_pair_enumerator_t::find_illegal_pair_for_i(int i) {
    int i_h = this->seq.modules_wh[i].get_x();
    int i_w = this->seq.modules_wh[i].get_y();
    //cout<< i_w<<" "<<i_h<<endl;
    for(int j = sequence_pair_t::fix_start_idx; j<sequence_pair_t::sequence_n; ++j){
        this->legal_pairs[i][j].clear();
        int fix_x = sequence_pair_t::seq_fixed_map[j]->get_left_lower().get_x();
        int fix_y = sequence_pair_t::seq_fixed_map[j]->get_left_lower().get_y();
        int fix_w = sequence_pair_t::seq_fixed_map[j]->get_size().get_x();
        int fix_h = sequence_pair_t::seq_fixed_map[j]->get_size().get_y();
        if(fix_x-i_w<0){
            this->legal_pairs[i][j].push_back(0); //left
            cout<< i<<" "<<j<<" left"<<endl;
        }
        if(fix_x+fix_w+i_w>static_cast<int>(chip_t::get_width())){
            this->legal_pairs[i][j].push_back(1); //right
            cout<< i<<" "<<j<<" right"<<endl;
        }
        if(fix_y+fix_h+i_h>static_cast<int>(chip_t::get_height())){
            this->legal_pairs[i][j].push_back(2); //upper
            cout<< i<<" "<<j<<" top"<<endl;
        }
        if(fix_y-i_h<0){
            this->legal_pairs[i][j].push_back(3); //bottom
            cout<< i<<" "<<j<<" bottom"<<endl;
        }
    }

}

void sequence_pair_enumerator_t::search_legal_perm_in_fix(int soft_i) {
    int vL = -1, vR = fix_n, hL = -1, hR = fix_n;
    vector<int> v_fix_map(sequence_pair_t::sequence_n), h_fix_map(sequence_pair_t::sequence_n);
    for(int i = 0; i<this->fix_sequence_v.size(); ++i){
        h_fix_map[fix_sequence_h[i]] = v_fix_map[fix_sequence_v[i]] = i;
    }
    for(int i = sequence_pair_t::fix_start_idx; i<sequence_pair_t::sequence_n; ++i){
        int fix_id_v = v_fix_map[i], fix_id_h = h_fix_map[i];
        int x = rand()%2;
        for(int j = 0; j<this->legal_pairs[soft_i][i].size(); ++j){
            if(this->legal_pairs[soft_i][i][j]==1){ //right
                //cant be V:[F, S], H: [F, S]
                if(vR<=fix_id_v||hR<=fix_id_h){continue;}
                if(fix_id_v>vL && x){
                    vR = std::min(vR, fix_id_v);
                }
                if(hL<fix_id_h && !x){
                    hR = std::min(hR, fix_id_h);
                }
            }
            else if(this->legal_pairs[soft_i][i][j]==0){ //left
                if(vL>fix_id_v||hL>fix_id_h){continue;}
                //cant be V:[S, F], H: [S, F]
                if(fix_id_v<vR&&x){
                    vL = std::max(vL, fix_id_v);
                }
                if(fix_id_h<hR&&!x){
                    hL = std::max(hL, fix_id_h);
                }
            }
            else if(this->legal_pairs[soft_i][i][j]==2){ //top
                if(vR<=fix_id_v||hL>fix_id_h){continue;}
                //cant be V:[F, S], H: [S, F]
                if(fix_id_v>vL&&x){
                    vR = std::min(vR, fix_id_v);
                }
                if(fix_id_h<hR&&!x){
                    hL = std::max(hL, fix_id_h);
                }
            }
            else if(this->legal_pairs[soft_i][i][j]==3){ //bottom
                if(vL>fix_id_v||hR<=fix_id_h){continue;}
                // cant be V: [S, F] H:[F,S]
                if(vR>fix_id_v&&x){
                    vL = std::max(vL, fix_id_v);
                }
                if(hL<fix_id_h&&!x){
                    hR = std::min(hR, fix_id_h);
                }

            }
        }
    }
    //cout<< vL<<" "<<vR<<" "<<hL<<" "<<hR<<endl;
    this->soft_seq_interval[soft_i] = {vL, vR, hL, hR};
    //this->soft_seq_interval[soft_i] = {-1, fix_n,-1, fix_n};
}

int sequence_pair_enumerator_t::sample_from_interval(int L, int R) {
    int n = R-L;
    int x = rand()%n; //x -> [0, n)
    return x+L; //[L, R)
}

void sequence_pair_enumerator_t::randomize(vector<int> &v) {
    int n = v.size();
    for(int i = 0; i<v.size(); ++i){
        int x = rand()%n;
        std::swap(v[i], v[x]);
    }
}

void sequence_pair_enumerator_t::set_only_fix() {
    this->set_fix_sequence();
    this->seq.v_sequence = fix_sequence_v;
    this->seq.h_sequence = fix_sequence_h;
    for(int i = sequence_pair_t::fix_start_idx; i<sequence_pair_t::sequence_n; ++i){
        seq.is_in_seq[i] = 1;
    }
}
