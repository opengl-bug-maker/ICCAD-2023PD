//
// Created by RayChang on 2023/7/14.
//

#ifndef ICCAD2023PD_SA_SOLVER_T_H
#define ICCAD2023PD_SA_SOLVER_T_H
#include "sequence_pair_t.h"
#include "sequence_pair_enumerator_t.h"
class SA_solver_t {
public:
    SA_solver_t();

    bool sample_p(double delta_c);

    bool neighbor_fnd = false;

    void run(sequence_pair_enumerator_t&,double);

    double get_delta(sequence_pair_t&, sequence_pair_t&);

    sequence_pair_t find_neighbor(sequence_pair_t);
    sequence_pair_t sequence_pair;
    sequence_pair_t best_sequence_pair;
    double t, r = 0.95;
    timer run_time = timer("run time");
};


#endif //ICCAD2023PD_SA_SOLVER_T_H
