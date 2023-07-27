//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_GENETIC_ALGO_T_H
#define INC_2023PD_GENETIC_ALGO_T_H

#include "polygon/polygon_forest_t.h"
#include "process/floorplan_t.h"
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

public:
    //essential functions
    solver_t();

    floorplan_t& get_best_fp();

    void SA_process();

    void run();

    double get_time_left(); //ms

    void set_timer();

    double runtime = 2*60*1000; // ms
    timer runtime_timer = timer("run time");
    floorplan_t best_fp;
};


#endif //INC_2023PD_GENETIC_ALGO_T_H
