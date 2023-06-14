//
// Created by 林士傑 on 2023/3/27.
//

#include "output_utility_t.h"

std::string output_utility_t::to_string() {
    std::string str = "\n" + this->module_name;
    str += " " + std::to_string(points.size());
    for (const auto& p : points) {
        str += "\n" + std::to_string((int)p.get_x()) + " " + std::to_string((int)p.get_y());
    }
    return str;
}

output_utility_t::output_utility_t(const std::string &moduleName, const std::vector<vec2d_t> &points) :
module_name(moduleName), points(points) {}
