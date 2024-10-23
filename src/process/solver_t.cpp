//
// Created by 林士傑 on 2023/3/27.
//

#include "solver_t.h"
//for debug
#include "iostream"
#include <thread>
#include <iomanip>
#include "SA_solver_t.h"
#include "sequence_pair/sequence_pair_enumerator_t.h"
#include "process/functional/random_helper.h"
#include "case_table_t.h"
#include <iomanip>

#include "quad_sequence/quad_sequence_t.h"

using std::cout;
using std::endl;
using std::setw;


floorplan_t& solver_t::get_best_fp() {
    return best_fp;
}

solver_t::solver_t() {
    random_helper::set_seed(); //build up random table

    floorplan_t::init(); //class initialization

    sequence_pair_t::init(); //class initialization

    std::ios_base::sync_with_stdio(false); //make cout faster
    std::cin.tie(0);
    cout.tie(0);

    if(chip_t::get_total_module_n()<1){
        this->invalid_input = true;
        return;
    }
}
void solver_t::SA_process(sequence_pair_enumerator_t& SPEN) {
    cout<<"--------------SA process--------------"<<endl;
    if(SPEN.valid_sequence_pairs.size()<1){
        cout<< "Failure in initialization. "<<endl;
        return;
    }
    SA_solver_t SA_solver;
    double time_left = std::min(this->get_time_left(), this->SA_runtime);
    SA_solver.run(SPEN, 0.6*time_left, 0.05, 0.008, false, 0, 1, false);
    SA_solver.run(SPEN, 0.4*time_left, 0.05, 0.008, false, 0, 1, true);
    SPEN.updated_best_SP();
    SPEN.valid_sequence_pairs[0].print_inline();
    SPEN.valid_sequence_pairs[0].sequence_pair_validation(1);
    SPEN.valid_sequence_pairs[0].to_rectilinear_and_plot();
}

void solver_t::run() {
    set_timer_start();

    if(this->invalid_input){
        cout<<"The process was terminated due to invalid inputs"<<endl;
        return;
    }

    sequence_pair_enumerator_t SPEN;
    SPEN.init_timeout = this->init_timeout;
    SPEN.generate_sequence_pairs(1);
    SPEN.valid_sequence_pairs[0].sequence_pair_validation();
    SPEN.valid_sequence_pairs[0].print_inline();
    SPEN.valid_sequence_pairs[0].get_wirelength();
    this->SA_process(SPEN);
}

void solver_t::set_timer_start() {
    this->runtime_timer.timer_start();
}

double solver_t::get_time_left() {
    this->runtime_timer.timer_end();
    double current_time = this->runtime_timer.get_time_elapsed();
    return runtime-current_time;
}

void solver_t::load_specific_best() {
    SA_solver_t SA_solver;
    case_table_t case_table;
    int case_id = chip_t::get_similar_case_num();
    sequence_pair_t SP;
    bool fnd_cases = false;
    if(case_id!=-1){
        fnd_cases = true;
        SP.v_sequence = case_table.cases[case_id][0];
        SP.h_sequence = case_table.cases[case_id][1];
        for(auto& e:SP.is_in_seq){e = 1;}
    }
    if(fnd_cases){
        SP.find_position(true, true, 0, 0);
        SP.find_position_with_area(true, true, 0, 0);
        SP = SA_solver.post_process(SP);
        floorplan_t loaded_fp = SP.to_fp();
        if(loaded_fp.get_wirelength()<this->best_fp.get_wirelength()){
            this->best_fp = loaded_fp;
            cout<<"Apply the prepared SP..."<<endl;
        }
    }

}

void solver_t::load_specific_without_cmp() {
    SA_solver_t SA_solver;
    case_table_t case_table;
    int case_id = chip_t::get_similar_case_num();
    sequence_pair_t SP;
    if(case_id!=-1){
        SP.v_sequence = case_table.cases[case_id][0];
        SP.h_sequence = case_table.cases[case_id][1];
        for(auto& e:SP.is_in_seq){e = 1;}
        SP.find_position(true, true, 0, 0);
        SP.find_position_with_area(true, true, 0, 0);
        SP = SA_solver.post_process(SP);
        SP.print_inline();
        //t1.print_time_elapsed();
        SP.sequence_pair_validation();
        floorplan_t loaded_fp = SP.to_fp();
        this->best_fp = loaded_fp;
        cout<< "Result : "<<std::setprecision(16)<<this->best_fp.get_wirelength()<<endl;
    }
}
