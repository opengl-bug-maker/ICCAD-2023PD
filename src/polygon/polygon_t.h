//
// Created by 林士傑 on 2023/3/9.
//

#ifndef INC_2023PD_POLYGON_H
#define INC_2023PD_POLYGON_H

#include <vector>
#include <map>
#include "utilities/rect_t.h"

class polygon_t {
    std::vector<rect_t> rects;

    std::vector<int> positionIndexX;
    std::vector<int> positionIndexY;

    std::vector<std::vector<int>> diffTable;
    std::vector<std::vector<int>> normalTable;
    std::vector<std::vector<int>> sumTable;


public:
    int getSize();

    bool isCollision(rect_t& rect);

    void addRect(rect_t& rect);

    void addRects(std::vector<rect_t>& rects);

    void addRects(polygon_t& polygon);

    void refresh();
};


#endif //INC_2023PD_POLYGON_H
