//
// Created by RayChang on 2023/6/26.
//

#include "sequence_pair_enumerator_t.h"
#include <iostream>
sequence_pair_enumerator_t::sequence_pair_enumerator_t() {
    this->fix_n = chip_t::get_fixed_modules().size();
    this->soft_n = chip_t::get_soft_modules().size();
    this->legal_pairs.resize(sequence_pair_t::sequence_n);
    this->fix_sequence_v.resize(fix_n);
    this->fix_sequence_h.resize(fix_n);

    for(auto& e:legal_pairs){
        e.resize(sequence_pair_t::sequence_n);
    }
    this->soft_seq_interval.resize(soft_n);

}

void sequence_pair_enumerator_t::seq_randomize() {
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        int x = rand()%sequence_pair_t::sequence_n;
        int y = rand()%sequence_pair_t::sequence_n;
        this->seq.swap_h(i, x);
        this->seq.swap_v(i, y);
    }
}


std::set<int> sequence_pair_enumerator_t::random_choose(int upb, int n) {
    std::set<int> selected_fix_id;
    while(selected_fix_id.size()<n){
        int id = rand()%upb;
        selected_fix_id.insert(id);
    }
    return selected_fix_id;
}

void sequence_pair_enumerator_t::find_illegal_pair_for_i(int i) {
    int i_h = this->seq.modules_wh[i].get_x();
    int i_w = this->seq.modules_wh[i].get_y();
    //cout<< i_w<<" "<<i_h<<endl;
    for(int j = sequence_pair_t::fix_start_idx; j<sequence_pair_t::sequence_n; ++j){
        this->legal_pairs[i][j].clear();
        int fix_x = sequence_pair_t::seq_fixed_map[j]->get_left_lower().get_x();
        int fix_y = sequence_pair_t::seq_fixed_map[j]->get_left_lower().get_y();
        int fix_w = sequence_pair_t::seq_fixed_map[j]->get_size().get_x();
        int fix_h = sequence_pair_t::seq_fixed_map[j]->get_size().get_y();
        if(fix_x-i_w<0){
            this->legal_pairs[i][j].push_back(0); //left
            //std::cout<< i<<" "<<j<<" left"<<std::endl;
        }
        if(fix_x+fix_w+i_w>static_cast<int>(chip_t::get_width())){
            this->legal_pairs[i][j].push_back(1); //right
            //std::cout<< i<<" "<<j<<" right"<<std::endl;
        }
        if(fix_y+fix_h+i_h>static_cast<int>(chip_t::get_height())){
            this->legal_pairs[i][j].push_back(2); //upper
            //std::cout<< i<<" "<<j<<" top"<<std::endl;
        }
        if(fix_y-i_h<0){
            this->legal_pairs[i][j].push_back(3); //bottom
            //std::cout<< i<<" "<<j<<" bottom"<<std::endl;
        }
    }

}
int sequence_pair_enumerator_t::sample_from_interval(int L, int R) {
    int n = R-L;
    int x = rand()%n; //x -> [0, n)
    return x+L; //[L, R)
}

void sequence_pair_enumerator_t::randomize(vector<int> &v) {
    int n = v.size();
    for(int i = 0; i<v.size(); ++i){
        int x = rand()%n;
        std::swap(v[i], v[x]);
    }
}
void sequence_pair_enumerator_t::search_legal_perm_in_fix(int soft_i) {
    int vL = -1, vR = fix_n, hL = -1, hR = fix_n;
    vector<int> v_fix_map(sequence_pair_t::sequence_n), h_fix_map(sequence_pair_t::sequence_n);
    for(int i = 0; i<this->fix_sequence_v.size(); ++i){
        h_fix_map[fix_sequence_h[i]] = v_fix_map[fix_sequence_v[i]] = i;
    }
    for(int i = sequence_pair_t::fix_start_idx; i<sequence_pair_t::sequence_n; ++i){
        int fix_id_v = v_fix_map[i], fix_id_h = h_fix_map[i];
        int x = rand()%2;
        for(int j = 0; j<this->legal_pairs[soft_i][i].size(); ++j){
            if(this->legal_pairs[soft_i][i][j]==1){ //right
                //cant be V:[F, S], H: [F, S]
                if(vR<=fix_id_v||hR<=fix_id_h){continue;}
                if(fix_id_v>vL && x){
                    vR = std::min(vR, fix_id_v);
                }
                if(hL<fix_id_h && !x){
                    hR = std::min(hR, fix_id_h);
                }
            }
            else if(this->legal_pairs[soft_i][i][j]==0){ //left
                if(vL>fix_id_v||hL>fix_id_h){continue;}
                //cant be V:[S, F], H: [S, F]
                if(fix_id_v<vR&&x){
                    vL = std::max(vL, fix_id_v);
                }
                if(fix_id_h<hR&&!x){
                    hL = std::max(hL, fix_id_h);
                }
            }
            else if(this->legal_pairs[soft_i][i][j]==2){ //top
                if(vR<=fix_id_v||hL>fix_id_h){continue;}
                //cant be V:[F, S], H: [S, F]
                if(fix_id_v>vL&&x){
                    vR = std::min(vR, fix_id_v);
                }
                if(fix_id_h<hR&&!x){
                    hL = std::max(hL, fix_id_h);
                }
            }
            else if(this->legal_pairs[soft_i][i][j]==3){ //bottom
                if(vL>fix_id_v||hR<=fix_id_h){continue;}
                // cant be V: [S, F] H:[F,S]
                if(vR>fix_id_v&&x){
                    vL = std::max(vL, fix_id_v);
                }
                if(hL<fix_id_h&&!x){
                    hR = std::min(hR, fix_id_h);
                }

            }
        }
    }
    //cout<< vL<<" "<<vR<<" "<<hL<<" "<<hR<<endl;
    this->soft_seq_interval[soft_i] = {vL, vR, hL, hR};
    //this->soft_seq_interval[soft_i] = {-1, fix_n,-1, fix_n};
}