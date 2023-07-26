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
using std::cout;
using std::endl;
using std::setw;

floorplan_t& solver_t::get_best_fp() {
    return best_fp;
}

solver_t::solver_t() {
    floorplan_t::init();
    sequence_pair_t::init();

}
void solver_t::SA_solver() {
    double init_timeout = 1800*1000; //30 minutes at most
    sequence_pair_enumerator_t SPEN;
    SPEN.init_timeout = init_timeout;
    SPEN.generate_sequence_pairs(1);

    //load immediately
    SPEN.validate_all_SP_print_all();
    cout<<"inital stage got = "<<SPEN.valid_sequence_pairs.size()<<" SPs"<<endl;

    cout<<"--------------------------------------"<<endl;
    if(SPEN.valid_sequence_pairs.size()<1){
        cout<< "Failure in initialization. "<<endl;
        return;
    }
    SA_solver_t SA_solver;
    SA_solver.run(SPEN, 120*1000);
    SPEN.updated_best_SP();
    this->best_fp = SPEN.best_SP.to_fp();
    cout<<"finally got wirelength = "<<std::setprecision(16)<<this->best_fp.get_wirelength()<<endl;
}
