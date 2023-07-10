//
// Created by RayChang on 2023/6/28.
//

#include "genetic_solver_t.h"
#include <algorithm>
#include "timer.h"
#include <iostream>
using std::cout;
using std::endl;

genetic_solver_t::genetic_solver_t(int greater_sequence_n,int max_sequence_n) {
    this->greater_sequence_n = greater_sequence_n;
    this->max_sequences_n = max_sequence_n;
}
void genetic_solver_t::generate_sequence_pair(int n){
//    //TODO: add a timeout to stop the process on time
//    bool timeout = false;
//    for(int i = 0; i<n; ++i){
//        sequence_pair_t sequence_pair;
//        bool done = false;
//        while(!done){
//            done = sequence_pair.add_soft_process(0);
//            if(done){
//                sequence_pair.load_best_sequence();
//                sequence_pair.predict_wire_length(false);
//                if(sequence_pair.predicted_wirelength==-1){
//                    cout<<"FUCK UP"<<endl;
//                    continue;
//                }
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
//    //build map first
//    vector<int> v_map(sequence_pair_t::sequence_n), h_map(sequence_pair_t::sequence_n);
//    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
//        v_map[seq.v_sequence[i]] = h_map[seq.h_sequence[i]] = i;
//    }
//    //type1 swap 2 sequence number in both sequence
//    bool a = find_legal_neighbor_type_1(seq, v_map, h_map);
//    if(a){return true;}
//    bool b = find_legal_neighbor_type_2(seq, v_map, h_map);
//    if(b){return true;}
//    bool c = find_legal_neighbor_type_3(seq, v_map, h_map);
//    if(c){return true;}
//    return false;
}
//bool genetic_solver_t::find_legal_neighbor_type_1(sequence_pair_t& seq, const vector<int>& v_map, const vector<int>& h_map){
//    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
//        for(int j = i+1; j<sequence_pair_t::sequence_n; ++j){
//            seq.swap_v(v_map[i], v_map[j]);
//            seq.swap_h(h_map[i], h_map[j]);
//            if(seq.find_position(false, false, 0, 0)){
//                return true;
//            }
//            seq.swap_v(v_map[i], v_map[j]);
//            seq.swap_h(h_map[i], h_map[j]);
//        }
//    }
//    return false;
//}
//bool genetic_solver_t::find_legal_neighbor_type_2(sequence_pair_t& seq, const vector<int>& v_map, const vector<int>& h_map){
//
//    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
//        for(int j = i+1; j<sequence_pair_t::sequence_n; ++j){
//            int x = rand()%2;
//            if(x){seq.swap_v(v_map[i], v_map[j]);}
//            else{seq.swap_h(h_map[i], h_map[j]);}
//            if(seq.find_position(false, false, 0, 0)){
//                return true;
//            }
//            if(x){seq.swap_v(v_map[i], v_map[j]);}
//            else{seq.swap_h(h_map[i], h_map[j]);}
//        }
//    }
//    return false;
//}
//bool genetic_solver_t::find_legal_neighbor_type_3(sequence_pair_t& seq, const vector<int>& v_map, const vector<int>& h_map){
//    //construct a sequence to change the size of the modules
//    vector<int> change_size_list(sequence_pair_t::sequence_n);
//    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
//        change_size_list[i] = i;
//    }
//    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
//        int x = rand()%sequence_pair_t::sequence_n;
//        std::swap(change_size_list[i], x);
//    }
//    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
//        int x = change_size_list[i];
//        //keep the origin size
//        vec2d_t ori = seq.modules_wh[x];
//        seq.change_size(x);
//        if(seq.find_position(false, false, 0, 0)){
//            return true;
//        }
//        else{
//            seq.modules_wh[x] = ori;
//        }
//    }
//    return false;
//}
void genetic_solver_t::selection(){
    this->sort_sequence_pair();
    while(this->sequence_pairs.size()>this->greater_sequence_n){
        this->sequence_pairs.pop_back();
    }
}
void genetic_solver_t::mutation(){
}
void genetic_solver_t::crossover(int t){
    while(t--){
        sequence_pair_t new_sequence_pair = crossover_operation();
        if(new_sequence_pair.predicted_wirelength==-1){continue;}
        else{sequence_pairs.push_back(new_sequence_pair);};
    }

}
sequence_pair_t genetic_solver_t::crossover_operation(){
    sequence_pair_t new_sequence_pair;
    int x = rand()%(this->greater_sequence_n);
    int y = rand()%(this->greater_sequence_n);
    if(x==y){
        y = (x+1)%(this->greater_sequence_n);
    }
    new_sequence_pair = sequence_pairs[x];
    for(int i = 0; i<this->sequence_pairs[x].modules_wh_i.size(); ++i){
        if(this->sequence_pairs[x].modules_wh_i[i]==this->sequence_pairs[y].modules_wh_i[i]){
            continue;
        }
        else{
            new_sequence_pair.change_size(i);
        }
    }
    new_sequence_pair.predict_wire_length(false);
    return new_sequence_pair;
}
void genetic_solver_t::growth_operation(int i){
    long long current_wirelength = sequence_pairs[i].predicted_wirelength;
    vector<vector<int>> legal_swaps;
   vector<int> v_map(sequence_pair_t::sequence_n), h_map(sequence_pair_t::sequence_n);
    for(int j = 0; j<sequence_pair_t::sequence_n; ++j){
        v_map[sequence_pairs[i].v_sequence[j]] = h_map[sequence_pairs[i].h_sequence[j]] = j;
    }
    for(int j = 0; j<sequence_pair_t::soft_n; ++j){
        for(int k = j+1; k<sequence_pair_t::soft_n; ++k){
            sequence_pairs[i].swap_v(v_map[j],v_map[k]);
            sequence_pairs[i].swap_h(h_map[j],h_map[k]);
            sequence_pairs[i].predict_wire_length(false);
            long long after_wirelength = sequence_pairs[i].predicted_wirelength;
            if(after_wirelength!=-1 && after_wirelength<current_wirelength){
                legal_swaps.push_back({j,k});
            }
            sequence_pairs[i].swap_v(v_map[j],v_map[k]);
            sequence_pairs[i].swap_h(h_map[j],h_map[k]);
        }
    }
    if(legal_swaps.size()==0){
        sequence_pairs[i].predict_wire_length(false);
        if(sequence_pairs[i].predicted_wirelength==-1){
            cout<<"FUCK UP"<<endl;
        }
        return;
    }
    int x = rand()%(legal_swaps.size());
    sequence_pairs[i].swap_v(v_map[legal_swaps[x][0]],v_map[legal_swaps[x][1]]);
    sequence_pairs[i].swap_h(h_map[legal_swaps[x][0]],h_map[legal_swaps[x][1]]);
    sequence_pairs[i].predict_wire_length(false);

}
void genetic_solver_t::growth(){
    for(int i = 0; i<sequence_pairs.size(); ++i){
        growth_operation(i);
    }
}
void genetic_solver_t::update_best_sequence_pair(){
    if(this->sequence_pairs[0].predicted_wirelength<this->best_sequence_pair.predicted_wirelength){
        this->best_sequence_pair = this->sequence_pairs[0];
    }
}

