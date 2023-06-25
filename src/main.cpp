#include <iostream>

#include "fstream"
#include "static_data/chip_t.h"
#include "process/solver_t.h"
#include "output_data/output_handler_t.h"
#include "output_data/output_utility_t.h"
#include "process/sequence_pair_t.h"

int main(/*arg*/){
    std::fstream input_file;
    std::fstream output_file;

    //chip_t::file_input("../../testcase/custom-input9.txt");//fstream
    chip_t::file_input("../../testcase/case01-input.txt");//fstream
    sequence_pair_t::init();
    sequence_pair_enumerator_t sqen;
    sqen.seq.v_sequence = sqen.seq.h_sequence = {3,2};
    sqen.set_only_fix();
    sqen.seq.v_sequence=  {0,3,2,1};
    sqen.seq.h_sequence=  {2,1,0,3};
    //bool success = sqen.seq.add_soft_process(0);
//    std::cout<< success<<std::endl;
    sqen.seq.print_inline();
    auto [ok, pos] = sqen.seq.find_position(0,0);

    std::cout<<ok<<std::endl;
    if(ok){
        sqen.seq.sequence_pair_validation(pos);
        fgetc(stdin);
    }

    //seq.print_soft_inline();
}
