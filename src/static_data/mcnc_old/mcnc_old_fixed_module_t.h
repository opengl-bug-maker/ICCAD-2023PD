#ifndef MCNC_OLD_FIXED_MODULE_T_H
#define MCNC_OLD_FIXED_MODULE_T_H

#include <string>
#include <vector>
#include "mcnc_old_module_t.h"
#include "mcnc_old_terminal_t.h"
#include "utilities/rect_t.h"

class mcnc_old_fixed_module_t : public mcnc_old_module_t {
public:
    rect_t rect;
};

#endif