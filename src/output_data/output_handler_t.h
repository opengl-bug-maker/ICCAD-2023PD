//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_OUTPUT_HANDLER_T_H
#define INC_2023PD_OUTPUT_HANDLER_T_H

#include <vector>
#include "process/floorplanning_t.h"
#include "output_data/output_utility_t.h"

class output_handler_t {
//    static fp
public:
    static void set_FP(floorplanning_t& floorplanning);
    static int WHPL();
    static std::vector<output_utility_t> to_real();
};


#endif //INC_2023PD_OUTPUT_HANDLER_T_H
