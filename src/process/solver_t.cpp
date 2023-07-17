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

using std::cout;
using std::endl;


void solver_t::run() {
//    timer solver_timer("solved timer");
//    solver_timer.timer_start();
//    sequence_pair_t::init();
//    floorplan_t::init();
//    genetic_solver_t genetic_solver(10, 5);
//    genetic_solver.run();
//    floorplan_t fp = genetic_solver.best_sequence_pair.to_fp();
//    cout<< "final wirelength : "<<std::setprecision(16)<<fp.get_wirelength()<<endl;
//    cout<<endl;
//    this->best_fp = fp;
//    this->best_fp.GUI_validation();
//    fgetc(stdin);
//    return;

    SA_solver_t SA_solver(1000);
    SA_solver.run();
    floorplan_t fp = SA_solver.best_sequence_pair.to_fp();
    fp.GUI_validation();
}
void solver_t::test1(){
    timer solver_timer("solved timer");
    solver_timer.timer_start();
    sequence_pair_t::init();
    floorplan_t::init();

    sequence_pair_enumerator_t SPEN;
    SPEN.generate_sequence_pairs(1);
    floorplan_t fp = SPEN.valid_sequence_pairs[0].to_fp();
    cout<< "predicted wirelength : "<<std::setprecision(16)<<SPEN.valid_sequence_pairs[0].get_wirelength(true, true)<<endl;
    cout<< "final wirelength : "<<std::setprecision(16)<<fp.get_wirelength()<<endl;
    cout<<endl;

    fp.GUI_validation();
    fgetc(stdin);

}
void solver_t::test2(){
    sequence_pair_enumerator_t sqen;
    sqen.generate_sequence_pairs(10);
    sqen.validate_all_SP_print_all();
    for(auto& seq:sqen.valid_sequence_pairs){
        seq.find_position(true, true, 0, 0);
        bool success = true;
        int t = 10;
        while(success&&t--){
            success = sqen.find_greater(seq);
        }
    }
    sqen.validate_all_SP_print_all();

}
void solver_t::test_specific(){
    sequence_pair_t::init();
    floorplan_t::init();

    timer solver_timer("solved timer");
    solver_timer.timer_start();

    sequence_pair_t sequence_pair;
    //sequence_pair.load_best_sequence();
    for(auto& e:sequence_pair.is_in_seq){e = 1;}
    sequence_pair.v_sequence =  {11, 16, 5, 13, 9, 0, 1, 7, 2, 18, 15, 3, 6, 8, 14, 12, 10, 4, 19, 17};
    sequence_pair.h_sequence =   {19, 15, 12, 10, 1, 3, 4, 7, 6, 17, 8, 16, 11, 5, 0, 13, 9, 14, 2, 18};
    vector<int> module_wh_i =  {3, 2, 2, 1, 2, 0, 0, 3, 0, 1, 0, 2, 4, 2, 0};
    //vector<int> module_wh_i =  {2, 2, 4, 0, 4, 1, 4, 4, 0, 2, 0, 4, 0, 3, 0};

    for(int i = 0; i<sequence_pair.soft_n; ++i){
        sequence_pair.modules_wh[i] = sequence_pair_t::soft_area_to_w_h_m[i][module_wh_i[i]];
        sequence_pair.modules_wh_i[i] = module_wh_i[i];
    }

    //sequence_pair.predict_wirelength(true, true);
    bool x = sequence_pair.find_position_with_area(false, true, 0,0);

    cout<<"SUCCESS"<<endl;
    solver_timer.timer_end();
    solver_timer.print_time_elapsed();
    sequence_pair.print_inline();
    sequence_pair.print_shapes();
    sequence_pair.sequence_pair_validation();
    floorplan_t fp = sequence_pair.to_fp();
    cout<< "predicted wirelength : "<<sequence_pair.predicted_wirelength<<endl;
    cout<< "final wirelength : "<<std::setprecision(16)<<fp.get_wirelength()<<endl;
    cout<<endl;
    this->best_fp = fp;
    fp.GUI_validation();
    fgetc(stdin);


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
