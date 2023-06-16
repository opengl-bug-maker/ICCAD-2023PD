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
    srand(time(NULL));

    const std::vector<soft_module_t*>& soft_modules = chip_t::get_soft_modules();
    const std::vector<fixed_module_t*>& fixed_modules = chip_t::get_fixed_modules();
    sequence_pair_t::sequence_n = static_cast<int>(chip_t::get_total_module_n());

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
    max_overlap = static_cast<int>(static_cast<float>(std::max(chip_t::get_height(), chip_t::get_width()))*0.5);
    max_distance = static_cast<int>(static_cast<float>(std::max(chip_t::get_height(), chip_t::get_width()))*0,2);
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
        //cout<< area<<" "<<i<<" "<<ceil(static_cast<float>(area)/static_cast<float>(i))<<endl;
        int x = i, y = ceil(static_cast<float>(area)/static_cast<float>(i));
        if(x*2>=y){
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
    pair<bool, vector<vec2d_t>> res;
    int overlap_v = 0, overlap_h = 0;
    for(int i = 0; i<=max_overlap; ++i){
        pair<bool, vector<vec2d_t>> presolve_res = this->find_position(overlap_h, overlap_v);
        if(presolve_res.first){
            res = presolve_res;
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
    floorplan_t fp;
    if(success){
        cout<< "SUCCESS"<<endl;
        //this->sequence_pair_validation(res.second);
        pair<bool, floorplan_t> valid_fp = this->place_all_modules(res.second);
        return valid_fp;
    }
    else{
        return {false, fp};
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
//    cout<<"horizontal constraint graph : "<<endl;
//    for(auto& e:constraint_graph_h){
//        cout<< "{"<<e.from<<"->"<<e.to<<", "<<e.w<<"}"<<endl;
//    }
//    cout<<"vertical constraint graph : "<<endl;
//    for(auto& e:constraint_graph_v){
//        cout<< "{"<<e.from<<"->"<<e.to<<", "<<e.w<<"}"<<endl;
//    }
}
pair<bool, vector<vec2d_t>> sequence_pair_t::find_position(int overlap_h, int overlap_v) {
    int constraint_n = this->constraint_graph_h.size()+this->constraint_graph_v.size()+chip_t::get_fixed_modules().size()*2;
    int constraint_i = 1; //constraint_counter
    int set_n = 2*(this->constraint_graph_h.size()+this->constraint_graph_v.size())+2*chip_t::get_fixed_modules().size();
    //apart from constraint graph, every fix module need 2 additional condition
    ILP_solver_t ILP_solver;
    ILP_solver.init("ILP_solver", constraint_n, 2*sequence_pair_t::sequence_n);
    ILP_solver.set_min();


    vector<int> set_i(set_n+1),set_j(set_n+1),set_val(set_n+1); //due to 1-index
    vector<int> coef_h(sequence_n,0), coef_v(sequence_n,0),coef(2*sequence_n+1);


    //set constraints to place modules
    for(int i = 0; i<this->constraint_graph_h.size(); ++i){
        int from = constraint_graph_h[i].from, to = constraint_graph_h[i].to, w = constraint_graph_h[i].w;
        coef_h[from]++; coef_h[to]--;
        string constraint_name = "h_c"+ std::to_string(i);
        ILP_solver.set_constraint_upb(constraint_i, 2, {from+1, to+1}, {1, -1}, constraint_name, -w+overlap_h);
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

void sequence_pair_t::sequence_pair_validation(vector<vec2d_t> res) {
    vector<std::pair<rect_t, std::string>> rects;
    for(int i = 0; i<res.size(); ++i){
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
    cout<<"[";
    for(int i = 0; i<this->v_sequence.size(); ++i){
        cout<< v_sequence[i];
        if(i!=this->v_sequence.size()-1){cout<<", ";}
    }
    cout<<"] ";
    cout<<"[";
    for(int i = 0; i<this->h_sequence.size(); ++i){
        cout<< h_sequence[i];
        if(i!=this->h_sequence.size()-1){cout<<", ";}
    }

    cout<<"]"<<endl;
}

void sequence_pair_t::print_shapes() {
    for(int i = 0; i<sequence_n; ++i){
        cout<< "i: "<< i<<" "<<modules_wh[i].get_x()<<" "<<modules_wh[i].get_y()<<endl;
    }
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
    this->seed.resize(sequence_pair_t::sequence_n);
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        seed[i] = i;
    }
    seed_need_n = 0;
}

void sequence_pair_enumerator_t::generate_valid_seq(int x) {
    int continuous_v_fail = 0;
    int continuous_h_fail = 0;
    const int tolerance_v = 5;
    const int jump_distance = 50;

    this->valid_seq.clear();
    for(int i = 0; i<seeds.size(); ++i){
        bool continuous_fail = false;
        for(int j = 0; j<seeds.size(); ++j){
            seq.set_h(seeds[i]);seq.set_v(seeds[j]);
            pair<bool,floorplan_t> success_fp = seq.get_fp(); //try if the sequence pair could be a legal fp
            if(success_fp.first==false){continue;}

            this->valid_seq.push_back(seq);
            if(valid_seq.size()>=x){return;}

            if(success_fp.first==false){continuous_v_fail++;}

//            if(continuous_v_fail==tolerance_v){
//                continuous_fail = true;
//                break;
//            }
        }
        continuous_v_fail = 0;
//        if(continuous_fail){
//            continuous_h_fail++;
//        }
//        if(continuous_h_fail){
//            continuous_h_fail++;
//            i+=jump_distance;
//        }
    }
}

void sequence_pair_enumerator_t::print_all_valid_seq() {
    for(auto& valid_sequence:this->valid_seq){
        valid_sequence.print();
    }
}

vector<sequence_pair_t> sequence_pair_enumerator_t::get_all_valid_seq() {
    return this->valid_seq;
}

void sequence_pair_enumerator_t::change_size() {

    for(int i = 0; i<chip_t::get_total_module_n(); ++i){
        if(sequence_pair_t::seq_is_fix[i]){
            seq.modules_wh[i] = sequence_pair_t::seq_fixed_map[i]->get_size();
        }
        else{
            vector<vec2d_t> shapes = seq.find_w_h(sequence_pair_t::seq_soft_map[i]->get_area());
            int id = static_cast<int>(rand())%shapes.size();
            seq.modules_wh[i] = shapes[id];
        }
    }
}

void sequence_pair_enumerator_t::seq_randomize() {
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        int x = rand()%sequence_pair_t::sequence_n;
        seq.swap_h(i, x);
    }
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        int x = rand()%sequence_pair_t::sequence_n;
        seq.swap_v(i, x);
    }
}


