#include "tester.h"
#include "process/case_table_t.h"
#include "process/sequence_pair/sequence_pair_t.h"
#include "process/quad_sequence/quad_sequence_t.h"
#include "process/functional/random_helper.h"
tester_t::tester_t(){
    random_helper::set_seed(); //build up random table

    floorplan_t::init(); //class initialization

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

void tester_t::test_qs()
{
    case_table_t case_table;
    int case_id = chip_t::get_similar_case_num();
    sequence_pair_t SP;
    bool fnd_cases = false;
    

    // SP.v_sequence = {6, 5, 14, 12, 13, 9, 11, 10, 8, 7, 4, 0, 3, 1, 2};
    // SP.h_sequence = {0, 4, 6, 5, 3, 8, 14, 7, 12, 1, 13, 9, 11, 10, 2};

    SP.v_sequence = {6, 5, 4, 0, 3, 1, 2};
    SP.h_sequence = {0, 4, 6, 5, 3, 1, 2};
    fnd_cases = true;
    for(auto& e:SP.is_in_seq){e = 1;}
    if(fnd_cases){
        SP.find_position(true, true, 0, 0);
        SP.print_inline();
        //SP.sequence_pair_validation();
        quad_sequence_t qs;
        qs.set_sequences(SP);
        qs.to_polygon();
        visualizer_t::draw_bounding_line(qs.modules_res);
    }
}

void tester_t::test_sp(){
    case_table_t case_table;
    int case_id = chip_t::get_similar_case_num();
    sequence_pair_t SP;
    if(case_id!=-1){
        SP.v_sequence = case_table.cases[case_id][0];
        SP.h_sequence = case_table.cases[case_id][1];
        for(auto& e:SP.is_in_seq){e = 1;}
        SP.find_position(true, true, 0, 0);
        SP.find_position_with_area(true, true, 0, 0);
        SP.print_inline();
        SP.sequence_pair_validation();
    }
}