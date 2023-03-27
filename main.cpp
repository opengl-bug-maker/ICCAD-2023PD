#include <iostream>

#include "fstream"
#include "static_data/chip_t.h"
#include "process/genetic_algo_t.h"
#include "output_data/output_handler_t.h"
#include "output_data/output_utility_t.h"


int main(/*arg*/){
    std::fstream input_file;
    std::fstream output_file;

    chip_t chip;
    chip.file_input(NULL);//fstream

    genetic_algo_t genetic_algo(chip);
    genetic_algo.run();
    output_handler_t::set_FP(genetic_algo.get_best_fp());
    output_file << "WHPL " << output_handler_t::WHPL() << std::endl;

    for (output_utility_t& output_utility : output_handler_t::to_real()) {
        output_file << output_utility.to_string();
    }
    output_file.close();
}
