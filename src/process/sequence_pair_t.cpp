//
// Created by RayChang on 2023/5/6.
//
#include "sequence_pair_t.h"
#include <glpk.h>
#include <iostream>
#include <string>

vector<std::vector<vec2d_t>> sequence_pair_t::soft_area_to_w_h_m;
vector<bool> sequence_pair_t::seq_is_fix;
vector<soft_module_t*> sequence_pair_t::seq_soft_map;
vector<fixed_module_t*> sequence_pair_t::seq_fixed_map;
int sequence_pair_t::sequence_n;

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

    int j = 0;
    for(int i = 0; i<soft_modules.size(); ++i){
        sequence_pair_t::seq_soft_map[j] = soft_modules[i];
        sequence_pair_t::soft_area_to_w_h_m[i] = sequence_pair_t::find_w_h( soft_modules[i]->get_area());

        sequence_pair_t::seq_fixed_map[j] = nullptr;
        sequence_pair_t::seq_is_fix[j] = false;
        j++;
    }
    for(int i = 0; i<fixed_modules.size(); ++i){
        sequence_pair_t::seq_fixed_map[j] = fixed_modules[i];
        sequence_pair_t::seq_soft_map[j] = nullptr;
        sequence_pair_t::seq_is_fix[j] = true;
        j++;
    }
    srand(time(NULL));
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

