//
// Created by RayChang on 2023/7/14.
//

#include "SA_solver_t.h"
#include "random_helper.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include "timer.h"

using std::cout;
using std::endl;
bool SA_solver_t::sample_p(double delta_c) {
    double p = exp(-delta_c/t);
    //let p x 100 be the percentage that we will take the result
    p*=100;
    int x = rand()%100;
    if(p>=x){
        return true;
    }
    else{
        return false;
    }
}

SA_solver_t::SA_solver_t() {
    this->t = 1;
}

void SA_solver_t::run(sequence_pair_enumerator_t & SPEN, double timeout) {
    int SP_n = SPEN.valid_sequence_pairs.size();
    SPEN.updated_best_SP();
    sequence_pair_t best_sp = SPEN.best_SP;
    this->run_time.timer_start();
    int it = 1;
    while(true){
        run_time.timer_end();
        if(run_time.get_time_elapsed()>= timeout){break;}
        SPEN.updated_best_SP(); //update best SP first, because SA may cause SPs to get worse
        for(auto& SP:SPEN.valid_sequence_pairs){
            sequence_pair_t after = find_neighbor(SP);
            double delta = get_delta(SP, after);
            bool change = sample_p(delta);
            if(change){
                SP = after;
            }
            if(SP.get_wirelength(true, true) < best_sp.get_wirelength(true, true)){
                best_sp = SP;
            }
        }
        cout<<"It : "<<it<<", t = "<<this->t<<endl;
        cout<<"current best wirlength : "<<std::setprecision(16)<<best_sp.get_wirelength(true, true)<<endl;
        it++; this->t*=r;
        //SPEN.validate_all_SP_print_all();
        cout<<"------------------------------"<<endl;
    }
    SPEN.validate_all_SP_print_all();
    SPEN.valid_sequence_pairs[0] = best_sp;
}

sequence_pair_t SA_solver_t::find_neighbor(sequence_pair_t SP) {
    vector<int> v_map(sequence_pair_t::sequence_n), h_map(sequence_pair_t::sequence_n);
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        v_map[SP.v_sequence[i]] = h_map[SP.h_sequence[i]] = i;
    }
    sequence_pair_t neighbor = SP;
    vector<int> rand_i = random_helper::rand_list(sequence_pair_t::sequence_n);
    vector<int> rand_j = random_helper::rand_list(sequence_pair_t::sequence_n);
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        for(int j = 0; j<sequence_pair_t::sequence_n; ++j){
            if(sequence_pair_t::seq_is_fix[i]||sequence_pair_t::seq_is_fix[j]){continue;}
            int p = v_map[rand_i[i]], q = h_map[rand_j[j]];
            for(int m = 0; m<2; ++m){
                for(int n = 0; n<2; ++n){
                    if(m==0&&n==0){continue;}
                    if(m){std::swap(neighbor.v_sequence[p], neighbor.v_sequence[q]);}
                    if(n){std::swap(neighbor.h_sequence[p], neighbor.h_sequence[q]);}

                    if(neighbor.find_position(false, true, 0, 0)){
                        this->neighbor_fnd = true;
                        return neighbor;
                    }
                    if(m){std::swap(neighbor.v_sequence[p], neighbor.v_sequence[q]);}
                    if(n){std::swap(neighbor.h_sequence[p], neighbor.h_sequence[q]);}

                }
            }

        }
    }
    this->neighbor_fnd = false;
    return SP; //can't find any neighbor
}

double SA_solver_t::get_delta(sequence_pair_t & ori, sequence_pair_t& after) {
    double ori_wirelength = ori.get_wirelength(true, true);
    double after_wirelength = after.get_wirelength(true, true);
    return (after_wirelength-ori_wirelength)/ori_wirelength;
}
