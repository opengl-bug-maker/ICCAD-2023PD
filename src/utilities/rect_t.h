//
// Created by 林士傑 on 2023/3/9.
//

#ifndef INC_2023PD_RECT_H
#define INC_2023PD_RECT_H

#include "position_t.h"

class rect_t {
    position_t LeftLower;
    position_t RightUpper;
    position_t Center;
    position_t Size;

public:
    const position_t &getLeftLower() const;

    const position_t &getRightUpper() const;

    const position_t &getCenter() const;

    const position_t &getSize() const;
};


#endif //INC_2023PD_RECT_H
