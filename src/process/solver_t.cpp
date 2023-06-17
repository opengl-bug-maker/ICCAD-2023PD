//
// Created by 林士傑 on 2023/3/27.
//

#include "solver_t.h"
//for debug
#include "iostream"
using std::cout;
using std::endl;


bool solver_t::run() {
    sequence_pair_enumerator_t sqen;
    vector<sequence_pair_t> valid_seq = sqen.get_all_valid_seq();
    bool ok = 0;
    floorplan_t fnd_fp;
    const int epoch = 500;
    for(int i = 0; i<epoch&&!ok; ++i){
        cout<< "epoch: "<<i+1<<endl;
        sqen.change_size();
        sqen.seq_randomize();
        pair<bool, floorplan_t>success_fp = sqen.seq.get_fp();
        if(success_fp.first){
            ok = 1;
            fnd_fp = success_fp.second;
        }
    }
    if(ok){
        this->best_fp = fnd_fp;
        return true;
    }
    else{
        cout<<"FAIL!!"<<endl;
        return false;
    }
}

void solver_t::print_info(bool) {
    return;
}


floorplan_t& solver_t::get_best_fp() {
    return best_fp;
}

solver_t::solver_t() {

}

void solver_t::generate_seq_rand() {

}


int solver_t::find_valid_seq_pair(int n) {
}
