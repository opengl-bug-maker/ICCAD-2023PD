#include <iostream>
#include <fstream>
#include <iomanip>

#include "static_data/chip_t.h"
#include "process/solver_t.h"
#include "process/floorplan/floorplan_t.h"
#include "output_data/output_handler_t.h"
#include "output_data/output_utility_t.h"
#include "process/quad_sequence/quad_sequence_t.h"
#include "plugin/visualizer_t.h"
#include "process/case_table_t.h"
#include "process/tester.h"
#include "macro_placer_lib.h"

macro_placer_t::macro_placer_t(string input_file_name){
    std::string output_file_name = "output.txt";
    chip_t::file_input(input_file_name, chip_t::file_type_t::iccad_pd);//fstream
    cout<<"Macro placer is running..."<<endl;
    tester_t tester;
    sequence_pair_t SP = tester.test_sp();
    this->result.modules_pos = SP.modules_positions;
    this->result.modules_wh = SP.modules_wh;


    this->matrix = vector<vector<int>>(this->matrix_size, vector<int>(this->matrix_size, 0));
    this->fill_matrix();
    //fgetc(stdin);
}

void macro_placer_t::normalization_by_chip_size(){
    double chip_w = chip_t::get_width();
    double chip_h = chip_t::get_height();
    for(auto& e:this->result.modules_wh){
        e.set_x(floor(e.get_x()/chip_w*this->matrix_size));
        e.set_y(floor(e.get_y()/chip_h*this->matrix_size));
    }
    for(auto& e:this->result.modules_pos){
        e.set_x(floor(e.get_x()/chip_w*this->matrix_size));
        e.set_y(floor(e.get_y()/chip_h*this->matrix_size));
    }
}
void macro_placer_t::print_matrix(){
    for(auto& row:this->matrix){
        for(auto& ele:row){
            cout<<ele<<" ";
        }
        cout<<endl;
    }
}
void macro_placer_t::fill_matrix()
{

    this->normalization_by_chip_size();
    //result.print();
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        int ll_x = this->result.modules_pos[i].get_x();
        int ll_y = this->result.modules_pos[i].get_y();
        for(int x = 0; x<result.modules_wh[i].get_x(); ++x){
            for(int y = 0; y<result.modules_wh[i].get_y(); ++y){
                int xx = max(min(this->matrix_size-1, ll_x+x), 0);
                int yy = max(min(this->matrix_size-1, ll_y+y), 0);
                int id = this->matrix[this->matrix_size-1-yy][xx];
                if(id!=0){
                    double ori_area = this->result.modules_wh[id-1].get_x()*this->result.modules_wh[id-1].get_y();
                    double area = this->result.modules_wh[i].get_x()*this->result.modules_wh[i].get_y();
                    if(area<ori_area){
                        this->matrix[this->matrix_size-1-yy][xx] = i+1;
                    }
                }
                else{this->matrix[this->matrix_size-1-yy][xx] = i+1;}
                
            }
        }
    }
    this->print_matrix();
}

void SP_result::print(){
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        cout<<i<<": "<<endl;
        cout<< "pos: "<<this->modules_pos[i]<<endl;
        cout<< "wh: "<<this->modules_wh[i]<<endl;
    }
}
