//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_POLYGON_FOREST_T_H
#define INC_2023PD_POLYGON_FOREST_T_H

#include <vector>
#include "utilities/bounding_rectangle_t.h"
#include "polygon/polygon_t.h"

class polygon_forest_t {
    std::vector<polygon_t> polygons;
    void test_without_collision();
public:
    bool add_rect(bounding_rectangle_t& boundingRectangle);
    void delete_rect(bounding_rectangle_t& boundingRectangle);
};


#endif //INC_2023PD_POLYGON_FOREST_T_H
