//
// Created by 林士傑 on 2023/5/12.
//

#ifndef ICCAD2023PD_LINE_T_H
#define ICCAD2023PD_LINE_T_H

#include "utilities/vec2d_t.h"
class line_t {
public:
    enum line_type {
        horizontal = 0,
        vertical = 1,
        none = 2
    };
private:
    line_type type;
    vec2d_t start;
    vec2d_t end;
public:
    line_t(const vec2d_t &start, const vec2d_t &anEnd);

    line_type getType() const;

    const vec2d_t &getStart() const;

    void setStart(const vec2d_t &start);

    const vec2d_t &getAnEnd() const;

    void setAnEnd(const vec2d_t &anEnd);

    std::pair<bool, vec2d_t> intersect(const line_t& line) const;
};


#endif //ICCAD2023PD_LINE_T_H
