//
// Created by 林士傑 on 2023/3/27.
//

#include "solver_t.h"
//for debug
#include "iostream"
using std::cout;
using std::endl;


bool solver_t::run() {
    static sequence_pair_enumerator_t sqen;

    bool ok = 0;
    floorplan_t fnd_fp;
    const int epoch = 1000000;
    for(int i = 0; i<epoch&&!ok; ++i){
        cout<< "epoch: "<<i+1<<endl;
        sqen.seq_randomize();
//        sqen.seq.print_inline();
//        for(auto& e:sqen.fix_sequence_v){cout<<e<<" ";}cout<<endl;
//        for(auto& e:sqen.fix_sequence_h){cout<<e<<" ";}cout<<endl;
        pair<bool, floorplan_t>success_fp = sqen.seq.get_fp();
//        if(success_fp.first==false){
//            sqen.seq.print_inline();
//        }
        if(success_fp.first){
            ok = 1;
            fnd_fp = success_fp.second;
        }
    }
    if(ok){
        cout<<"SUCCESS"<<endl;
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
    floorplan_t::init();
    sequence_pair_t::init();
}

void solver_t::generate_seq_rand() {

}


int solver_t::find_valid_seq_pair(int n) {
}
