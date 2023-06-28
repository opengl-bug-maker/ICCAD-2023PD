//
// Created by 林士傑 on 2023/3/27.
//

#include "solver_t.h"
//for debug
#include "iostream"
using std::cout;
using std::endl;


void solver_t::run() {
    sequence_pair_t::init();
    floorplan_t::init();
    sequence_pair_t sequence_pair;
    for(int i = 0; i<sequence_pair_t::soft_area_to_w_h_m.size(); ++i){
        auto x = sequence_pair_t::soft_area_to_w_h_m[i];
        for(auto& e:x)
            cout<< i<<" :" <<e.get_x()<<" "<<e.get_y()<<endl;
    }
    bool success = sequence_pair.add_soft_process(0);
    if(success){cout<<"SUCCESS"<<endl;}
    else{cout<<"FAIL"<<endl;}

    sequence_pair.wire_length_predict(true);
    cout<< "predicted wirelength : "<<sequence_pair.predicted_wirelength<<endl;
    floorplan_t fp = sequence_pair.to_fp();
    cout<< "final wirelength : "<<fp.get_wirelength()<<endl;
    this->best_fp = fp;
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
