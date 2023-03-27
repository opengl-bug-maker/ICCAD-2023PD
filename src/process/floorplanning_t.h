//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_FLOORPLANNING_T_H
#define INC_2023PD_FLOORPLANNING_T_H

#include "polygon/polygon_forest_t.h"

class floorplanning_t {

public:
    polygon_forest_t& get_polygon_forest();
    int get_wire_length();
};


#endif //INC_2023PD_FLOORPLANNING_T_H