void genetic_solver_t::run() {
    this->sequence_pairs.clear();
    this->generate_sequence_pair(this->greater_sequence_n);

    const int epoch = 10;
    vector<pair<double, long long>> epoch_log;
    for(int t = 0; t<epoch; ++t){
        timer epoch_timer("epoch_timer");
        epoch_timer.timer_start();
        this->selection();
        this->crossover(20);
        //this->generate_sequence_pair(10);
        this->growth();
        //this->growth();
        this->update_best_sequence_pair();

        epoch_timer.timer_end();
        epoch_log.push_back({epoch_timer.get_time_elapsed(), this->best_sequence_pair.predicted_wirelength});

        cout<< "Epoch : "<<t+1<<", got wirelength : "<<epoch_log[t].second<<"->"<<epoch_log[t].first<<"ms. "<<endl;
    }
    for(auto& e:this->sequence_pairs) {
        for(auto& shape_i:e.modules_wh_i){
            cout<<shape_i<<" ";
        }
        cout<<", Wirelength : "<<e.predicted_wirelength<<endl;
    }
    cout<<endl;
    for(int i = 0; i<epoch_log.size(); ++i){
        cout<< "Epoch : "<<i+1<<", got wirelength : "<<epoch_log[i].second<<"->"<<epoch_log[i].first<<"ms. "<<endl;
    }
    this->best_sequence_pair.predict_wire_length(true);
}
