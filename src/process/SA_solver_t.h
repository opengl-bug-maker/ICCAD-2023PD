//
// Created by RayChang on 2023/7/14.
//

#ifndef ICCAD2023PD_SA_SOLVER_T_H
#define ICCAD2023PD_SA_SOLVER_T_H
#include "sequence_pair_t.h"

class SA_solver_t {
public:
    SA_solver_t(int);

    bool sample_p(double delta_c);

    void run();


    sequence_pair_t sequence_pair;
    sequence_pair_t best_sequence_pair;
    int t;
};


#endif //ICCAD2023PD_SA_SOLVER_T_H
