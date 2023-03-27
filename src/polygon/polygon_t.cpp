//
// Created by 林士傑 on 2023/3/9.
//

#include "polygon_t.h"

int polygon_t::getSize() {
    return sumTable.back().back();
}

bool polygon_t::isCollision(rect_t &rect) {

    return false;
}

void polygon_t::addRect(rect_t &rect) {

}

void polygon_t::addRects(std::vector<rect_t> &rects) {

}

void polygon_t::addRects(polygon_t &polygon) {

}

void polygon_t::refresh() {
    for (int i = 1; i < diffTable.size(); ++i) {
        for (int j = 1; j < diffTable[i].size(); ++j) {
            normalTable[i][j] = diffTable[i][j] + diffTable[i - 1][j] + diffTable[i][j - 1] - diffTable[i - 1][j - 1];
        }
    }
    for (int i = 1; i < diffTable.size(); ++i) {
        for (int j = 1; j < diffTable.size(); ++j) {
            sumTable[i][j] = normalTable[i][j] + normalTable[i - 1][j] + normalTable[i][j - 1] - normalTable[i - 1][j - 1];
        }
    }
}
