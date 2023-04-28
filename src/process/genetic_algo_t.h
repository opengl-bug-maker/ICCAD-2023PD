//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_GENETIC_ALGO_T_H
#define INC_2023PD_GENETIC_ALGO_T_H

#include "polygon/polygon_forest_t.h"
#include "process/floorplanning_t.h"
#include "static_data/chip_t.h"
#include "quadratic/quadratic_t.h"
#include <map>
#include <set>
#include <algorithm>
#include <iterator>
#include <random>
#include<time.h>
class genetic_algo_t {
    void growing(floorplanning_t& );
    void crossover_process();

    //floorplanning_t mutation(const floorplanning_t&);
    void selection();
    void evaluate(const floorplanning_t&);
    void update_best_fp();
    vector<floorplanning_t> floorplannings;
    size_t floorplanning_n = 2000;
    size_t greater_floorplanning_n = 200;
    const static quadratic_t quadratic_calculator;
    const int growing_rate = 3;
public:
    floorplanning_t mutation(const floorplanning_t&);
    floorplanning_t crossover(const floorplanning_t& fp1, const floorplanning_t& fp2);
    genetic_algo_t();
    void run();
    void print_info(bool);
    floorplanning_t get_fp(size_t);
    floorplanning_t& get_best_fp();
};


#endif //INC_2023PD_GENETIC_ALGO_T_H
