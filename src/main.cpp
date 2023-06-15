#include <iostream>

#include "fstream"
#include "static_data/chip_t.h"
#include "process/solver_t.h"
#include "output_data/output_handler_t.h"
#include "output_data/output_utility_t.h"


int main(/*arg*/){
    std::fstream input_file;
    std::fstream output_file;

    chip_t::file_input("");//fstream

    floorplanning_t::init();
    solver_t solver;
    solver.run();
    floorplanning_t fp = solver.get_best_fp();
    output_handler_t::set_FP(solver.get_best_fp());

    output_file.open("", std::ios::out | std::ios::trunc);
    output_file << "WHPL " << output_handler_t::WHPL();

    for (output_utility_t& output_utility : output_handler_t::to_real()) {
        output_file << output_utility.to_string();
    }
    output_file.close();
    std::cout<<"Press any key to continue"<<std::endl;
    fgetc(stdin);
}
