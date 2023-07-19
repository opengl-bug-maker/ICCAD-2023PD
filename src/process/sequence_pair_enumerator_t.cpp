//
// Created by RayChang on 2023/6/26.
//

#include "sequence_pair_enumerator_t.h"
#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
sequence_pair_enumerator_t::sequence_pair_enumerator_t(){
    this->seed_SP = sequence_pair_t();
    int a = 5;
}





std::set<int> sequence_pair_enumerator_t::random_choose(int upb, int n) {
    std::set<int> selected_fix_id;
    while(selected_fix_id.size()<n){
        int id = rand()%upb;
        selected_fix_id.insert(id);
    }
    return selected_fix_id;
}

int sequence_pair_enumerator_t::sample_from_interval(int L, int R) {
    int n = R-L;
    int x = rand()%n; //x -> [0, n)
    return x+L; //[L, R)
}

void sequence_pair_enumerator_t::generate_sequence_pairs(int n) {
    timer init_timer("init");
    init_timer.timer_start();

    this->target_sp_n = n;
    this->add_soft_process(0, true, 5);

    init_timer.timer_end();
    init_timer.print_time_elapsed();
}

bool sequence_pair_enumerator_t::add_soft_process(int i, bool with_area, int cutoff) {
    if(this->valid_sequence_pairs.size()>=target_sp_n){
        return false;
    }
    if(i>=this->seed_SP.fix_start_idx){
        this->seed_SP.predict_wirelength(true, with_area); //minimize wirelength at last
        this->valid_sequence_pairs.push_back(this->seed_SP);
        return true;
    }
    bool fnd = false;
    if(seed_SP.v_sequence.size() == 0){ //in case there are no fix module

        //TODO: fix this
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

    seed_SP.change_size(i);
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

void sequence_pair_enumerator_t::validate_all_SP() {
    cout<< "currently got "<< this->valid_sequence_pairs.size()<<" sequences pairs"<<endl;
    for(int i = 0; i<this->valid_sequence_pairs.size(); ++i){
        bool valid = this->valid_sequence_pairs[i].find_position_with_area(false,false, 0, 0);
        if(valid==false){
            cout<<"invalid SP"<<endl;
        }
        else{
            double wirelength = this->valid_sequence_pairs[i].get_wirelength(true, true);
            cout<<"valid SP : "<<std::setprecision(16)<<wirelength<<endl;

        }
    }
}


void sequence_pair_enumerator_t::validate_all_SP_print_all() {
    cout<< "currently got "<< this->valid_sequence_pairs.size()<<" sequences pairs"<<endl;
    for(int i = 0; i<this->valid_sequence_pairs.size(); ++i){
        this->valid_sequence_pairs[i].print_inline();
        bool valid = this->valid_sequence_pairs[i].find_position_with_area(false,false, 0, 0);
        if(valid==false){
            cout<<"invalid SP"<<endl;
        }
        else{
            double wirelength = this->valid_sequence_pairs[i].get_wirelength(true, true);
            cout<<"valid SP : "<<std::setprecision(16)<<wirelength<<endl;

        }

    }
}

bool sequence_pair_enumerator_t::find_greater(sequence_pair_t& sequence_pair) {
    timer find_greater_timer("find greater");
    find_greater_timer.timer_start();

    bool fnd = false;
    timer a1("a1"), a2("a2"),a3("a3");
    double wirelength = sequence_pair.get_wirelength(true, false);
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        for(int j = i+1; j<sequence_pair_t::sequence_n; ++j){
            for(int p = 0; p<=1; ++p){
                for(int q = 0; q<=1; ++q){
                    if(p==0&&q==0){continue;}
                    if(sequence_pair_t::seq_is_fix[i]||sequence_pair_t::seq_is_fix[j]){continue;}
                    a3.timer_start();
                    sequence_pair.swap_seq_number(i, j,p,q);
                    a1.timer_start();
                    bool x = sequence_pair.find_position_with_area(false, false, 0, 0);
                    a1.timer_end();
                    //a1.print_time_elapsed();
                    if(x){
                        a2.timer_start();
                        sequence_pair.predict_wirelength(true, true);
                        a2.timer_end();
                        //a2.print_time_elapsed();
                        if(sequence_pair.predicted_wirelength<wirelength){
                            fnd |= true;
                            wirelength = sequence_pair.predicted_wirelength;
                            continue;
                        }
                    }
                    sequence_pair.swap_seq_number(i, j,p,q);
                    a3.timer_end();
                    //a3.print_time_elapsed();
                }
            }
        }
    }
    find_greater_timer.timer_end();
    find_greater_timer.print_time_elapsed();
    return fnd;
}

void sequence_pair_enumerator_t::updated_best_SP() {
    double  current_best_wirelength = 1e20;
    for(auto& SP:this->valid_sequence_pairs){
        double v = SP.get_wirelength(true, true);
        if(v < current_best_wirelength){
            this->best_SP = SP;
            current_best_wirelength = v;
        }
    }
}
