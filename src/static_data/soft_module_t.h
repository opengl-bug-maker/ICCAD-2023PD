//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_SOFT_MODULE_T_H
#define INC_2023PD_SOFT_MODULE_T_H

#include "static_data/module_t.h"

class soft_module_t : public module_t {
    int minArea;
    soft_module_t();
public:
    const int getHalfWidth() const override;

    const int getHalfHeight() const override;

    const int getCenterPosX() const override;

    const int getCenterPosY() const override;

    bounding_rectangle_t make_bd() override;

    std::pair<bounding_rectangle_t, bool> make_bd(rect_t &rect) override;

    static soft_module_t* fileInput(std::fstream& file);
};


#endif //INC_2023PD_SOFT_MODULE_T_H
