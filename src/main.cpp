#include <iostream>

#include "fstream"
#include "static_data/chip_t.h"
#include "process/genetic_algo_t.h"
#include "output_data/output_handler_t.h"
#include "output_data/output_utility_t.h"


int main(/*arg*/){
    std::fstream input_file;
    std::fstream output_file;

    chip_t::file_input("../../testcase/custom-input2.txt");//fstream

    floorplanning_t::init();
    genetic_algo_t genetic_algo;
    genetic_algo.run();
    floorplanning_t fp = genetic_algo.get_best_fp();
    output_handler_t::set_FP(genetic_algo.get_best_fp());

    output_file.open("../../custom-output2.txt", std::ios::out | std::ios::trunc);
    output_file << "WHPL " << output_handler_t::WHPL();

    for (output_utility_t& output_utility : output_handler_t::to_real()) {
        output_file << output_utility.to_string();
    }
    output_file.close();
    system("PAUSE");
}
