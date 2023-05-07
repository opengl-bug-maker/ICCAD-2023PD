//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_GENETIC_ALGO_T_H
#define INC_2023PD_GENETIC_ALGO_T_H

#include "polygon/polygon_forest_t.h"
#include "process/floorplanning_t.h"
#include "static_data/chip_t.h"
#include "quadratic/quadratic_t.h"
#include "sequence_pair_t.h"
#include <map>
#include <set>
#include <algorithm>
#include <iterator>
#include <random>
#include <time.h>

class solver_t {
    sequence_pair_t sequence_pair;
    vector<floorplanning_t> floorplannings;
    floorplanning_t best_fp;
public:
    void generate_seq_rand();
    //essential functions
    solver_t();
    void run();
    void print_info(bool);
    floorplanning_t get_fp(size_t);
    floorplanning_t& get_best_fp();
};


#endif //INC_2023PD_GENETIC_ALGO_T_H
