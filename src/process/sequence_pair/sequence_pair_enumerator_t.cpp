//
// Created by RayChang on 2023/6/26.
//

#include "process/sequence_pair/sequence_pair_enumerator_t.h"
#include <iostream>
#include <iomanip>
#include "process/functional/random_helper.h"
using std::cout;
using std::endl;
sequence_pair_enumerator_t::sequence_pair_enumerator_t(){
    this->seed_SP = sequence_pair_t();
}

void sequence_pair_enumerator_t::generate_sequence_pairs(int n) {
    this->init_timer.timer_start();
    this->target_sp_n = n;
    this->current_sp_n = 0;
    if(sequence_pair_t::soft_n!=0){
        bool result  = this->add_soft_process_cont(0, false, 5, 0, 0);
        if(result==false){
            cout<<"Failed in initialization"<<endl;
        }
    }
    else{
        bool success = this->seed_SP.find_position(true, true, 0, 0);
        this->valid_sequence_pairs.push_back(this->seed_SP);
    }
    this->init_timer.timer_end();
    this->init_timer.print_time_elapsed();
}

bool sequence_pair_enumerator_t::add_soft_process(int i, bool with_area, int cutoff) {
    if(this->valid_sequence_pairs.size()>=target_sp_n){
        return false;
    }
    if(i>=this->seed_SP.fix_start_idx){
        this->seed_SP.get_wirelength(); //minimize wirelength at last
        if(this->seed_SP.predicted_wirelength!=-1){
            this->valid_sequence_pairs.push_back(this->seed_SP);
            return true;
        }
        else{
            return false;
        }
    }
    bool fnd = false;
    if(seed_SP.v_sequence.size() == 0){ //in case there are no fix module
        seed_SP.v_sequence.push_back(this->seed_SP.add_soft_order[i]);
        seed_SP.h_sequence.push_back(this->seed_SP.add_soft_order[i]);
        this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 1;
        bool success;
        if(with_area){
            success = this->seed_SP.find_position_with_area(false, false, 0, 0);
        }
        else{
            success = this->seed_SP.find_position(false, false, 0, 0);
        }
        fnd|= this->add_soft_process(i+1,with_area, cutoff);
        this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 0;
        seed_SP.v_sequence.pop_back();
        seed_SP.h_sequence.pop_back();
        return fnd;
    }

    int success_n = 0;

    for(int j = 0; j <= seed_SP.v_sequence.size(); ++j){
        for(int k = 0; k <= seed_SP.h_sequence.size(); ++k){
            seed_SP.change_size(i);
            seed_SP.v_sequence.insert(seed_SP.v_sequence.begin() + j, this->seed_SP.add_soft_order[i]);
            seed_SP.h_sequence.insert(seed_SP.h_sequence.begin() + k, this->seed_SP.add_soft_order[i]);
            this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 1;
            bool success;
            if(with_area){
                //cout<< i<<" : ";
                timer a1("find time with area");
                a1.timer_start();
                success = this->seed_SP.find_position_with_area(false, false, 0, 0);
                a1.timer_end();
                //a1.print_time_elapsed();
            }
            else{
                //cout<< i<<" : ";
                timer a1("find time");
                a1.timer_start();
                success = this->seed_SP.find_position(false, false, 0, 0);
                a1.timer_end();
                //a1.print_time_elapsed();
            }
            if(success){
                if(this->add_soft_process(i+1, with_area, cutoff)){
                    fnd|=1;
                }
            }
            this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 0;
            seed_SP.v_sequence.erase(seed_SP.v_sequence.begin() + j);
            seed_SP.h_sequence.erase(seed_SP.h_sequence.begin() + k);
            if(fnd&&i>=cutoff){
                return true;
            }
        }
    }
    //cout<<i<<" : success n  = "<<success_n<<endl;
    return false;
}
bool sequence_pair_enumerator_t::add_soft_process_cont(int i,bool with_area, int cutoff, int start_j, int start_k) {
    this->init_timer.timer_end();
    if( this->init_timer.get_time_elapsed() >= this->init_timeout){
        return false;
    }
    if(this->current_sp_n >= this->target_sp_n){
        return false;
    }
    cout<<"Initializing-placing "<<i<<"-th module"<<endl;
    if(i>=this->seed_SP.fix_start_idx){
        //this->seed_SP.predict_wirelength(true, false); //minimize wirelength at last
        this->res_SP = this->seed_SP;
        //if(this->seed_SP.predicted_wirelength!=-1){
            this->valid_sequence_pairs.push_back(this->seed_SP);
        //}
        this->current_sp_n++;
        //this->seed_SP.print_inline();
        return true;
    }
    if(this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]]){
        return add_soft_process_cont(i+1, with_area, cutoff, start_j, start_k);
    }

    bool fnd = false;
    if(seed_SP.v_sequence.size() == 0){ //in case there are no fix module
        seed_SP.v_sequence.push_back(this->seed_SP.add_soft_order[i]);
        seed_SP.h_sequence.push_back(this->seed_SP.add_soft_order[i]);
        this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 1;
        bool success;
        if(with_area){
            success = this->seed_SP.find_position_with_area(false, false, 0, 0);
        }
        else{
            success = this->seed_SP.find_position(false, false, 0, 0);
        }
        fnd|= this->add_soft_process_cont(i+1,with_area, cutoff, 0, 0);
        this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 0;
        seed_SP.v_sequence.pop_back();
        seed_SP.h_sequence.pop_back();
        return fnd;
    }
    vector<pair<double,vector<int>>> legal_pos;

    for(int j = 0; j <= seed_SP.v_sequence.size(); ++j){
        for(int k = 0; k <= seed_SP.h_sequence.size(); ++k){
            int jj = (j+start_j)%(seed_SP.v_sequence.size()+1);
            int kk = (k+start_k)%(seed_SP.h_sequence.size()+1);
            seed_SP.change_size(this->seed_SP.add_soft_order[i]);

            seed_SP.v_sequence.insert(seed_SP.v_sequence.begin() + jj, this->seed_SP.add_soft_order[i]);
            seed_SP.h_sequence.insert(seed_SP.h_sequence.begin() + kk, this->seed_SP.add_soft_order[i]);
            this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 1;
            bool success;
            if(with_area){
                success = this->seed_SP.find_position_with_area(false, false, 0, 0);
            }
            else{
                //success = this->seed_SP.find_position_allow_illegal(true, true, 0, 0);
                success = this->seed_SP.find_position(true, true, 0, 0);
            }
            if(success){
                double wl = this->seed_SP.z;
                //legal_pos.push_back({wl, {jj, kk}});
                //break;
                if(this->add_soft_process_cont(i+1, with_area, cutoff, jj, kk)){
                    return true;
                }
            }
            this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 0;
            seed_SP.v_sequence.erase(seed_SP.v_sequence.begin() + jj);
            seed_SP.h_sequence.erase(seed_SP.h_sequence.begin() + kk);
            if(fnd&&i>=cutoff){
                return true;
            }
            if(this->current_sp_n>=this->target_sp_n){
                return false;
            }
        }
    }
    return false;
}



void sequence_pair_enumerator_t::updated_best_SP() {
    if(this->valid_sequence_pairs.size()<1){
        return;
    }
    double current_best_wirelength = this->valid_sequence_pairs[0].get_wirelength();
    this->best_SP = this->valid_sequence_pairs[0];
}





