//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_SOFT_MODULE_T_H
#define INC_2023PD_SOFT_MODULE_T_H

#include "static_data/module_t.h"

class soft_module_t : public module_t {
    int minArea;

public:
    int getHalfWidth() override;

    int getHalfHeight() override;

    int getCenterPosX() override;

    int getCenterPosY() override;

    static soft_module_t* fileInput(std::fstream& file);
};


#endif //INC_2023PD_SOFT_MODULE_T_H
