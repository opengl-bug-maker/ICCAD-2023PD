//
// Created by 林士傑 on 2023/8/19.
//

#ifndef ICCAD2023PD_PUZZLE_SOLVER_H
#define ICCAD2023PD_PUZZLE_SOLVER_H

#include "bounding/bounding_line_handler_t.h"

class puzzle_solver {
    bounding_line_handler_t handler;
public:
    puzzle_solver();
    void solve();
};


#endif //ICCAD2023PD_PUZZLE_SOLVER_H
