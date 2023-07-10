#include <iostream>

#include "fstream"
#include "static_data/chip_t.h"
#include "process/solver_t.h"
#include "output_data/output_handler_t.h"
#include "output_data/output_utility_t.h"
#include "process/sequence_pair_t.h"
#include "process/timer.h"
int main(/*arg*/){
    timer main_timer("main");
    main_timer.timer_start();
    std::fstream input_file;
    std::fstream output_file;

    //chip_t::file_input("../../testcase/custom-input7.txt");//fstream
    chip_t::file_input("../../testcase/Case03.txt");//fstream
    //chip_t::file_input("../../testcase/case01-input.txt");//fstream
    sequence_pair_t::init();
    floorplan_t::init();
    sequence_pair_t sequence_pair_1;
    bool success = sequence_pair_1.add_soft_process(0);
    floorplan_t fp = sequence_pair_1.to_fp();
    fp.GUI_validation();

    main_timer.timer_end();
    main_timer.print_time_elapsed();

    fgetc(stdin);
    //seq.print_soft_inline();

}
