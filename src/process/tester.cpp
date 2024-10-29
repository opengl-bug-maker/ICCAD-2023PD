#include "tester.h"
#include "process/case_table_t.h"
#include "process/sequence_pair/sequence_pair_t.h"
#include "process/functional/random_helper.h"
#include "SA_solver_t.h"
#include <iomanip>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;
tester_t::tester_t(){
    random_helper::set_seed(); //build up random table

    sequence_pair_t::init(); //class initialization

    std::ios_base::sync_with_stdio(false); //make cout faster
    std::cin.tie(0);
    cout.tie(0);

    if(chip_t::get_total_module_n()<1){
        return;
    }
}

void tester_t::test()
{
}
void tester_t::test_sp(){
    case_table_t case_table;
    int case_id = chip_t::get_similar_case_num();
    sequence_pair_t SP;
    if(case_id!=-1){
        SP.v_sequence = case_table.cases[case_id][0];
        SP.h_sequence = case_table.cases[case_id][1];
        for(auto& e:SP.is_in_seq){e = 1;}
        bool success = SP.find_position_allow_illegal_process();
        //bool success = SP.find_position(true, true, 0, 0);
        if(success==false){
            cout<<"Unable to initialize"<<endl;
            return;
        }
        double z = SP.get_wirelength();
        SP.sequence_pair_validation();
        SP.to_rectilinear_and_plot();
        SP.print_inline();
    }
    else{
        cout<< "This is not a default testcase, can't load a predetermined sequence pair. "<<endl;
    }
}