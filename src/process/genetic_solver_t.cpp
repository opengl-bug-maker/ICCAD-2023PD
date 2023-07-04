//
// Created by RayChang on 2023/6/28.
//

#include "genetic_solver_t.h"
#include <algorithm>

#include <iostream>
using std::cout;
using std::endl;

genetic_solver_t::genetic_solver_t(int greater_sequence_n,int max_sequence_n) {
    this->greater_sequence_n = greater_sequence_n;
    this->max_sequences_n = max_sequence_n;
}
void genetic_solver_t::generate_sequence_pair(int n){
    //TODO: add a timeout to stop the process on time
//    bool timeout = false;
//    for(int i = 0; i<n; ++i){
//        sequence_pair_t sequence_pair;
//        bool done = false;
//        while(!done){
//            done = sequence_pair.add_soft_process(0);
//            if(done){
//                sequence_pair.predict_wire_length(true);
//            }
//        }
//        this->sequence_pairs.push_back(sequence_pair);
//        if(timeout){break;}
//    }
}
void genetic_solver_t::sort_sequence_pair(){
    std::sort(this->sequence_pairs.begin(), this->sequence_pairs.end(), [](sequence_pair_t& a, sequence_pair_t& b){
       return a.predicted_wirelength<b.predicted_wirelength;
    });
}
bool genetic_solver_t::find_legal_neighbor(sequence_pair_t& seq){
    //build map first
    vector<int> v_map(sequence_pair_t::sequence_n), h_map(sequence_pair_t::sequence_n);
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        v_map[seq.v_sequence[i]] = h_map[seq.h_sequence[i]] = i;
    }
    //type1 swap 2 sequence number in both sequence
    bool a = find_legal_neighbor_type_1(seq, v_map, h_map);
    if(a){return true;}
    bool b = find_legal_neighbor_type_2(seq, v_map, h_map);
    if(b){return true;}
    bool c = find_legal_neighbor_type_3(seq, v_map, h_map);
    if(c){return true;}
    return false;
}
bool genetic_solver_t::find_legal_neighbor_type_1(sequence_pair_t& seq, const vector<int>& v_map, const vector<int>& h_map){
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        for(int j = i+1; j<sequence_pair_t::sequence_n; ++j){
            seq.swap_v(v_map[i], v_map[j]);
            seq.swap_h(h_map[i], h_map[j]);
            if(seq.find_position(false, false, 0, 0)){
                return true;
            }
            seq.swap_v(v_map[i], v_map[j]);
            seq.swap_h(h_map[i], h_map[j]);
        }
    }
    return false;
}
bool genetic_solver_t::find_legal_neighbor_type_2(sequence_pair_t& seq, const vector<int>& v_map, const vector<int>& h_map){

    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        for(int j = i+1; j<sequence_pair_t::sequence_n; ++j){
            int x = rand()%2;
            if(x){seq.swap_v(v_map[i], v_map[j]);}
            else{seq.swap_h(h_map[i], h_map[j]);}
            if(seq.find_position(false, false, 0, 0)){
                return true;
            }
            if(x){seq.swap_v(v_map[i], v_map[j]);}
            else{seq.swap_h(h_map[i], h_map[j]);}
        }
    }
    return false;
}
bool genetic_solver_t::find_legal_neighbor_type_3(sequence_pair_t& seq, const vector<int>& v_map, const vector<int>& h_map){
    //construct a sequence to change the size of the modules
    vector<int> change_size_list(sequence_pair_t::sequence_n);
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        change_size_list[i] = i;
    }
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        int x = rand()%sequence_pair_t::sequence_n;
        std::swap(change_size_list[i], x);
    }
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        int x = change_size_list[i];
        //keep the origin size
        vec2d_t ori = seq.modules_wh[x];
        seq.change_size(x);
        if(seq.find_position(false, false, 0, 0)){
            return true;
        }
        else{
            seq.modules_wh[x] = ori;
        }
    }
    return false;
}
void genetic_solver_t::selection(){
    while(this->sequence_pairs.size()>this->max_sequences_n){
        this->sequence_pairs.pop_back();
    }
}
void genetic_solver_t::mutation(){
//    int sequence_pair_size =this->sequence_pairs.size(); //to avoid growing sequence pair size
//    for(int it = 0; it<sequence_pair_size; ++it){
//        sequence_pair_t nx = this->sequence_pairs[it];
//        if(this->find_legal_neighbor(nx)){
//            nx.predict_wire_length(true);
//            if(nx.predicted_wirelength<this->sequence_pairs[it].predicted_wirelength){
//                this->sequence_pairs[it] = nx;
//            }
//        }
//    }
}
void genetic_solver_t::update_best_sequence_pair(){
    if(this->sequence_pairs[0].predicted_wirelength<this->best_sequence_pair.predicted_wirelength){
        this->best_sequence_pair = this->sequence_pairs[0];
    }
}

void genetic_solver_t::run() {
    this->sequence_pairs.clear();
    this->generate_sequence_pair(this->max_sequences_n);
    const int epoch = 1000;
    vector<long long> epoch_log;
    for(int t = 0; t<epoch; ++t){
        this->mutation();
        this->sort_sequence_pair();
        if(t%2==0){this->selection();}
        this->update_best_sequence_pair();
        epoch_log.push_back(this->best_sequence_pair.predicted_wirelength);

   }
    for(auto& e:this->sequence_pairs) {
        cout<<e.predicted_wirelength<<" ";
    }
    cout<<endl;
    for(int i = 0; i<epoch_log.size(); ++i){
        cout<< "Epoch : "<<i+1<<", got wirelength : "<<this->best_sequence_pair.predicted_wirelength<<endl;
    }
}
