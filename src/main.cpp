#include <iostream>
#include <fstream>
#include <iomanip>

#include "static_data/chip_t.h"
#include "process/solver_t.h"
#include "process/floorplan/floorplan_t.h"
#include "output_data/output_handler_t.h"
#include "output_data/output_utility_t.h"
#include "plugin/visualizer_t.h"
#include "process/case_table_t.h"
#include "process/tester.h"
#include "plugin/argumentParser.h"
using namespace std;
void usage(int argc, char* argv[] ){}
int main(int argc, char** argv){
    argumentParser argument;
    try{
        argument = argumentParser(argc, argv, usage);
    }catch(std::exception &e){
        std::cerr << e.what() << std::endl;
    }

    std::string input_file_name = "";
    std::string output_file_name = "output.txt";
    if(argument.has("i")){
        input_file_name = argument.get("i");    
        chip_t::file_input(input_file_name, chip_t::file_type_t::iccad_pd);//fstream

    }
    if(argument.has("o")){
        output_file_name = argument.get("o");
    }
    tester_t tester;
    tester.test_sp();
    fgetc(stdin);
    std::cout << "press any key to end the program" << std::endl;
    return 0;




    
    solver_t solver;
    solver.run();
    return 0;
    // solver.load_specific_without_cmp();

    // floorplan_t fp = solver.get_best_fp();
    // fp.GUI_validation();
    // cout<< "FP wl : "<<fp.get_wirelength()<<endl;

    // std::fstream output_file;
    // output_file.open(output_file_name, std::ios::out | std::ios::trunc);

    // output_file << std::setiosflags(std::ios_base::fixed) << std::setprecision(1);

    // output_file << "HPWL " << output_handler_t::HPWL() << std::endl;

    // output_file << "SOFTMODULE " << chip_t::get_soft_modules().size();

    // for (output_utility_t& output_utility : output_handler_t::to_real()) {
    //     output_file << output_utility.to_string();
    // }
    // output_file.close();

    // fgetc(stdin);
    // std::cout << "press any key to end the program" << std::endl;
}
