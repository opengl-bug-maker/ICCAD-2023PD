//
// Created by 林士傑 on 2023/3/27.
//

#include "solver_t.h"
//for debug
#include "iostream"
#include "genetic_solver_t.h"
#include <iomanip>
#include "SA_solver_t.h"
#include "sequence_pair_enumerator_t.h"
#include "LCS_helper_t.h"
#include "random_helper.h"
using std::cout;
using std::endl;
using std::setw;

floorplan_t& solver_t::get_best_fp() {
    return best_fp;
}

solver_t::solver_t() {
    random_helper::set_seed();

    floorplan_t::init();
    sequence_pair_t::init();

    if(chip_t::get_total_module_n()<1){
        this->invalid_input = true;
        return;
    }
}
void solver_t::SA_process() {
    double init_timeout = 1800*1000; //30 minutes at most
    sequence_pair_enumerator_t SPEN;
    SPEN.init_timeout = init_timeout;
    SPEN.generate_sequence_pairs(1);

    //load immediately
    //SPEN.validate_all_SP_print_all();
    cout<<"inital stage got = "<<SPEN.valid_sequence_pairs.size()<<" SPs"<<endl;
    cout<<"--------------------------------------"<<endl;
    if(SPEN.valid_sequence_pairs.size()<1){
        cout<< "Failure in initialization. "<<endl;
        return;
    }
    SA_solver_t SA_solver;
    double time_left = std::min(this->get_time_left(), this->SA_runtime);
    SA_solver.run(SPEN, time_left);
    SPEN.updated_best_SP();
    this->best_fp = SPEN.best_SP.to_fp();
    SPEN.best_SP.write_inline();
    SPEN.best_SP.print_inline();
    cout<<"finally got wirelength = "<<std::setprecision(16)<<this->best_fp.get_wirelength()<<endl;
}

void solver_t::run() {
    if(this->invalid_input){
        cout<<"The process was terminated due to invalid inputs"<<endl;
        return;
    }
    set_timer();
    this->SA_process();
}

void solver_t::set_timer() {
    this->runtime_timer.timer_start();
}

double solver_t::get_time_left() {
    this->runtime_timer.timer_end();
    double current_time = this->runtime_timer.get_time_elapsed();
    return runtime-current_time;
}

void solver_t::load_best() {
    //case01
    sequence_pair_t SP;
    SP.v_sequence = {16, 5, 1, 0, 15, 6, 3, 10, 12, 7, 9, 4, 8, 14, 11, 18, 13, 2, 17, 19};
    SP.h_sequence = {9, 15, 5, 11, 12, 10, 14, 0, 4, 19, 16, 6, 2, 17, 13, 7, 1, 8, 3, 18};
    for(auto& e:SP.is_in_seq){e = 1;}
    SP.find_position(true, true, 0, 0);
    SP.print_inline();
    this->best_fp = SP.to_fp();
}
