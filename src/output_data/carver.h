//
// Created by 林士傑 on 2023/8/28.
//

#ifndef ICCAD2023PD_CARVER_H
#define ICCAD2023PD_CARVER_H

#include "polygon/polygon_forest_t.h"

class carver {
    polygon_forest_t polygon_forest;
    std::vector<rect_t> rects;
    int push_test(int index, rect_t rect);
    bool push(int index, int dir);
public:
    polygon_forest_t carving(polygon_forest_t& polygon_forest);
    polygon_forest_t pushing(polygon_forest_t& polygon_forest);
};


#endif //ICCAD2023PD_CARVER_H
