//
// Created by 林士傑 on 2023/5/12.
//

#include "line_t.h"
#include "exception"

line_t::line_t(const vec2d_t &start, const vec2d_t &anEnd) : start(start), end(anEnd), type(line_t::line_type::none) {
    if(start.get_x() == end.get_x())
        this->type = line_t::line_type::horizontal;
    else if(start.get_y() == end.get_y())
        this->type = line_t::line_type::vertical;
    throw std::exception();
}

std::pair<bool, vec2d_t> line_t::intersect(const line_t &line) const {
    vec2d_t point(this->type == line_t::line_type::horizontal ? this->start.get_x() : line.start.get_x(),
                  this->type == line_t::line_type::vertical ? this->start.get_y() : line.start.get_y());

    if(this->type == line.type) return {false, point};

    if( point.get_x() >= this->start.get_x() &&
        point.get_x() <= this->end.get_x() &&
        point.get_x() >= line.start.get_x() &&
        point.get_x() <= line.end.get_x() &&
        point.get_y() >= this->start.get_y() &&
        point.get_y() <= this->end.get_y() &&
        point.get_y() >= line.start.get_y() &&
        point.get_y() <= line.end.get_y())
        return {true, point};

    return {false, point};
}

line_t::line_type line_t::getType() const {
    return type;
}

const vec2d_t &line_t::getStart() const {
    return start;
}

void line_t::setStart(const vec2d_t &start) {
    line_t::start = start;
}

const vec2d_t &line_t::getAnEnd() const {
    return end;
}

void line_t::setAnEnd(const vec2d_t &anEnd) {
    end = anEnd;
}
