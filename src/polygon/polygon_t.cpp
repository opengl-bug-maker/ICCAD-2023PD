//
// Created by 林士傑 on 2023/3/9.
//

#include "polygon_t.h"

polygon_t::polygon_t(bounding_rectangle_t rect) : bounding_rect(rect.getRect()) {
    this->rects.push_back(rect);
}

int polygon_t::getSize() {
    return sumTable.back().back();
}

bool polygon_t::is_bounding_collision(bounding_rectangle_t &rect) const {
    double x_dis0 = this->bounding_rect.get_left_lower().get_x() - rect.getRect().get_right_upper().get_x();
    double x_dis1 = this->bounding_rect.get_right_upper().get_x() - rect.getRect().get_left_lower().get_x();
    double x_length = this->bounding_rect.get_size().get_x() + rect.getRect().get_size().get_x();

    double y_dis0 = this->bounding_rect.get_left_lower().get_y() - rect.getRect().get_right_upper().get_y();
    double y_dis1 = this->bounding_rect.get_right_upper().get_y() - rect.getRect().get_left_lower().get_y();
    double y_length = this->bounding_rect.get_size().get_y() + rect.getRect().get_size().get_y();

    return
            fabs(x_dis0) < x_length &&
            fabs(x_dis1) < x_length &&
            fabs(y_dis0) < y_length &&
            fabs(y_dis1) < y_length;
}


bool polygon_t::is_collision(bounding_rectangle_t &rect) {
    return true;
}

bool polygon_t::merge_polygon(const polygon_t &polygon) const {
    return false;
}

void polygon_t::addRect(rect_t &rect) {

}

void polygon_t::addRects(std::vector<rect_t> &rects) {

}

void polygon_t::addRects(polygon_t &polygon) {

}

void polygon_t::refresh() {
//    for (int i = 1; i < diffTable.size(); ++i) {
//        for (int j = 1; j < diffTable[i].size(); ++j) {
//            normalTable[i][j] = diffTable[i][j] + diffTable[i - 1][j] + diffTable[i][j - 1] - diffTable[i - 1][j - 1];
//        }
//    }
//    for (int i = 1; i < diffTable.size(); ++i) {
//        for (int j = 1; j < diffTable.size(); ++j) {
//            sumTable[i][j] = normalTable[i][j] + normalTable[i - 1][j] + normalTable[i][j - 1] - normalTable[i - 1][j - 1];
//        }
//    }
}
