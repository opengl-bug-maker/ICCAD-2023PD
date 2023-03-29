//
// Created by 林士傑 on 2023/3/8.
//

#ifndef INC_2023PD_MODULE_H
#define INC_2023PD_MODULE_H

#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <cmath>
#include "utilities/rect_t.h"
#include "utilities/bounding_rectangle_t.h"

class module_t {
    friend class chip_t;
protected:
    std::string name;
    std::vector<std::pair<const module_t *const, const int>> connections;

public:
    virtual const int getHalfWidth() const = 0;

    virtual const int getHalfHeight() const = 0;

    virtual const int getCenterPosX() const = 0;

    virtual const int getCenterPosY() const = 0;

    virtual bounding_rectangle_t make_bd() = 0;

    virtual std::pair<bounding_rectangle_t, bool> make_bd(rect_t& rect) = 0;

    const std::string &getName() const;

    const std::vector<std::pair<const module_t *const, const int>> &getConnections() const;
};




#endif //INC_2023PD_MODULE_H
