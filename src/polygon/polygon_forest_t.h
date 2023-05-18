//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_POLYGON_FOREST_T_H
#define INC_2023PD_POLYGON_FOREST_T_H

#include <vector>
#include "utilities/quadtree_t.h"
#include "utilities/bounding_rectangle_t.h"
#include "polygon/polygon_t.h"
#include <memory>

class polygon_forest_t {
    quadtree_t<polygon_t> quadtree;
    std::vector<std::shared_ptr<polygon_t>> polygons;
    void test_without_collision();
public:
    polygon_forest_t();

    std::vector<polygon_t> get_polygons();

    bool test_collision(const rect_t& rect);

    bool add_rect(const bounding_rectangle_t& boundingRectangle);

    std::vector<rect_t> get_empty_spaces();
};


#endif //INC_2023PD_POLYGON_FOREST_T_H
