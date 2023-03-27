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

class module_t {
protected:
    std::string name;
    std::vector<std::pair<module_t*, int>> connections;
public:
    virtual int getHalfWidth() = 0;

    virtual int getHalfHeight() = 0;

    virtual int getCenterPosX() = 0;

    virtual int getCenterPosY() = 0;

    const std::string &getName() const;

    std::vector<std::pair<module_t *, int>> &getConnections();
};




#endif //INC_2023PD_MODULE_H
