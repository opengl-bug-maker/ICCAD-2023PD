#include <vector>
#pragma once
class sequence_pair_t;
class sp_ilp_settings_t{
public:
    sequence_pair_t* sp;
    sp_ilp_settings_t(sequence_pair_t*);
    //subfunction for LP
    void set_constraints_modules_overlap_h();
    void set_constraints_modules_overlap_v();
    void set_constraints_modules_fixed();
    void set_constraints_net_direction();
    void set_constraints_net();
    void set_constraints_ratio_equal_1();
    void set_constraints_boundaries();
    void set_variables_modules();
    void set_variables_connections();
    void set_variables_shapes();
    void set_variables_hands();
    void set_coef(std::vector<double>&);
    void set_constraints_opt_nets();
    void set_constraints_opt_modules();
    void set_constraints_only_one_hand();
    
};