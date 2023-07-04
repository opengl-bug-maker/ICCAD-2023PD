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
        sequence_pair_t sequence_pair;
        sequence_pair.add_soft_order.resize(sequence_pair_t::soft_n);
        vector<double> areas = sequence_pair.modules_area;
        for(int i = 0; i<sequence_pair_t::soft_n; ++i){
            sequence_pair.add_soft_order[i] = i;
        }
        std::sort(sequence_pair.add_soft_order.begin(), sequence_pair.add_soft_order.end(), [&](int&a, int& b){
            return areas[a]>areas[b];
        });
        bool success = sequence_pair.add_soft_process(0);

    if(success){

        cout<<"SUCCESS"<<endl;
        sequence_pair.load_best_sequence();
        sequence_pair.print_inline();
        sequence_pair.predict_wire_length(true);
        bool x = sequence_pair.find_position(false,false,0,0);
        int a  =5;

    }
    else{cout<<"FAIL"<<endl;}
    sequence_pair.predict_wire_length(true);
    cout<< "predicted wirelength : "<<sequence_pair.predicted_wirelength<<endl;
    floorplan_t fp = sequence_pair.to_fp();
    cout<< "final wirelength : "<<std::setprecision(16)<<fp.get_wirelength()<<endl;
    this->best_fp = fp;
    cout<<endl;
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
