//
// Created by 林士傑 on 2023/6/19.
//

#ifndef BOUNDINGLINE_BOUNDING_LINE_T_H
#define BOUNDINGLINE_BOUNDING_LINE_T_H

#include "circular_T_list_t.h"
#include "utilities/line_t.h"

class bounding_line_t;

class bounding_line_element_t : public line_t {
public:
    bounding_line_element_t();
    bounding_line_element_t(const line_t &line);
    bounding_line_element_t(const vec2d_t &start, const vec2d_t &end);

    bool operator==(const bounding_line_element_t& bounding_line_element) const;

    bool operator!=(const bounding_line_element_t& bounding_line_element) const;
};

class bounding_line_t {
    bool clockwise = false;

    double area;
    
    bounding_line_t();

    void update_area();

    void update_clockwise();

    // bool collision(const bounding_line_t& bounding_line) const;
public:
    circular_T_list_t<bounding_line_element_t> lines;
    bounding_line_t(circular_T_list_t<bounding_line_element_t> circular_list, bool clockwise = true);
    bounding_line_t(const std::vector<bounding_line_element_t>& lines, bool clockwise = true);
    bounding_line_t(const std::vector<vec2d_t>& points, bool clockwise = true);

    bounding_line_t operator=(const bounding_line_t& bounding_line);

    // const line_t& get_left_most_line();

    // const line_t& get_top_most_line();

    // const line_t& get_right_most_line();

    // const line_t& get_bottom_most_line();

    bool touch(const bounding_line_t& bounding_line) const;

    void update();

    std::vector<vec2d_t> get_nodes() const;

    double get_area() const;

    bool get_clockwise() const;

    void print() const;

    void print_reverse() const;

    friend std::vector<bounding_line_t> merge(bounding_line_t bounding_line0, bounding_line_t bounding_line1);
};

std::ostream& operator<<(std::ostream& os, const bounding_line_t& bounding_line);

#endif //BOUNDINGLINE_BOUNDING_LINE_T_H
