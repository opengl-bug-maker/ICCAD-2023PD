//
// Created by 林士傑 on 2023/6/9.
//

#ifndef ICCAD2023PD_LENF_PF_T_H
#define ICCAD2023PD_LENF_PF_T_H

#include <vector>
#include <memory>
#include "utilities/quadtree_t.h"
#include "utilities/bounding_rectangle_t.h"
#include "polygon/polygon_t.h"


class lenf_pf_t {
    quadtree_t<polygon_t> quadtree;
    std::vector<std::shared_ptr<polygon_t>> polygons;
    void test_without_collision();
public:
    lenf_pf_t();

    std::vector<polygon_t> get_polygons();

    bool test_collision(const rect_t& rect);

    bool add_rect(const bounding_rectangle_t& boundingRectangle);

    std::vector<rect_t> get_empty_spaces();
};


#endif //ICCAD2023PD_LENF_PF_T_H
