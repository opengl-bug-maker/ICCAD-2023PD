//
// Created by RayChang on 2023/5/18.
//

#include "ILP_solver_t.h"
#include "utilities/vec2d_t.h"

#include <iostream>
using std::cout;
using std::endl;
ILP_solver_t::ILP_solver_t(){
}
void ILP_solver_t::init(string name, int constraints_n, int variable_n){
    this->constraints_n = constraints_n;
    this->ILP = glp_create_prob();
    glp_set_prob_name(this->ILP, name.c_str());

    glp_add_rows(this->ILP, constraints_n);
    glp_add_cols(this->ILP, variable_n);
    var_n = variable_n;

    set_i.resize(1); set_j.resize(1); set_val.resize(1); //due to 1-index :(
}
void ILP_solver_t::set_max() {
    glp_set_obj_dir(ILP, GLP_MAX);
}

void ILP_solver_t::set_min() {
    glp_set_obj_dir(ILP, GLP_MIN);
}

void ILP_solver_t::set_constraint_upb(int row_i, int variable_n, vector<int> variable_i, vector<int> values, string name,int upb) {
    for(int i = 0; i<variable_n; ++i){
        set_i.push_back(row_i);
        set_j.push_back(variable_i[i]); //due to sequence# to coefficient#
        set_val.push_back(values[i]);
        set_n++;
    }
    glp_set_row_name(ILP, constraint_i, name.c_str());
    glp_set_row_bnds(ILP, constraint_i, GLP_UP, 0, upb); //x_i-x_j<=- w

    this->constraint_i++;
}

void ILP_solver_t::set_constraint_fx(int row_i, int variable_n, vector<int> variable_i, vector<int> values, string name, int fix) {
    for(int i = 0; i<variable_n; ++i){
        set_i.push_back(row_i);
        set_j.push_back(variable_i[i]); //due to sequence# to coefficient#
        set_val.push_back(values[i]);
        set_n++;
    }
    glp_set_row_name(ILP, constraint_i, name.c_str());
    glp_set_row_bnds(ILP, constraint_i, GLP_FX, fix, fix); //x_i-x_j<=- w

    this->constraint_i++;
}

void ILP_solver_t::set_variable_double_range(int var_i, int lb, int ub) {
    //glp_set_col_bnds(ILP, i, GLP_LO, 0.0, inf);
    if(lb==ub){
        glp_set_col_bnds(ILP, var_i, GLP_FX, lb, ub);
    }
    else{
        glp_set_col_bnds(ILP, var_i, GLP_DB, lb, ub);
    }

    glp_set_col_kind(ILP, var_i, GLP_IV);
}

void ILP_solver_t::set_obj_coef(vector<int> coef) {
    for(int i = 1; i<=this->var_n; ++i){
        glp_set_obj_coef(this->ILP, i, coef[i]);
    }

}

ILP_result_t ILP_solver_t::solve() {
    glp_iocp parm;

    glp_init_iocp(&parm);
    parm.presolve = GLP_ON;
    parm.msg_lev = GLP_MSG_OFF;


    int err = glp_intopt(this->ILP, &parm);

    int z = glp_mip_obj_val(this->ILP);
    vector<int> result(1); //due to 1-index
    for(int i = 1; i<=this->var_n; ++i){
        result.push_back(glp_mip_col_val(this->ILP, i));
    }

    release_solver();
    ILP_result_t ILP_result;
    if(err!=0){
        ILP_result.legal = false;
        return ILP_result;
    }
    ILP_result.legal = true;
    ILP_result.var_values = result;
    ILP_result.z = z;


    return ILP_result;
}

void ILP_solver_t::load() {
    set_i_a = new int[set_n+1];
    set_j_a = new int[set_n+1];
    set_val_a = new double[set_n+1];
    for(int i = 1; i<=set_n; ++i){
        set_i_a[i] = this->set_i[i];
        set_j_a[i] = this->set_j[i];
        set_val_a[i] = this->set_val[i];
    }
    glp_load_matrix(this->ILP, set_n, set_i_a, set_j_a, set_val_a);
}

void ILP_solver_t::release_solver() {
    delete[] set_i_a;
    delete[] set_j_a;
    delete[] set_val_a;
    glp_delete_prob(this->ILP);
}





