//
// Created by 林士傑 on 2023/3/9.
//

#ifndef INC_2023PD_POLYGON_H
#define INC_2023PD_POLYGON_H

#include <vector>
#include <map>
#include "polygon/polygon_module_t.h"
#include "utilities/bounding_rectangle_t.h"
#include "utilities/quadtree_t.h"
#include "utilities/box_t.h"
#include "utilities/rect_t.h"

class polygon_forest_t;

class polygon_t : public box_t{
    rect_t bounding_rect;
    quadtree_t<polygon_module_t> quadtree;
    std::vector<polygon_module_t> polygon_modules;

public:
    const rect_t& get_bounding_rect() const override;

    explicit polygon_t(const bounding_rectangle_t& first_rect);

    std::vector<polygon_module_t>& get_rects();

    bool is_bounding_collision(const rect_t& rect) const;
    bool is_bounding_collision(const bounding_rectangle_t& rect) const;

    bool is_collision(const rect_t& rect) const;
    bool is_collision(const bounding_rectangle_t& rect) const;

    bool merge_polygon(const polygon_t& polygon);
};


#endif //INC_2023PD_POLYGON_H
