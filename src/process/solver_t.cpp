//
// Created by 林士傑 on 2023/3/27.
//

#include "solver_t.h"
//for debug
#include "iostream"
#include "genetic_solver_t.h"
#include <iomanip>
using std::cout;
using std::endl;


void solver_t::run() {
    sequence_pair_t::init();
    floorplan_t::init();



//    genetic_solver_t genetic_solver(10, 5);
//    genetic_solver.run();
//    floorplan_t fp = genetic_solver.best_sequence_pair.to_fp();
//    cout<< "final wirelength : "<<std::setprecision(16)<<fp.get_wirelength()<<endl;
//    this->best_fp = fp;
//    cout<<endl;



    sequence_pair_t sequence_pair;

    bool success = sequence_pair.add_soft_process(0);

    if(success){
        //sequence_pair.load_best_sequence();
        sequence_pair.predict_wire_length(true);

        cout<<"SUCCESS"<<endl;
        sequence_pair.print_inline();
        sequence_pair.sequence_pair_validation();
        floorplan_t fp = sequence_pair.to_fp();
        cout<< "predicted wirelength : "<<sequence_pair.predicted_wirelength<<endl;
        cout<< "final wirelength : "<<std::setprecision(16)<<fp.get_wirelength()<<endl;
        cout<<endl;
        this->best_fp = fp;
        fp.GUI_validation();
        fgetc(stdin);

    }
    else{cout<<"FAIL"<<endl; return;}


}

void solver_t::print_info(bool) {
    return;
}


floorplan_t& solver_t::get_best_fp() {
    return best_fp;
}

solver_t::solver_t() {
    floorplan_t::init();
    sequence_pair_t::init();
}

void solver_t::generate_seq_rand() {

}


int solver_t::find_valid_seq_pair(int n) {
}
