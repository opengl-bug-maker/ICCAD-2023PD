//
// Created by RayChang on 2023/5/18.
//

#ifndef ICCAD2023PD_ILP_SOLVER_T_H
#define ICCAD2023PD_ILP_SOLVER_T_H
#include <glpk.h>
#include <string>
#include <vector>
#include "edge_t.h"
using std::string;
using std::vector;
class ILP_result_t{
public:
    bool legal;
    int z;
    vector<int> var_values;
    ILP_result_t(){
        var_values.resize(1);
    }
};
class ILP_solver_t {
    int* set_i_a;
    int* set_j_a;
    double* set_val_a;
    bool invalid_input = false;
public:
    int set_n = 0,set_id;
    vector<int> set_i,set_j,set_val; //due to 1-index
    glp_prob *ILP;
    int constraints_n;
    int constraint_i = 1;
    int var_n;
    ILP_solver_t();
    void init(string name, int constraints_n, int variable_n);
    void set_max();
    void set_min();

    void set_constraint_upb(int row_i, int variable_n, vector<int> variable_i, vector<int> values,string,int);
    void set_constraint_fx(int row_i, int variable_n, vector<int>variable_i,vector<int> values, string,int);
    void set_variable_double_range(int var_i, int lb, int ub);
    void set_obj_coef(vector<int>);
    void release_solver();
    ILP_result_t solve();
    void load();

    bool get_is_invalid();

    void set_variable_BV(int var_i);
};


#endif //ICCAD2023PD_ILP_SOLVER_T_H
