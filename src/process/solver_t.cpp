//
// Created by 林士傑 on 2023/3/27.
//

#include "solver_t.h"
//for debug
#include "iostream"
#include "genetic_solver_t.h"
#include <iomanip>
#include "SA_solver_t.h"
#include "sequence_pair_enumerator_t.h"
#include "LCS_helper_t.h"
using std::cout;
using std::endl;
using std::setw;

void solver_t::run() {
    this->SA_solver();
}
void solver_t::test_initial(){
    timer solver_timer("solved timer");
    solver_timer.timer_start();
    sequence_pair_t::init();
    floorplan_t::init();

    sequence_pair_enumerator_t SPEN;
    SPEN.generate_sequence_pairs(40);
    if(SPEN.valid_sequence_pairs.size()==0){
        cout<<"fail on initialization"<<endl;
        return;
    }
    for(int i = 0; i<SPEN.valid_sequence_pairs.size(); ++i){
        bool x = SPEN.valid_sequence_pairs[i].find_position(true, true, 0, 0);
    }
    SPEN.validate_all_SP_print_all();
    SPEN.updated_best_SP();
    SPEN.best_SP.print_inline();
    SPEN.best_SP.sequence_pair_validation();
    floorplan_t fp = SPEN.best_SP.to_fp();
    cout<< "predicted wirelength : "<<std::setprecision(16)<<SPEN.best_SP.get_wirelength(true, true)<<endl;
    cout<< "final wirelength : "<<std::setprecision(16)<<fp.get_wirelength()<<endl;
    cout<<endl;

    fp.GUI_validation();
    fgetc(stdin);

}
void solver_t::test_initial_and_find_greater(){
    sequence_pair_enumerator_t SPEN;
    SPEN.generate_sequence_pairs(1);
    for(auto& seq:SPEN.valid_sequence_pairs){
        bool success = true;
        int t = 10;
        while(success&&t--){
            success = SPEN.find_greater(seq);
        }
    }
    bool x = SPEN.valid_sequence_pairs[0].find_position(false, true, 0, 0);
    SPEN.valid_sequence_pairs[0].sequence_pair_validation();
    floorplan_t fp = SPEN.valid_sequence_pairs[0].to_fp();
    SPEN.valid_sequence_pairs[0].print_inline();
    fp.GUI_validation();
    cout<<"finally got wirelength : "<<std::setprecision(16)<<fp.get_wirelength()<<endl;
}
void solver_t::test_specific(){
    sequence_pair_t::init();
    floorplan_t::init();

    timer solver_timer("solved timer");
    solver_timer.timer_start();

    sequence_pair_t sequence_pair;
    //sequence_pair.load_best_sequence();
    for(auto& e:sequence_pair.is_in_seq){e = 1;}
//    sequence_pair.v_sequence =  {16, 7, 6, 5, 14, 8, 0, 3, 2, 18, 15, 13, 9, 11, 10, 12, 4, 1, 19, 17};
//    sequence_pair.h_sequence =   {19, 15, 10, 11, 9, 12, 4, 1, 0, 13, 17, 16, 6, 7, 5, 3, 14, 8, 2, 18};
//    vector<int> module_wh_i =  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};


    sequence_pair.v_sequence =  {16, 1, 3, 7, 6, 8, 5, 4, 2, 18, 15, 13, 9, 11, 10, 12, 14, 0, 19, 17};
    sequence_pair.h_sequence =    {19, 15, 13, 9, 11, 10, 12, 14, 5, 4, 0, 17, 16, 1, 6, 2, 3, 8, 7, 18};
    vector<int> module_wh_i =  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

//    for(int i = 0; i<sequence_pair.soft_n; ++i){
//        sequence_pair.modules_wh[i] = sequence_pair_t::soft_area_to_w_h_m[i][module_wh_i[i]];
//        sequence_pair.modules_wh_i[i] = module_wh_i[i];
//    }s

    bool x = sequence_pair.find_position(false, false, 0, 0);
    sequence_pair.predict_wirelength(true, false);
    sequence_pair.print_inline();


    cout<<"SUCCESS"<<endl;
    solver_timer.timer_end();
    solver_timer.print_time_elapsed();
    sequence_pair.print_inline();
    sequence_pair.print_shapes_i();
    sequence_pair.sequence_pair_validation();
    floorplan_t fp = sequence_pair.to_fp();
    cout<< "predicted wirelength : "<<sequence_pair.predicted_wirelength<<endl;
    cout<< "final wirelength : "<<std::setprecision(16)<<fp.get_wirelength()<<endl;
    cout<<endl;
    this->best_fp = fp;
    fp.GUI_validation();




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
void solver_t::test_LCS() {

    sequence_pair_enumerator_t SPEN;
    SPEN.generate_sequence_pairs(5);
    SPEN.valid_sequence_pairs[0].print_v();
    SPEN.valid_sequence_pairs[2].print_v();
    sequence_pair_t res = LCS_helper_t::crossover(SPEN.valid_sequence_pairs[0], SPEN.valid_sequence_pairs[2]);
    SPEN.seed_SP = res;
    SPEN.add_soft_process_cont(0, false, 5, 0, 0);

    cout<<"Gene : "<<endl;
    res.print_inline();

    cout<<"Parent 1 : "<<endl;
    SPEN.valid_sequence_pairs[0].print_inline();

    cout<<"Parent 2 : "<<endl;
    SPEN.valid_sequence_pairs[2].print_inline();

    cout<<"Child : "<<endl;
    SPEN.res_SP.print_inline();
}

void solver_t::test_genetic_solver(){
    double init_timeout = 10*1000;
    sequence_pair_enumerator_t SPEN;
    SPEN.init_timeout = init_timeout;
    SPEN.generate_sequence_pairs(10);
    int t = 10;
    while(t--){
        std::sort(SPEN.valid_sequence_pairs.begin(), SPEN.valid_sequence_pairs.end(), [&](auto& a, auto& b){
            return a.get_wirelength(true, false)<b.get_wirelength(true, false);
        });
        sequence_pair_t res = LCS_helper_t::crossover(SPEN.valid_sequence_pairs[0], SPEN.valid_sequence_pairs[1]);
        SPEN.seed_SP = res;
        SPEN.generate_sequence_pairs(5);
        bool seed_init = SPEN.seed_SP.find_position(true, true, 0, 0);
//        cout<<"Seed : "<<endl;
//        SPEN.seed_SP.print_inline();
//        cout<<"--------------------------------"<<endl;
//        while(SPEN.valid_sequence_pairs.size()>10){
//            SPEN.valid_sequence_pairs.pop_back();
//        }
        if(t%1==0){
            cout<<"t = "<<10-t<<" "<<"wirelength = "<<std::setprecision(16)<<SPEN.valid_sequence_pairs[0].get_wirelength(true, true)<<endl;
        }
    }

    SPEN.validate_all_SP_print_all();

}

void solver_t::test_other() {
    double t = 1;
    double r = 0.95;
    for(int i = 0; i<100; ++i){
        cout<< i<<" : "<<t*r<<endl;
        t*=r;
    }
}

void solver_t::SA_solver() {
    double init_timeout = 600*1000;
    sequence_pair_enumerator_t SPEN;
    SPEN.init_timeout = init_timeout;
    SPEN.generate_sequence_pairs(1000);

    cout<<"inital stage got = "<<SPEN.valid_sequence_pairs.size()<<" SPs"<<endl;


    cout<<"--------------------------------------"<<endl;
    SA_solver_t SA_solver;
    SA_solver.run(SPEN, 600*1000);
    SPEN.updated_best_SP();
    this->best_fp = SPEN.best_SP.to_fp();
//    SPEN.best_SP.print_inline();
//    floorplan_t fp = SPEN.best_SP.to_fp();
//    fp.GUI_validation();
//    cout<<"--------------------------------------"<<endl;
//    cout<<"finally got wirelength = "<<std::setprecision(16)<<fp.get_wirelength()<<endl;
}
