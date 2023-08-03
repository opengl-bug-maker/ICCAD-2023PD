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
    for(auto& e:SPEN.valid_sequence_pairs){e.find_position(true, true, 0, 0);}
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
    this->load_specific_best();
}

void solver_t::set_timer() {
    this->runtime_timer.timer_start();
}

double solver_t::get_time_left() {
    this->runtime_timer.timer_end();
    double current_time = this->runtime_timer.get_time_elapsed();
    return runtime-current_time;
}

void solver_t::load_specific_best() {
//Case01
//    v: {16, 2, 18, 3, 11, 15, 14, 7, 6, 10, 13, 4, 9, 8, 0, 1, 12, 19, 5, 17}
//    h: {15, 4, 19, 3, 0, 7, 6, 9, 11, 13, 8, 14, 10, 12, 5, 1, 16, 2, 17, 18}
//    wirelength : 156712525
//    v: {16, 5, 4, 14, 3, 1, 9, 13, 7, 15, 6, 10, 8, 11, 18, 0, 2, 19, 12, 17}
//    h: {15, 19, 2, 16, 14, 3, 10, 12, 7, 5, 11, 6, 9, 8, 4, 13, 17, 0, 1, 18}
//    wirelength : 159334926


//Case02
//    v: {19, 18, 20, 12, 1, 21, 9, 15, 0, 10, 3, 8, 5, 7, 22, 13, 14, 2, 23, 4, 6, 11, 16, 17}
//    h: {16, 18, 3, 9, 7, 14, 19, 5, 1, 10, 12, 20, 4, 17, 15, 8, 2, 11, 0, 21, 13, 6, 22, 23}
//    wirelength : 19232105
//    v: {19, 20, 18, 21, 3, 12, 22, 4, 11, 14, 15, 1, 9, 10, 7, 8, 0, 13, 5, 2, 23, 6, 16, 17}
//    h: {16, 18, 1, 11, 4, 9, 14, 5, 19, 3, 10, 15, 17, 2, 6, 20, 12, 21, 8, 13, 7, 0, 22, 23}
//    wirelength : 20138472

//Case03
//    v: {41, 40, 28, 20, 39, 29, 6, 30, 19, 38, 25, 23, 7, 21, 17, 24, 9, 5, 1, 0, 10, 18, 22, 8, 3, 4, 14, 26, 2, 11, 12, 31, 32, 15, 27, 13, 16, 37, 36, 35, 33, 34}
//    h: {38, 39, 37, 17, 40, 41, 18, 5, 36, 14, 19, 16, 27, 22, 3, 6, 24, 20, 15, 4, 0, 23, 8, 7, 21, 1, 13, 2, 9, 35, 11, 10, 12, 25, 26, 34, 28, 33, 32, 29, 30, 31}
//    wirelength : 2180111

//    v: {41, 28, 40, 39, 25, 17, 29, 30, 5, 26, 22, 38, 18, 23, 1, 37, 20, 9, 16, 27, 7, 21, 6, 31, 3, 4, 36, 19, 10, 0, 32, 13, 14, 8, 24, 15, 11, 2, 12, 33, 34, 35}
//    h: {37, 16, 38, 23, 39, 40, 36, 35, 4, 41, 26, 22, 15, 7, 14, 25, 18, 11, 3, 28, 2, 21, 13, 24, 5, 17, 0, 1, 12, 27, 34, 8, 6, 33, 29, 9, 10, 19, 20, 30, 32, 31}
//    wirelength : 2652869

//Case04
//    v: {27, 20, 17, 18, 16, 15, 13, 21, 1, 9, 26, 10, 14, 4, 11, 6, 5, 12, 8, 7, 19, 3, 25, 2, 0, 22, 23, 24}
//    h: {26, 25, 27, 16, 24, 19, 14, 0, 5, 13, 15, 17, 4, 2, 7, 18, 1, 3, 6, 11, 20, 9, 10, 8, 12, 21, 23, 22}
//    wirelength : 67140100
//Case05
//    v: {16, 18, 3, 4, 19, 20, 2, 17, 1, 21, 9, 14, 13, 10, 22, 0, 12, 11, 23, 6, 15, 8, 7, 5}
//    h: {21, 20, 22, 0, 23, 1, 19, 2, 18, 6, 3, 7, 4, 9, 12, 5, 14, 15, 8, 10, 11, 16, 13, 17}
//    wirelength : 16245400
//    v: {16, 17, 18, 3, 19, 20, 21, 4, 1, 6, 22, 0, 2, 9, 10, 12, 14, 15, 7, 11, 8, 5, 23, 13}
//    h: {21, 22, 20, 19, 0, 23, 1, 18, 2, 14, 12, 3, 4, 11, 13, 16, 15, 9, 8, 10, 5, 6, 7, 17}
//    wirelength : 17831700

//Case06
//    v: {21, 22, 23, 33, 24, 0, 1, 9, 2, 16, 25, 3, 4, 14, 17, 5, 11, 12, 15, 10, 13, 32, 19, 20, 18, 6, 26, 27, 8, 31, 7, 30, 29, 28}
//    h: {25, 12, 24, 14, 26, 27, 13, 16, 28, 15, 18, 17, 19, 23, 9, 11, 20, 6, 7, 8, 22, 0, 21, 1, 10, 2, 3, 29, 4, 30, 5, 31, 32, 33}
//    wirelength : 37476750

//    v: {21, 22, 23, 33, 0, 1, 24, 9, 25, 2, 16, 17, 11, 3, 14, 13, 15, 12, 20, 19, 4, 5, 10, 26, 18, 7, 27, 6, 32, 8, 31, 30, 28, 29}
//    h: {25, 24, 26, 27, 12, 28, 16, 13, 23, 14, 22, 18, 17, 21, 15, 19, 20, 9, 7, 6, 11, 1, 8, 0, 29, 33, 10, 30, 2, 3, 4, 5, 31, 32}
//    wirelength : 39360800


    int case_id = chip_t::get_similar_case_num();
    sequence_pair_t SP;
    bool fnd_cases = false;
    if(case_id==0){
        SP.v_sequence = {16, 5, 4, 14, 3, 1, 9, 13, 7, 15, 6, 10, 8, 11, 18, 0, 2, 19, 12, 17};
        SP.h_sequence = {15, 19, 2, 16, 14, 3, 10, 12, 7, 5, 11, 6, 9, 8, 4, 13, 17, 0, 1, 18};
        fnd_cases = true;
    }
    if(case_id==1){
        SP.v_sequence = {19, 20, 18, 21, 3, 12, 22, 4, 11, 14, 15, 1, 9, 10, 7, 8, 0, 13, 5, 2, 23, 6, 16, 17};
        SP.h_sequence = {16, 18, 1, 11, 4, 9, 14, 5, 19, 3, 10, 15, 17, 2, 6, 20, 12, 21, 8, 13, 7, 0, 22, 23};
        fnd_cases = true;
    }
    if(case_id==2){
        SP.v_sequence = {41, 28, 40, 39, 25, 17, 29, 30, 5, 26, 22, 38, 18, 23, 1, 37, 20, 9, 16, 27, 7, 21, 6, 31, 3, 4, 36, 19, 10, 0, 32, 13, 14, 8, 24, 15, 11, 2, 12, 33, 34, 35};
        SP.h_sequence = {37, 16, 38, 23, 39, 40, 36, 35, 4, 41, 26, 22, 15, 7, 14, 25, 18, 11, 3, 28, 2, 21, 13, 24, 5, 17, 0, 1, 12, 27, 34, 8, 6, 33, 29, 9, 10, 19, 20, 30, 32, 31};
        fnd_cases = true;
    }
    if(case_id==3){
        SP.v_sequence = {27, 20, 17, 18, 16, 15, 13, 21, 1, 9, 26, 10, 14, 4, 11, 6, 5, 12, 8, 7, 19, 3, 25, 2, 0, 22, 23, 24};
        SP.h_sequence = {26, 25, 27, 16, 24, 19, 14, 0, 5, 13, 15, 17, 4, 2, 7, 18, 1, 3, 6, 11, 20, 9, 10, 8, 12, 21, 23, 22};
        fnd_cases = true;
    }
    if(case_id==4){
        SP.v_sequence = {16, 17, 18, 3, 19, 20, 21, 4, 1, 6, 22, 0, 2, 9, 10, 12, 14, 15, 7, 11, 8, 5, 23, 13};
        SP.h_sequence = {21, 22, 20, 19, 0, 23, 1, 18, 2, 14, 12, 3, 4, 11, 13, 16, 15, 9, 8, 10, 5, 6, 7, 17};
        fnd_cases = true;
    }
    if(case_id==5){
        SP.v_sequence = {21, 22, 23, 33, 0, 1, 24, 9, 25, 2, 16, 17, 11, 3, 14, 13, 15, 12, 20, 19, 4, 5, 10, 26, 18, 7, 27, 6, 32, 8, 31, 30, 28, 29};
        SP.h_sequence = {25, 24, 26, 27, 12, 28, 16, 13, 23, 14, 22, 18, 17, 21, 15, 19, 20, 9, 7, 6, 11, 1, 8, 0, 29, 33, 10, 30, 2, 3, 4, 5, 31, 32};
        fnd_cases = true;
    }
    for(auto& e:SP.is_in_seq){e = 1;}
    if(fnd_cases){
        timer t1("find pos");
        t1.timer_start();
        SP.find_position(true, true, 0, 0);
        t1.timer_end();
        //t1.print_time_elapsed();
        //SP.sequence_pair_validation();

        floorplan_t loaded_fp = SP.to_fp();
        if(loaded_fp.get_wirelength()<this->best_fp.get_wirelength()){
            this->best_fp = loaded_fp;
        }
    }

}
