//
// Created by RayChang on 2023/6/26.
//

#ifndef ICCAD2023PD_SEQUENCE_PAIR_ENUMERATOR_T_H
#define ICCAD2023PD_SEQUENCE_PAIR_ENUMERATOR_T_H

#include "sequence_pair_t.h"

class sequence_pair_enumerator_t{
    int fix_n = 0;
    int soft_n = 0;

public:
    sequence_pair_enumerator_t();

    void seq_randomize();
    void find_illegal_pair_for_i(int);
    void search_legal_perm_in_fix(int);
    void randomize(vector<int>&);
    std::set<int> random_choose(int upb, int x);
    int sample_from_interval(int,int);

    vector<vector<int>> soft_seq_interval;
    vector<vector<vector<int>>> legal_pairs; // 0->left, 1->right, 2 ->upper, 3->bottom //soft->fix  illegal
    vector<int> fix_sequence_v, fix_sequence_h;
    sequence_pair_t seq;
};

#endif //ICCAD2023PD_SEQUENCE_PAIR_ENUMERATOR_T_H
