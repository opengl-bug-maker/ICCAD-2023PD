//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_OUTPUT_UTILITY_T_H
#define INC_2023PD_OUTPUT_UTILITY_T_H

#include <string>
#include <vector>
#include <static_data/module_t.h>
#include <utilities/vec2d_t.h>

class output_utility_t {
    friend class output_handler_t;
    std::string module_name;
    std::vector<vec2d_t> points;
    output_utility_t(const std::string &moduleName, const std::vector<vec2d_t> &points);
public:
    std::string to_string();
};


#endif //INC_2023PD_OUTPUT_UTILITY_T_H
