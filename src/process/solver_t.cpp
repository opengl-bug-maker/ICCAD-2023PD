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
void solver_t::test_initial(){
    timer solver_timer("solved timer");
    solver_timer.timer_start();
    sequence_pair_t::init();
    floorplan_t::init();

    sequence_pair_enumerator_t SPEN;
    SPEN.generate_sequence_pairs(10);
    if(SPEN.valid_sequence_pairs.size()==0){
        cout<<"fail on initialization"<<endl;
        return;
    }
    SPEN.valid_sequence_pairs[0].print_inline();
//    int t = 10;
//    while(t--){
//        bool fnd = SPEN.find_greater(SPEN.valid_sequence_pairs[0]);
//        if(fnd==false){
//            break;
//        }
//    }


    SPEN.valid_sequence_pairs[0].sequence_pair_validation();
    floorplan_t fp = SPEN.valid_sequence_pairs[0].to_fp();
    cout<< "predicted wirelength : "<<std::setprecision(16)<<SPEN.valid_sequence_pairs[0].get_wirelength(true, true)<<endl;
    cout<< "final wirelength : "<<std::setprecision(16)<<fp.get_wirelength()<<endl;
    cout<<endl;

    fp.GUI_validation();
    fgetc(stdin);

}
void solver_t::test2(){
    sequence_pair_enumerator_t SPEN;
    SPEN.generate_sequence_pairs(10);
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
    sequence_pair.v_sequence =  {113, 34, 112, 84, 27, 67, 111, 76, 110, 50, 94, 99, 141, 140, 139, 138, 100, 32, 31, 83, 30, 29, 82, 80, 93, 28, 33, 142, 156, 170, 184, 109, 22, 128, 36, 61, 60, 62, 59, 23, 54, 137, 101, 5, 55, 58, 21, 57, 143, 157, 171, 185, 144, 158, 172, 186, 145, 159, 173, 187, 146, 160, 174, 188, 147, 161, 175, 189, 53, 148, 52, 162, 176, 190, 108, 18, 129, 98, 1, 19, 86, 20, 56, 26, 136, 102, 16, 2, 3, 37, 4, 150, 151, 152, 153, 154, 155, 149, 164, 165, 166, 167, 168, 169, 163, 178, 179, 180, 181, 182, 183, 177, 192, 193, 194, 195, 196, 197, 191, 63, 97, 77, 49, 198, 199, 6, 114, 85, 87, 78, 0, 115, 65, 64, 116, 68, 91, 66, 69, 107, 130, 79, 15, 25, 81, 135, 103, 127, 41, 12, 96, 44, 92, 75, 74, 73, 72, 43, 71, 70, 90, 95, 117, 24, 104, 126, 48, 39, 38, 11, 17, 13, 45, 40, 35, 14, 42, 118, 105, 125, 8, 51, 10, 7, 47, 46, 9, 119, 106, 131, 132, 133, 134, 124, 122, 89, 88, 123, 121, 120};
    sequence_pair.h_sequence =   {138, 139, 140, 141, 137, 54, 33, 62, 28, 99, 94, 23, 61, 93, 136, 36, 80, 26, 59, 86, 56, 60, 19, 20, 98, 1, 135, 81, 25, 134, 88, 133, 132, 89, 131, 128, 129, 130, 110, 50, 76, 111, 67, 112, 27, 113, 109, 22, 82, 84, 30, 29, 34, 32, 31, 184, 170, 156, 142, 108, 52, 18, 53, 185, 21, 186, 187, 188, 189, 190, 171, 172, 173, 174, 83, 175, 176, 157, 158, 159, 160, 161, 162, 143, 144, 58, 145, 146, 147, 57, 5, 148, 107, 79, 15, 85, 87, 6, 63, 97, 49, 199, 198, 55, 191, 192, 193, 194, 195, 77, 196, 197, 177, 178, 179, 180, 181, 182, 183, 163, 164, 165, 166, 167, 168, 169, 149, 150, 151, 152, 2, 153, 16, 154, 155, 37, 122, 123, 121, 106, 124, 8, 51, 47, 46, 120, 105, 125, 10, 7, 48, 39, 9, 38, 119, 11, 17, 45, 14, 104, 126, 96, 13, 42, 92, 75, 74, 73, 40, 72, 71, 70, 35, 118, 103, 127, 3, 41, 12, 44, 43, 90, 4, 0, 24, 78, 65, 95, 64, 68, 91, 66, 117, 69, 100, 101, 102, 114, 115, 116};
    vector<int> module_wh_i =  {4, 2, 1, 1, 2, 4, 4, 3, 0, 0, 2, 2, 4, 3, 2, 2, 2, 0, 4, 3, 2, 1, 4, 0, 3, 0, 0, 4, 1, 0, 0, 0, 0, 0, 0, 2, 1, 1, 3, 0, 3, 4, 3, 4, 4, 3, 3, 4, 1, 1, 2, 1, 3, 0, 1, 2, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 3, 4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
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
    bool x = sequence_pair.find_position_with_area(true, false, 0,0);
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
