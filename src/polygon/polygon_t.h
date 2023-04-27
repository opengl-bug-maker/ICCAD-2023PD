//
// Created by 林士傑 on 2023/3/9.
//

#ifndef INC_2023PD_POLYGON_H
#define INC_2023PD_POLYGON_H

#include <vector>
#include <map>
#include "utilities/bounding_rectangle_t.h"
#include "utilities/rect_t.h"

class polygon_forest_t;

class polygon_t {
    friend class polygon_forest_t;

    rect_t bounding_rect;
    std::vector<bounding_rectangle_t> rects;

public:

    explicit polygon_t(bounding_rectangle_t first_rect);

    std::vector<bounding_rectangle_t>& get_rects();

    bool is_bounding_collision(const rect_t& rect) const;
    bool is_bounding_collision(const bounding_rectangle_t& rect) const;

    //todo detail collision
    bool is_collision(const rect_t& rect);
    bool is_collision(const bounding_rectangle_t& rect);

    //todo merge, update bounding rect, update rects
    bool merge_polygon(const polygon_t& polygon) const;

    bool has_bounding_rect(const bounding_rectangle_t& rect) const;

    //todo cut,
    std::vector<polygon_t> cut_polygon(const bounding_rectangle_t& rect) const;
};


#endif //INC_2023PD_POLYGON_H
