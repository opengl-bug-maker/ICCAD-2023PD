//
// Created by RayChang on 2023/5/6.
//
#include "sequence_pair_t.h"
#include <glpk.h>
#include <iostream>
#include <string>
#include <plugin/visualizer_t.h>
#include <chrono>

vector<std::vector<vec2d_t>> sequence_pair_t::soft_area_to_w_h_m;
vector<bool> sequence_pair_t::seq_is_fix;
vector<soft_module_t*> sequence_pair_t::seq_soft_map;
vector<fixed_module_t*> sequence_pair_t::seq_fixed_map;
int sequence_pair_t::sequence_n;
unordered_map<const module_t*, int> sequence_pair_t::soft_module_to_id_m;
unordered_map<const module_t*, int> sequence_pair_t::fix_module_to_id_m;
vector<edge_t> sequence_pair_t::connections;
vector<vector<int>> sequence_pair_t::connectivities;
using std::cout;
using std::endl;
using std::string;
void sequence_pair_t::init() {
    const std::vector<soft_module_t*>& soft_modules = chip_t::get_soft_modules();
    const std::vector<fixed_module_t*>& fixed_modules = chip_t::get_fixed_modules();
    sequence_pair_t::sequence_n = chip_t::get_total_module_n();

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
    srand(time(NULL));

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

    //check connections
//    for(auto& e:connections){
//        cout<<"from "<<e.from<<" to"<<e.to<<", w : "<<e.w<<endl;
//    }
}

sequence_pair_t::sequence_pair_t() {
    v_sequence.resize(chip_t::get_total_module_n());
    h_sequence.resize(chip_t::get_total_module_n());


    modules_wh.resize(chip_t::get_total_module_n());

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
    }

    //set sequence to [1, n] by default
    for(int i = 0; i<v_sequence.size(); ++i){
        h_sequence[i] = v_sequence[i] = i;
    }
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
    if(area==1){return {{1,1}};}
    uint32_t  from = sqrt(area/2);
    vector<vec2d_t> ret;
    for(uint32_t i = from; i<= sqrt(area); ++i){ //i will be the short edge
        if(area%i==0){
            int x = i, y = area/i;
            if(x*2>=y){
                ret.push_back({x, y});
                if(x!=y){
                    ret.push_back({y, x});
                }

            }
        }
    }
    if(ret.size()==0){
        int x = ceil(sqrt(area));
        return {{x,x}};
    }
    return ret;
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

bool sequence_pair_t::to_fp() {
    build_constraint_graph();
    bool success = false;
    //pair<bool, vector<vec2d_t>> res = this->ILP_process();
    pair<bool, vector<vec2d_t>> presolve_res = this->ILP();

    if(presolve_res.first==true){
        this->sequence_pair_validation(presolve_res);
//        pair<bool, vector<vec2d_t>> final_res =this->ILP_relaxation(presolve_res.second);
//        this->sequence_pair_validation(final_res);
        fgetc(stdin);
        return true;
    }
    else{
        //std::cout <<"fail in "<<elapsed.count() << " ms\n";
        return false;
    }

}


void sequence_pair_t::build_constraint_graph() {
    this->constraint_graph_h.clear();
    this->constraint_graph_v.clear();
    //TODO: accelerate by inverse pair (merge sort)
    vector<int> v_map(sequence_pair_t::sequence_n),h_map(sequence_pair_t::sequence_n);
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        v_map[v_sequence[i]] = i;
        h_map[h_sequence[i]] = i;
    }
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        for(int j = 0; j<sequence_pair_t::sequence_n; ++j) {
            if(v_map[i]<v_map[j] && h_map[i]<h_map[j]){ //{a, b}, {a, b}
                this->constraint_graph_h.push_back({i, j, static_cast<int>(this->modules_wh[i].get_x())});
            }
            else if(v_map[i]<v_map[j] && h_map[j]<h_map[i]) { //{a, b}, {b,a}
                this->constraint_graph_v.push_back({i, j, static_cast<int>(this->modules_wh[i].get_y())});
            }
        }
    }
    cout<<"horizontal constraint graph : "<<endl;
    for(auto& e:constraint_graph_h){
        cout<< "{"<<e.from<<"->"<<e.to<<", "<<e.w<<"}"<<endl;
    }
    cout<<"vertical constraint graph : "<<endl;
    for(auto& e:constraint_graph_v){
        cout<< "{"<<e.from<<"->"<<e.to<<", "<<e.w<<"}"<<endl;
    }
}
pair<bool, vector<vec2d_t>> sequence_pair_t::ILP() {
    int constraint_n = this->constraint_graph_h.size()+this->constraint_graph_v.size()+chip_t::get_fixed_modules().size()*2;

    ILP_solver_t ILP_solver;
    ILP_solver.init("ILP_solver", constraint_n, 2*sequence_pair_t::sequence_n);
    ILP_solver.set_min();
    int constraint_i = 1;
    int set_n = 2*(this->constraint_graph_h.size()+this->constraint_graph_v.size())+2*chip_t::get_fixed_modules().size();

    vector<int> set_i(set_n+1),set_j(set_n+1),set_val(set_n+1); //due to 1-index
    vector<int> coef_h(sequence_n,0), coef_v(sequence_n,0),coef(2*sequence_n+1);


    //set constraints to avoid overlap
    for(int i = 0; i<this->constraint_graph_h.size(); ++i){
        int from = constraint_graph_h[i].from, to = constraint_graph_h[i].to, w = constraint_graph_h[i].w;
        coef_h[from]++; coef_h[to]--;
        string constraint_name = "h_c"+ std::to_string(i);
        ILP_solver.set_constraint_upb(constraint_i, 2, {from+1, to+1}, {1, -1}, constraint_name, -w+overlap_v);
        constraint_i++;
    }
    for(int i = 0; i<this->constraint_graph_v.size(); ++i){
        int from = constraint_graph_v[i].from, to = constraint_graph_v[i].to, w = constraint_graph_v[i].w;
        coef_v[from]++; coef_v[to]--;
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
    for(int i = 1;i<=sequence_pair_t::sequence_n; ++i){
        string var_name = "x"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, i, var_name.c_str());
        ILP_solver.set_variable_double_range(i, 0.0,chip_t::get_width()-this->modules_wh[i-1].get_x());
    }
    for(int i = sequence_pair_t::sequence_n+1;i<=2*sequence_pair_t::sequence_n; ++i){
        string var_name = "x"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, i, var_name.c_str());
        ILP_solver.set_variable_double_range(i, 0.0, chip_t::get_height()-this->modules_wh[ i-1-sequence_n].get_y());
    }
    //set coefficients
