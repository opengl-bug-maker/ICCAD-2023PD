//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_ANSWER_CHECKER_T_H
#define INC_2023PD_ANSWER_CHECKER_T_H

#include <string>
#include <fstream>
#include <exception>
#include <iostream>
#include "static_data/chip_t.h"
#include "static_data/module_t.h"
#include "utilities/vec2d_t.h"
#include "utilities/line_t.h"

class unit {
    module_t* link;
    std::string name;
    std::vector<vec2d_t> points;
    rect_t bounding_rect;
    double area = 0;
public:
    explicit unit(const std::string &name, module_t* link);
    void addPoint(vec2d_t vec2D);
    void is_valid();
};

class checker_exception : public std::exception {
public:
    std::string what;
    explicit checker_exception(const std::string &what);
};

class answer_checker_t {
    static unsigned long long int hpwl;
    static std::vector<unit> units;
    static bool read_file(std::string output_file);
public:
    static bool test(std::string output_file);
};


#endif //INC_2023PD_ANSWER_CHECKER_T_H