floorplanning_t sequence_pair_t::to_fp() {
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

pair<bool, vector<vec2d_t>> sequence_pair_t::ILP() {
    glp_prob *ILP = glp_create_prob();
    glp_set_prob_name(ILP, "sample");
    glp_set_obj_dir(ILP, GLP_MIN);

    int constraint_n = this->constraint_graph_h.size()+this->constraint_graph_v.size()+chip_t::get_fixed_modules().size()*2;
    int constraint_i = 1;
    glp_add_rows(ILP, constraint_n);

    int set_n = 2*(this->constraint_graph_h.size()+this->constraint_graph_v.size())+2*chip_t::get_fixed_modules().size();

    vector<int> set_i(set_n+1),set_j(set_n+1),set_val(set_n+1); //due to 1-index
    int set_cnt = 1;

    //set constraints to avoid overlap
    for(int i = 0; i<this->constraint_graph_h.size(); ++i){

        int from = constraint_graph_h[i].from, to = constraint_graph_h[i].to, w = constraint_graph_h[i].w;
        string constraint_name = "h_c"+ std::to_string(i);

        set_i[set_cnt] = constraint_i;
        set_j[set_cnt] = (from+1); //due to sequence# to coefficient#
        set_val[set_cnt] = 1;
        set_cnt++;

        set_i[set_cnt] = constraint_i;
        set_j[set_cnt] = (to+1);
        set_val[set_cnt] = -1;
        set_cnt++;

        glp_set_row_name(ILP, constraint_i, constraint_name.c_str());
        glp_set_row_bnds(ILP, constraint_i, GLP_DB, (-1)*static_cast<int>(chip_t::get_width()), -w); //x_i-x_j<=- w
        constraint_i++;
    }
    for(int i = 0; i<this->constraint_graph_v.size(); ++i){
        int from = constraint_graph_v[i].from, to = constraint_graph_v[i].to, w = constraint_graph_v[i].w;
        string constraint_name = "v_c"+ std::to_string(i);

        set_i[set_cnt] = constraint_i;
        set_j[set_cnt] = from+1+sequence_n; //due to sequence# to coefficient#
        set_val[set_cnt] = 1;
        set_cnt++;

        set_i[set_cnt] = constraint_i;
        set_j[set_cnt] = (to+1)+sequence_n;
        set_val[set_cnt] = -1;
        set_cnt++;

        glp_set_row_name(ILP, constraint_i, constraint_name.c_str());
        glp_set_row_bnds(ILP, constraint_i, GLP_DB, (-1)*static_cast<int>(chip_t::get_height()), -w); //y_i-y_j<=- w
        constraint_i++;
    }

    //set constraints to fix module
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        if(sequence_pair_t::seq_is_fix[i]){
            set_i[set_cnt] = constraint_i;
            set_j[set_cnt] = i+1; //due to sequence# to coefficient#
            set_val[set_cnt] = 1;
            set_cnt++;

            vec2d_t ll_pos = sequence_pair_t::seq_fixed_map[i]->get_left_lower();
            string x_constraint_name = "fix_x_c"+ std::to_string(i);
            glp_set_row_name(ILP, constraint_i, x_constraint_name.c_str());
            glp_set_row_bnds(ILP, constraint_i, GLP_FX, static_cast<int>(ll_pos.get_x()), static_cast<int>(ll_pos.get_x())); //x_i = k
            constraint_i++;

            set_i[set_cnt] = constraint_i;
            set_j[set_cnt] = i+1+sequence_n; //due to sequence# to coefficient#
            set_val[set_cnt] = 1;
            set_cnt++;

            string y_constraint_name = "fix_y_c"+ std::to_string(i);
            glp_set_row_name(ILP, constraint_i, y_constraint_name.c_str());
            glp_set_row_bnds(ILP, constraint_i, GLP_FX, static_cast<int>(ll_pos.get_y()), static_cast<int>(ll_pos.get_y())); //y_i = k
            constraint_i++;
        }
    }
    //set variables
    glp_add_cols(ILP, 2*sequence_pair_t::sequence_n);
    for(int i = 1;i<=sequence_pair_t::sequence_n; ++i){
        string var_name = "x"+ std::to_string(i);
        glp_set_col_name(ILP, i, var_name.c_str());
        glp_set_col_bnds(ILP, i, GLP_DB, 0.0, chip_t::get_width()-this->modules_wh[h_sequence[i-1]].get_x());
        glp_set_col_kind(ILP, i, GLP_IV);
    }
    for(int i = sequence_pair_t::sequence_n+1;i<=2*sequence_pair_t::sequence_n; ++i){
        string var_name = "x"+ std::to_string(i);
        glp_set_col_name(ILP, i, var_name.c_str());
        glp_set_col_bnds(ILP, i, GLP_DB, 0.0, chip_t::get_height()-this->modules_wh[v_sequence[i-1-sequence_pair_t::sequence_n]].get_y());
        glp_set_col_kind(ILP, i, GLP_IV);
    }

    int* set_i_a = new int[set_n+1];
    int* set_j_a = new int[set_n+1];
    double* set_val_a = new double[set_n+1];
    for(int i = 1; i<=set_n; ++i){
        set_i_a[i] = set_i[i];
        set_j_a[i] = set_j[i];
        set_val_a[i] = set_val[i];
    }

    glp_load_matrix(ILP, set_n, set_i_a, set_j_a, set_val_a);

    glp_iocp parm;
    glp_init_iocp(&parm);
    parm.presolve = GLP_ON;
    int err = glp_intopt(ILP, &parm);
    vector<vec2d_t> result;
//    for(int i = 1; i<=sequence_n; ++i){
//        cout<< "seq# "<<i-1<<":";
//        cout<<"{"<<glp_mip_col_val(ILP, i)<<" "<<glp_mip_col_val(ILP, i+sequence_n)<<"} "<<endl;
//    }
//    cout<<endl;
    for(int i = 1; i<=sequence_n; ++i){
        result.emplace_back(glp_mip_col_val(ILP, i), glp_mip_col_val(ILP, i+sequence_n));
    }
    delete[] set_i_a;
    delete[] set_j_a;
    delete[] set_val_a;
    glp_delete_prob(ILP);
    if(err==0){return {true, result};}
    else{return {false, {}};}

}

void sequence_pair_t::seq_randomize() {
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        int x = rand()%sequence_pair_t::sequence_n;
        std::swap(this->v_sequence[i], this->v_sequence[x]);
    }
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        int x = rand()%sequence_pair_t::sequence_n;
        std::swap(this->h_sequence[i], this->h_sequence[x]);
    }
}

