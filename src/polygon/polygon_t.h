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

    std::vector<int> positionIndexX;
    std::vector<int> positionIndexY;

    std::vector<std::vector<int>> diffTable;
    std::vector<std::vector<int>> normalTable;
    std::vector<std::vector<int>> sumTable;

    explicit polygon_t(bounding_rectangle_t first_rect);

public:

    int getSize();

    bool is_bounding_collision(bounding_rectangle_t& rect) const;

    bool has_bounding_rect(const bounding_rectangle_t& rect) const;

    //todo detail collision
    bool is_collision(bounding_rectangle_t& rect);

    //todo merge, update bounding rect, update rects
    bool merge_polygon(const polygon_t& polygon) const;

    //todo cut,
    std::vector<polygon_t> cut_polygon(const bounding_rectangle_t& rect) const;

    void addRect(rect_t& rect);

    void addRects(std::vector<rect_t>& rects);

    void addRects(polygon_t& polygon);

    void refresh();
};


#endif //INC_2023PD_POLYGON_H
