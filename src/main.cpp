#include <iostream>
#include <fstream>
#include <iomanip>

#include "static_data/chip_t.h"
#include "process/solver_t.h"

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
    if(argument.has("t")){
        tester_t tester;
        tester.test_sp();
        std::cout << "Press any key to end the program" << std::endl;
        fgetc(stdin);
        return 0;
    }
    else{
        solver_t solver;
        solver.run();
        return 0;
    }
}
