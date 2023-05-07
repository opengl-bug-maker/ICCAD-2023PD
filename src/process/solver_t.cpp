//
// Created by 林士傑 on 2023/3/27.
//

#include "solver_t.h"
//for debug
#include "iostream"
using std::cout;
using std::endl;


void solver_t::run() {
    return;
}

void solver_t::print_info(bool) {
    return;
}

floorplanning_t solver_t::get_fp(size_t) {
    return floorplanning_t();
}

floorplanning_t& solver_t::get_best_fp() {
    return best_fp;
}

solver_t::solver_t() {

}

void solver_t::generate_seq_rand() {

}
