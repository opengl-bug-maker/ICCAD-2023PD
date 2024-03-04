#include "tester.h"
#include "process/case_table_t.h"
#include "process/sequence_pair/sequence_pair_t.h"
#include "process/quad_sequence/quad_sequence_t.h"
#include "process/functional/random_helper.h"
#include <iomanip>
#include "SA_solver_t.h"
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
    

    if(case_id!=-1){
        SP.v_sequence = case_table.cases[case_id][0];
        SP.h_sequence = case_table.cases[case_id][1];
    }
    for(auto& e:SP.is_in_seq){e = 1;}
    SP.find_position(true, true, 0, 0);
    SP.print_inline();
    SP.sequence_pair_validation();
    quad_sequence_t qs;
    qs.set_sequences(SP);
    //qs.build_constraint_graph_from_SP(SP);
    qs.to_polygon(SP);
    visualizer_t::draw_bounding_line(qs.modules_res);
}

void tester_t::test_sp(){
    case_table_t case_table;
    int case_id = chip_t::get_similar_case_num();
    sequence_pair_t SP;
    SA_solver_t SA_solver;
    if(case_id!=-1){
        
        SP.v_sequence = case_table.cases[case_id][0];
        SP.h_sequence = case_table.cases[case_id][1];
        for(auto& e:SP.is_in_seq){e = 1;}

        SP.find_position(true, true, 0, 0);
        //SP.find_position_allow_illegal(true, true, 0, 0);
        
        //SP.to_rectilinear();
        //SP.find_position_with_area(true, true, 0, 0);
        // SP = SA_solver.post_process(SP);
        // SP.fill_near();
        // SP.overlap_optimization();
        // SP.carve();
        // SP.set_bounding_lines();
        // //visualizer_t::draw_bounding_line(SP.bouding_lines);
        // visualizer_t::draw_bounding_line_connection(SP.bouding_lines);
        // SP.print_inline();
        // SP.print_result();
        // cout<<"Rectangle wirelength: "<<SP.predicted_wirelength<<endl;
        // cout<<"Rectilinear wirelength: "<<SP.rectilinear_wirelength<<endl;
        // cout<<std::setprecision(2)<<(SP.predicted_wirelength-SP.rectilinear_wirelength)/SP.predicted_wirelength*100<<"% optimization"<<endl;
        //SP.to_rectilinear_and_plot();
        //SP.sequence_pair_validation();
        SP.sequence_pair_validation();
    }
}

void tester_t::test_init(){
    sequence_pair_enumerator_t SPEN;
    SPEN.init_timeout = 1e10;
    SPEN.generate_sequence_pairs(1);
    sequence_pair_t SP = SPEN.valid_sequence_pairs[0];
    bool ok = SP.find_position(true, true, 0, 0);
    if(ok){
        cout<<std::setprecision(16)<<"Init success, got wirelength: "<<SP.z<<endl;
        //SP.predict_wirelength(true, true);
    }
    SP.sequence_pair_validation();

}