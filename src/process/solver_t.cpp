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
using std::setw;

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
void solver_t::test_initial(){
    timer solver_timer("solved timer");
    solver_timer.timer_start();
    sequence_pair_t::init();
    floorplan_t::init();

    sequence_pair_enumerator_t SPEN;
    SPEN.generate_sequence_pairs(1);
    if(SPEN.valid_sequence_pairs.size()==0){
        cout<<"fail on initialization"<<endl;
        return;
    }

    SPEN.valid_sequence_pairs[0].print_inline();
    SPEN.valid_sequence_pairs[0].sequence_pair_validation();
    floorplan_t fp = SPEN.valid_sequence_pairs[0].to_fp();
    cout<< "predicted wirelength : "<<std::setprecision(16)<<SPEN.valid_sequence_pairs[0].get_wirelength(true, true)<<endl;
    cout<< "final wirelength : "<<std::setprecision(16)<<fp.get_wirelength()<<endl;
    cout<<endl;

    fp.GUI_validation();
    fgetc(stdin);

}
void solver_t::test_initial_and_find_greater(){
    sequence_pair_enumerator_t SPEN;
    SPEN.generate_sequence_pairs(1);
    SPEN.validate_all_SP_print_all();
    for(auto& seq:SPEN.valid_sequence_pairs){
        seq.find_position(true, true, 0, 0);
        bool success = true;
        int t = 10;
        while(success&&t--){
            success = SPEN.find_greater(seq);
        }
    }
    //SPEN.validate_all_SP_print_all();
    SPEN.updated_best_SP();
    floorplan_t fp = SPEN.best_SP.to_fp();
    fp.GUI_validation();
    cout<<"finally got wirelength : "<<fp.get_wirelength()<<endl;
}
void solver_t::test_specific(){
    sequence_pair_t::init();
    floorplan_t::init();

    timer solver_timer("solved timer");
    solver_timer.timer_start();

    sequence_pair_t sequence_pair;
    //sequence_pair.load_best_sequence();
    for(auto& e:sequence_pair.is_in_seq){e = 1;}
    sequence_pair.v_sequence =  {113, 34, 112, 84, 83, 76, 111, 55, 99, 110, 67, 141, 140, 139, 138, 100, 32, 31, 30, 29, 82, 77, 28, 27, 33, 142, 156, 170, 184, 109, 128, 61, 60, 20, 62, 59, 54, 137, 101, 3, 58, 57, 143, 157, 171, 185, 144, 158, 172, 186, 23, 145, 159, 173, 187, 146, 160, 174, 188, 147, 161, 175, 189, 53, 148, 162, 49, 176, 190, 108, 129, 37, 6, 1, 4, 86, 56, 50, 26, 136, 102, 5, 98, 0, 2, 150, 151, 152, 153, 154, 155, 149, 164, 165, 166, 167, 168, 169, 163, 178, 179, 180, 181, 182, 183, 177, 192, 193, 194, 195, 196, 197, 191, 63, 22, 78, 97, 87, 198, 88, 199, 114, 66, 65, 64, 85, 115, 69, 68, 91, 116, 72, 71, 70, 107, 130, 52, 73, 25, 81, 135, 103, 127, 79, 46, 80, 92, 43, 21, 19, 93, 44, 95, 96, 39, 75, 74, 94, 90, 117, 104, 126, 35, 18, 14, 17, 15, 13, 16, 12, 24, 51, 11, 118, 105, 125, 48, 47, 45, 41, 38, 40, 42, 36, 119, 106, 131, 132, 133, 134, 124, 8, 9, 7, 10, 89, 122, 123, 121, 120};
    sequence_pair.h_sequence =   {138, 139, 140, 141, 137, 54, 20, 33, 62, 28, 61, 27, 136, 26, 23, 59, 67, 86, 50, 56, 60, 1, 77, 4, 6, 135, 81, 10, 25, 134, 133, 132, 131, 128, 129, 130, 110, 99, 111, 55, 76, 112, 83, 113, 109, 82, 84, 30, 29, 34, 32, 31, 184, 170, 156, 142, 108, 49, 37, 53, 185, 186, 187, 188, 189, 190, 171, 172, 173, 174, 175, 176, 157, 158, 159, 160, 161, 162, 143, 144, 58, 145, 146, 147, 57, 3, 148, 107, 52, 88, 73, 97, 63, 85, 87, 199, 198, 191, 192, 22, 193, 78, 194, 195, 196, 197, 177, 178, 179, 180, 181, 182, 183, 163, 164, 165, 166, 167, 168, 169, 149, 150, 151, 152, 153, 154, 2, 155, 122, 8, 123, 9, 121, 5, 106, 124, 7, 48, 47, 45, 41, 38, 40, 89, 120, 105, 125, 42, 36, 35, 18, 14, 17, 119, 104, 15, 13, 16, 126, 92, 12, 11, 19, 21, 24, 93, 95, 96, 51, 39, 75, 74, 118, 103, 94, 98, 127, 79, 46, 80, 43, 44, 90, 117, 100, 101, 0, 66, 65, 64, 69, 68, 91, 72, 71, 70, 102, 114, 115, 116};
    vector<int> module_wh_i =  {2, 0, 3, 4, 0, 2, 4, 2, 1, 1, 0, 2, 0, 0, 0, 2, 0, 1, 0, 3, 3, 3, 3, 1, 1, 0, 1, 4, 1, 0, 0, 0, 0, 0, 0, 2, 1, 4, 1, 0, 1, 2, 1, 4, 4, 2, 4, 2, 2, 4, 4, 2, 3, 0, 0, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 4, 3, 4, 4, 0, 0, 4, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0};
    //vector<int> module_wh_i =  {2, 2, 4, 0, 4, 1, 4, 4, 0, 2, 0, 4, 0, 3, 0};

    for(int i = 0; i<sequence_pair.soft_n; ++i){
        sequence_pair.modules_wh[i] = sequence_pair_t::soft_area_to_w_h_m[i][module_wh_i[i]];
        sequence_pair.modules_wh_i[i] = module_wh_i[i];
    }

    //sequence_pair.predict_wirelength(true, true);
    sequence_pair.print_inline();
    timer a1("a1");
    a1.timer_start();
    //bool x = sequence_pair.find_position(true, false, 0,0);
    bool x = sequence_pair.find_position_with_area(true, true, 0,0);
    a1.timer_end();
    a1.print_time_elapsed();

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
