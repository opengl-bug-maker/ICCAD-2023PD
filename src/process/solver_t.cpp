//
// Created by 林士傑 on 2023/3/27.
//

#include "solver_t.h"
//for debug
#include "iostream"
#include "genetic_solver_t.h"
#include <iomanip>
using std::cout;
using std::endl;


void solver_t::run() {
    sequence_pair_t::init();
    floorplan_t::init();



//    genetic_solver_t genetic_solver(10, 5);
//    genetic_solver.run();
//    floorplan_t fp = genetic_solver.best_sequence_pair.to_fp();
//    cout<< "final wirelength : "<<std::setprecision(16)<<fp.get_wirelength()<<endl;
//    this->best_fp = fp;
//    cout<<endl;



    sequence_pair_t sequence_pair;
    for(int i = 0; i<sequence_pair_t::soft_area_to_w_h_m.size(); ++i){
        cout<< sequence_pair.modules_area[i]<<endl;
        for(int j = 0; j<sequence_pair_t::soft_area_to_w_h_m[i].size(); ++j){
            cout<< j<<" ";
            auto e = sequence_pair_t::soft_area_to_w_h_m[i][j];
            cout<<e.get_x()<<" "<<e.get_y()<<endl;
        }
        cout<<endl;
    }
    bool success = sequence_pair.add_soft_process(0);

    if(success){
        for(auto& e: sequence_pair.is_in_seq){e = 1;}
        sequence_pair.load_best_sequence();
        bool useless = sequence_pair.find_position(true, true, 0, 0);
        sequence_pair.predict_wire_length(true);
        cout<<sequence_pair.predicted_wirelength<<endl;
        cout<<"pos"<<endl;
        for(auto& e:sequence_pair.positions){
            cout<<e.get_x()<<" "<<e.get_y()<<endl;
        }
        cout<<"WH"<<endl;
        for(int i = 0; i<sequence_pair_t::soft_n; ++i){
            cout<< sequence_pair_t::seq_soft_map[i]->getName()<<"->";
            double x = sequence_pair.modules_wh[i].get_x();
            double y = sequence_pair.modules_wh[i].get_y();
            double a = x*y;
            cout<<"("<<sequence_pair.modules_wh[i].get_x()<<", "<<sequence_pair.modules_wh[i].get_y()<<")";
            cout<<"->"<<std::setprecision(16)<<a<<", "<< sequence_pair_t::seq_soft_map[i]->get_area();
            if(a>sequence_pair_t::seq_soft_map[i]->get_area()){
                cout<<" left "<<a-sequence_pair_t::seq_soft_map[i]->get_area()<<endl;
                cout<<"waste "<<(a-sequence_pair_t::seq_soft_map[i]->get_area())/a*100<<"%"<<endl;
            }
            else{cout<<" need "<<sequence_pair_t::seq_soft_map[i]->get_area()-a<<endl;}
        }
        cout<<"SUCCESS"<<endl;
        sequence_pair.print_inline();
        sequence_pair.sequence_pair_validation();
        //fgetc(stdin);
        cout<< "predicted wirelength : "<<sequence_pair.predicted_wirelength<<endl;
        floorplan_t fp = sequence_pair.to_fp();
        cout<< "final wirelength : "<<std::setprecision(16)<<fp.get_wirelength()<<endl;
        this->best_fp = fp;
        cout<<endl;
    }
    else{cout<<"FAIL"<<endl; return;}


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
