//
// Created by 林士傑 on 2023/3/27.
//

#include <random>
#include "quadratic_t.h"
#include "static_data/chip_t.h"
std::vector<vec2d_t> quadratic_t::final_position;
std::vector<vec2d_t> quadratic_t::slope;
std::vector<vec2d_t> quadratic_t::constrain;

vec2d_t quadratic_t::get_coor(int index) {
    return final_position[index];
}

vec2d_t quadratic_t::get_coor_random(int index) {
    std::default_random_engine rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> xDis(0, chip_t::get_width());
    std::uniform_int_distribution<> yDis(0, chip_t::get_height());
    return vec2d_t(xDis(gen), yDis(gen));
}

void quadratic_t::set_floopplanning(const std::vector<bounding_rectangle_t> &bounding_rectangles,
                                    const std::vector<bool> &is_placed) {
}
