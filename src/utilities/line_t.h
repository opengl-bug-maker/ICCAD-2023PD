//
// Created by 林士傑 on 2023/6/19.
//

#ifndef BOUNDINGLINE_LINE_T_H
#define BOUNDINGLINE_LINE_T_H

#include "utilities/vec2d_t.h"

class line_t {
public:
    enum line_type{
        type_horizontal = 0,
        type_vertical = 1,
        type_none = 2
    };
    enum line_direction_type{
        direction_type_east = 0,
        direction_type_north = 1,
        direction_type_west = 2,
        direction_type_south = 3,
        direction_type_none = 4
    };
    enum line_collision_type{
        collision_horizontal = 0,
        collision_vertical = 1,
        collision_type_none = 2
    };
private:
    vec2d_t _start, _end;
    line_t::line_type _line_type = line_t::line_type::type_none;
    line_t::line_direction_type _line_direction_type = line_t::line_direction_type::direction_type_none;
public:
    line_t();
    line_t(const vec2d_t &start, const vec2d_t &end);

    bool operator==(const line_t& line) const;

    bool operator!=(const line_t& line) const;

//     line_t(const vec2d_t &start, const vec2d_t &anEnd);

//     line_type getType() const;

//     const vec2d_t &getStart() const;

//     void setStart(const vec2d_t &start);

//     const vec2d_t &getAnEnd() const;

//     void setAnEnd(const vec2d_t &anEnd);

    bool overlap(const line_t line) const;

    static std::vector<line_t> to_lines(const std::vector<vec2d_t> points);

    const vec2d_t &get_start() const;

    const vec2d_t &get_end() const;

    const double get_little_x() const;

    const double get_little_y() const;

    const double get_large_x() const;

    const double get_large_y() const;

    line_t::line_type get_line_type() const;

    line_t::line_direction_type get_line_direction_type() const;

    vec2d_t get_vec() const;

    double dot(const line_t& line) const;

    bool is_vertical(const line_t& line) const;

    bool is_parallel(const line_t& line) const;

    bool is_same_line(const line_t& line) const;
    
    vec2d_t intersect_point(const line_t& line) const;

    line_t::line_collision_type is_intersect(const line_t& line) const;

    line_t::line_collision_type is_touch(const line_t& line) const;

    std::optional<std::vector<line_t>> parallel_cut(const line_t& line) const;

    std::optional<std::vector<line_t>> parallel_cut_reverse(const line_t& line) const;
};

std::ostream& operator<<(std::ostream& os, const line_t& line);

#endif //BOUNDINGLINE_LINE_T_H
