//
// Created by RayChang on 2023/6/26.
//

#ifndef ICCAD2023PD_SEQUENCE_PAIR_ENUMERATOR_T_H
#define ICCAD2023PD_SEQUENCE_PAIR_ENUMERATOR_T_H

#include "sequence_pair_t.h"
#include "timer.h"
class sequence_pair_enumerator_t{

public:
    sequence_pair_enumerator_t();

    std::set<int> random_choose(int upb, int x);

    int sample_from_interval(int,int);

    void generate_sequence_pairs(int n);

    bool add_soft_process(int, bool,int);

    void validate_all_SP();

    void validate_all_SP_print_all();

    bool find_greater(sequence_pair_t&);

    vector<sequence_pair_t> valid_sequence_pairs;
    sequence_pair_t seed_SP;
    int target_sp_n = 0;
};

#endif //ICCAD2023PD_SEQUENCE_PAIR_ENUMERATOR_T_H
