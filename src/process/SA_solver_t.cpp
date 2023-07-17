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
    double p = std::pow(exp(1), -(delta_c/ this->t));
    p*=100;
    int x = rand()%100;
    if(p>=x){
        return true;
    }
    else{
        return false;
    }
}

SA_solver_t::SA_solver_t(int t) {

}
void SA_solver_t::run() {
    cout<< "original wirlength : "<<sequence_pair.predicted_wirelength<<endl;
    while(t>0){
        timer epoch_timer("epoch timer");
        epoch_timer.timer_start();
        sequence_pair.predict_wirelength(true, true);
        long long ori_wirelength = sequence_pair.predicted_wirelength;

        sequence_pair.find_position(false, true, 0, 0); //to modify the shapes
        vector<vector<int>> legal_swaps;
        for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
            for(int j = i+1; j<sequence_pair_t::sequence_n; ++j){
                for(int v = 0; v<3; ++v){
                    int p = 0, q = 0;
                    if(v==0){p = 1, q = 0;}
                    else if(v==1){p = 0, q = 1;}
                    else if(v==2){p = q = 1;}
                    sequence_pair.swap_seq_number(i, j,p,q);

                    if(sequence_pair.find_position_with_area(false,false,0,0)){
                        //legal_swaps.push_back({i, j, p, q});
                        sequence_pair.predict_wirelength(true, true);
                        long long after_wirelength = sequence_pair.predicted_wirelength;
                        double  delta_c = static_cast<double>(after_wirelength-ori_wirelength)/static_cast<double>(ori_wirelength)*10000;
                        double v = -(delta_c/t);
                        double p = std::exp(-(delta_c/t));
                        bool update = this->sample_p(delta_c);
                        if(update){
                            continue;
                        }
                    }
                    sequence_pair.swap_seq_number(i, j,p,q);
                }
            }

        }

        if(sequence_pair.predicted_wirelength < best_sequence_pair.predicted_wirelength) {
            best_sequence_pair = sequence_pair; //update
        }
        epoch_timer.timer_end();
        epoch_timer.print_time_elapsed();
        //shuffle
//        random_helper::shuffle(legal_swaps);
//        for(int i = 0; i<(int)legal_swaps.size(); ++i){
//            sequence_pair.swap_seq_number(legal_swaps[i][0], legal_swaps[i][1],legal_swaps[i][2],legal_swaps[i][3]);
//            sequence_pair.predict_wirelength(true,true);
//            long long after_wirelength = sequence_pair.predicted_wirelength;
//            double  delta_c = static_cast<double>(after_wirelength-ori_wirelength)/static_cast<double>(ori_wirelength)*10000;
//            double v = -(delta_c/t);
//            double p = std::exp(-(delta_c/t));
//            bool update = this->sample_p(delta_c);
//            if(update){
//                break;
//            }
//            sequence_pair.swap_seq_number(legal_swaps[i][0], legal_swaps[i][1],legal_swaps[i][2],legal_swaps[i][3]);
//        }
//        if(sequence_pair.predicted_wirelength < best_sequence_pair.predicted_wirelength){
//            best_sequence_pair = sequence_pair; //update
//            best_sequence_pair.predict_wirelength(true, true);
//        }

        std::cout<<"t = "<<t<<endl;
        cout<<"current wirelength : "<<std::setprecision(16)<<sequence_pair.predicted_wirelength<<std::endl;
        cout<<"best wirelength : "<<std::setprecision(16)<<best_sequence_pair.predicted_wirelength<<std::endl;
        t--;
    }
    sequence_pair.find_position(true, true, 0, 0);
    std::cout<<"got best wirelength : "<<std::setprecision(16)<<best_sequence_pair.predicted_wirelength<<std::endl;
}