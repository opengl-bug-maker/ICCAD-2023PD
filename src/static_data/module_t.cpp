//
// Created by 林士傑 on 2023/3/8.
//

#include "module_t.h"

const std::string &module_t::getName() const {
    return name;
}

std::vector<std::pair<module_t *, int>> &module_t::getConnections() {
    return connections;
}
