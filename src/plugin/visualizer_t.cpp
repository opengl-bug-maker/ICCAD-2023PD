//
// Created by lkj on 2023/4/7.
//

#include "visualizer_t.h"
#include "static_data/chip_t.h"
#include "static_data/module_t.h"
#include <thread>
#include <iostream>
#include <sstream>
#include <iomanip>

void my_out(const std::string& str){
#ifdef USING_VIS
    std::cout << str << std::endl;
#endif
}
void visualizer_t::show_fp(std::vector<bounding_rectangle_t> & bd_rect) {
    srand(time(NULL));
    }
    return ;
}
