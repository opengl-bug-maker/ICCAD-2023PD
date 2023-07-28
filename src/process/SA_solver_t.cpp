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

    p*=1000;//let p x 100 be the percentage that we will take the result
    //cout<<"p: "<<p<<endl;
    int x = rand()%100;
    if(p>=x){
        return true;
    }
    else{
        return false;
    }
}

SA_solver_t::SA_solver_t() {
    this->parameters_init();
}

void SA_solver_t::run(sequence_pair_enumerator_t & SPEN, double timeout) {
    this->runtime_timer.timer_start();
    this->time_limit = timeout;
    int SP_n = SPEN.valid_sequence_pairs.size();

    SPEN.updated_best_SP();
    sequence_pair_t best_sp = SPEN.best_SP;

    int it = 1;
    while(true){
        runtime_timer.timer_end();
        if(runtime_timer.get_time_elapsed() >= timeout){break;}

        this->it_timer.timer_start();

        SPEN.updated_best_SP(); //update best SP first, because SA may cause SPs to get worse
        for(auto& SP:SPEN.valid_sequence_pairs){
            timer a1("find neighbor");
            a1.timer_start();
            sequence_pair_t after = find_neighbor(SP);
            a1.timer_end();
            //a1.print_time_elapsed();
            double delta = get_delta(SP, after);
            bool change = sample_p(delta);
            if(change){
                SP = after;
            }
            if(SP.get_wirelength(true, true) < best_sp.get_wirelength(true, true)){
                best_sp = SP;
            }
        }
        if(it%10==0){
            cout<<"It : "<<it<<", t = "<<this->t<<endl;
            cout<<"current best wirlength : "<<std::setprecision(16)<<best_sp.get_wirelength(true, true)<<endl;
            cout<<"current wirelength : "<<std::setprecision(16)<<SPEN.valid_sequence_pairs[0].get_wirelength(true, true)<<endl;
            cout<<"------------------------------"<<endl;
            runtime_timer.timer_end();
            runtime_timer.print_time_elapsed();
        }
        this->t*=r;
        this->it_timer.timer_end();
        this->it_average_time =  (this->it_average_time*(it-1)+this->it_timer.get_time_elapsed()) / it;
        update_parameters();
        it++;
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
                    timer a1("find position 85");
                    a1.timer_start();
                    bool success = neighbor.find_position(false, true, 0, 0); //6ms at most
                    a1.timer_end();
                    //a1.print_time_elapsed();
                    if(success){
                        return neighbor;
                    }
                    if(m){std::swap(neighbor.v_sequence[p], neighbor.v_sequence[q]);}
                    if(n){std::swap(neighbor.h_sequence[p], neighbor.h_sequence[q]);}

                }
            }

        }
    }
    return SP; //can't find any neighbor
}

double SA_solver_t::get_delta(sequence_pair_t & ori, sequence_pair_t& after) {
    double ori_wirelength = ori.get_wirelength(true, true);
    double after_wirelength = after.get_wirelength(true, true);
    double delta = (after_wirelength-ori_wirelength)/ori_wirelength;
    return delta*10.0;
}

void SA_solver_t::parameters_init() {
    this->t = 1;
    this->r = 0.999;
}

double SA_solver_t::get_time_left() {
    this->runtime_timer.timer_end();
    double current_time = this->runtime_timer.get_time_elapsed();
    return time_limit - current_time;
}

void SA_solver_t::update_parameters() {
    double time_left = this->get_time_left();
    double it_time = this->it_average_time;
    double it_left = std::max(time_left/it_time, 1.0);
    double new_r = pow((0.005)/this->t, 1/it_left);
    this->r =  new_r;
}