//    for(int i = 0; i<sequence_n; ++i){
//        coef_h[i] = coef_v[i] = 0;
//    }
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
        result.emplace_back(ILP_result.var_values[i], ILP_result.var_values[i+sequence_n]);
    }
    if(ILP_result.legal){
        return {true, result};
    }
    else{return {false, {}};}
}
//pair<bool, vector<vec2d_t>> sequence_pair_t::ILP_relaxation(vector<vec2d_t> ILP_presolve_result){
//
//    int constraint_n = this->constraint_graph_h.size()+this->constraint_graph_v.size()+chip_t::get_fixed_modules().size()*2;
//
//    ILP_solver_t ILP_solver;
//    ILP_solver.init("ILP_solver", constraint_n, 2*sequence_pair_t::sequence_n);
//    ILP_solver.set_min();
//    int constraint_i = 1;
//    int set_n = 2*(this->constraint_graph_h.size()+this->constraint_graph_v.size())+2*chip_t::get_fixed_modules().size();
//
//    vector<int> set_i(set_n+1),set_j(set_n+1),set_val(set_n+1); //due to 1-index
//    vector<int> coef_h(sequence_n,0), coef_v(sequence_n,0),coef(2*sequence_n+1);
//
//    int overlap_v = 2;
//    //set constraints to avoid overlap
//    for(int i = 0; i<this->constraint_graph_h.size(); ++i){
//        int from = constraint_graph_h[i].from, to = constraint_graph_h[i].to, w = constraint_graph_h[i].w;
//        string constraint_name = "h_c"+ std::to_string(i);
//        ILP_solver.set_constraint_upb(constraint_i, 2, {from+1, to+1}, {1, -1}, constraint_name, -w+overlap_v);
//        constraint_i++;
//    }
//    for(int i = 0; i<this->constraint_graph_v.size(); ++i){
//        int from = constraint_graph_v[i].from, to = constraint_graph_v[i].to, w = constraint_graph_v[i].w;
//        string constraint_name = "v_c"+ std::to_string(i);
//        ILP_solver.set_constraint_upb(constraint_i, 2, {from+1+sequence_n, to+1+sequence_n}, {1, -1}, constraint_name, -w+overlap_v);
//        constraint_i++;
//    }
//    //set constraints to fix module
//    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
//        if(sequence_pair_t::seq_is_fix[i]){
//            string x_constraint_name = "fix_x_c"+ std::to_string(i);
//            string y_constraint_name = "fix_y_c"+ std::to_string(i);
//            vec2d_t ll_pos = sequence_pair_t::seq_fixed_map[i]->get_left_lower();
//            ILP_solver.set_constraint_fx(constraint_i, 1, {i+1}, {1}, x_constraint_name, static_cast<int>(ll_pos.get_x()));
//            constraint_i++;
//            ILP_solver.set_constraint_fx(constraint_i, 1, {i+1+sequence_n}, {1}, y_constraint_name, static_cast<int>(ll_pos.get_y()));
//            constraint_i++;
//        }
//    }
//    //set variables
//    for(int i = 1;i<=sequence_pair_t::sequence_n; ++i){
//        string var_name = "x"+ std::to_string(i);
//        glp_set_col_name(ILP_solver.ILP, i, var_name.c_str());
//        ILP_solver.set_variable_double_range(i, 0.0,chip_t::get_width()-this->modules_wh[i-1].get_x());
//    }
//    for(int i = sequence_pair_t::sequence_n+1;i<=2*sequence_pair_t::sequence_n; ++i){
//        string var_name = "x"+ std::to_string(i);
//        glp_set_col_name(ILP_solver.ILP, i, var_name.c_str());
//        ILP_solver.set_variable_double_range(i, 0.0, chip_t::get_height()-this->modules_wh[ i-1-sequence_n].get_y());
//    }
//    //set coefficients
//    for(int i = 0; i<this->constraint_graph_h.size(); ++i){
//        int from = constraint_graph_h[i].from, to = constraint_graph_h[i].to, w = constraint_graph_h[i].w;
//        if(ILP_presolve_result[from].get_x()+w>ILP_presolve_result[to].get_x()){
//            cout<<"overlap : "<<from<<" "<<to<<endl;
//            coef_h[from]++;
//            coef_h[to]--;
//        }
//    }
//    for(int i = 0; i<this->constraint_graph_v.size(); ++i){
//        int from = constraint_graph_v[i].from, to = constraint_graph_v[i].to, w = constraint_graph_h[i].w;
//        if(ILP_presolve_result[from].get_y()+w>ILP_presolve_result[to].get_y()){
//            cout<<"overlap : "<<from<<" "<<to<<endl;
//            coef_v[from]++;
//            coef_v[to]--;
//        }
//    }
//    //prepare coefficients
//    for(int i = 1; i<2*sequence_n; ++i){
//        if(i<sequence_n){
//            coef[i] = coef_h[i-1];
//        }
//        else{
//            coef[i] = coef_v[i-sequence_n-1];
//        }
//    }
//    //solve
//    ILP_solver.set_obj_coef(coef);
//    ILP_solver.load();
//    ILP_result_t ILP_result = ILP_solver.solve();
//    vector<vec2d_t> result;
//    cout<< "z : "<< ILP_result.z<<endl;
//    for(int i = 1; i<=sequence_n; ++i){
//        result.emplace_back(ILP_result.var_values[i], ILP_result.var_values[i+sequence_n]);
//    }
//    if(ILP_result.legal){
//        return {true, result};
//    }
//    else{return {false, {}};}
//}
void sequence_pair_t::seq_randomize() {
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        int x = rand()%sequence_pair_t::sequence_n;
        std::swap(this->v_sequence[i], this->v_sequence[x]);
    }
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        int x = rand()%sequence_pair_t::sequence_n;
        std::swap(this->h_sequence[i], this->h_sequence[x]);
    }
    //this->set_fix_sequence();
}

