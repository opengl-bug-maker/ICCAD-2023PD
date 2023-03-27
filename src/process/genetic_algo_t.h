//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_GENETIC_ALGO_T_H
#define INC_2023PD_GENETIC_ALGO_T_H

#include "polygon/polygon_forest_t.h"
#include "process/floorplanning_t.h"
#include "static_data/chip_t.h"


class genetic_algo_t {
public:
    explicit genetic_algo_t(chip_t& chip);
    void run();
    floorplanning_t& get_best_fp();
};


#endif //INC_2023PD_GENETIC_ALGO_T_H