void sequence_pair_t::set_fix_sequence() {
    vector<int> v_map(sequence_pair_t::sequence_n),h_map(sequence_pair_t::sequence_n);
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        v_map[v_sequence[i]] = i;
        h_map[h_sequence[i]] = i;
    }
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        for(int j = 0; j<i; ++j){ //j<i
            if(this->seq_is_fix[i]==0||this->seq_is_fix[j]==0){continue;}
            int x_i = static_cast<int>(sequence_pair_t::seq_fixed_map[i]->get_left_lower().get_x());
            int x_j = static_cast<int>(sequence_pair_t::seq_fixed_map[j]->get_left_lower().get_x());
            int y_i = static_cast<int>(sequence_pair_t::seq_fixed_map[i]->get_left_lower().get_y());
            int y_j = static_cast<int>(sequence_pair_t::seq_fixed_map[j]->get_left_lower().get_y());
            if(x_i<x_j && h_map[i]<h_map[j]&&v_map[i]<v_map[j]){ //i is left to j -> has to be <i, j>, <i, j>
                std::swap(h_sequence[h_map[i]], h_sequence[h_map[j]]);
            }
            if(y_i<y_j && h_map[i]>h_map[j]&&v_map[i]<v_map[j]){ //i is below to j -> has to be <j, i>, <i, j>
                std::swap(h_sequence[v_map[i]], h_sequence[v_map[j]]);
            }
        }
    }
}

void sequence_pair_t::sequence_pair_validation(pair<bool, vector<vec2d_t>> res) {
    vector<std::pair<rect_t, std::string>> rects;
    for(int i = 0; i<res.second.size(); ++i){
        if(this->seq_is_fix[i]){
            rects.push_back({{res.second[i], this->modules_wh[i]}, "f"+ std::to_string(i)});
        }
        else{
            rects.push_back({{res.second[i], this->modules_wh[i]}, "s"+std::to_string(i)});
            //this->fp.place_soft_module(i, {res.second[i]}, {this->modules_wh[i]});
        }
    }
    visualizer_t::show_fp_rect_no_border(rects);
    //fgetc(stdin);
}

pair<bool, vector<vec2d_t>> sequence_pair_t::ILP_process() {
//    pair<bool, vector<vec2d_t>> presolve_result = ILP();
//    if(presolve_result.first==false){
//        return {false, {}};
//    }
//    else{
//        pair<bool, vector<vec2d_t>> final_result = ILP_relaxation(presolve_result.second);
//        return presolve_result;
//    }
    return {false, {}};
}

