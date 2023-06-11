#include <iostream>

#include "fstream"
#include "static_data/chip_t.h"
#include "process/solver_t.h"
#include "output_data/output_handler_t.h"
#include "output_data/output_utility_t.h"


#include "polygon/lenf_unit.h"
#include "utilities/lenf_quadtree_t.h"
#include "static_data/soft_module_t.h"
#include "polygon/lenf_polygon_t.h"

class tes{
    int a;
    int b;
public:
    tes(int a, int b) : a(a), b(b) {}
};

int main(/*arg*/){

//    lenf_quadtree_t<lenf_unit> tree(rect_t(vec2d_t(0,0), vec2d_t(20, 20)));
//    tree.add_value(lenf_unit(soft_module_t::void_module.make_bd(rect_t(vec2d_t(0,0), vec2d_t(5, 5))).first));
//    tree.add_value(lenf_unit(soft_module_t::void_module.make_bd(rect_t(vec2d_t(6,6), vec2d_t(2, 2))).first));
//    tree.add_value(lenf_unit(soft_module_t::void_module.make_bd(rect_t(vec2d_t(8,0), vec2d_t(5, 5))).first));
//    tree.add_value(lenf_unit(soft_module_t::void_module.make_bd(rect_t(vec2d_t(0,7), vec2d_t(4, 4))).first));
//    tree.add_value(lenf_unit(soft_module_t::void_module.make_bd(rect_t(vec2d_t(0,0), vec2d_t(2, 2))).first));
//    tree.add_value(lenf_unit(soft_module_t::void_module.make_bd(rect_t(vec2d_t(1,1), vec2d_t(2, 2))).first));
//    tree.add_value(lenf_unit(soft_module_t::void_module.make_bd(rect_t(vec2d_t(0,1), vec2d_t(2, 2))).first));
//    tree.add_value(lenf_unit(soft_module_t::void_module.make_bd(rect_t(vec2d_t(1,0), vec2d_t(2, 2))).first));
//    tree.print();

//    return 0;



    std::fstream input_file;
    std::fstream output_file;

    chip_t::file_input("../../testcase/custom-input2.txt");//fstream

    lenf_polygon_t polygon(chip_t::get_soft_modules()[3]->make_bd(rect_t(vec2d_t(0, 0), vec2d_t(5, 5))).first);
    lenf_polygon_t polygon1(chip_t::get_soft_modules()[3]->make_bd(rect_t(vec2d_t(4, 4), vec2d_t(5, 5))).first);
    lenf_polygon_t polygon2(chip_t::get_soft_modules()[3]->make_bd(rect_t(vec2d_t(4, 0), vec2d_t(5, 5))).first);
    lenf_polygon_t polygon3(chip_t::get_soft_modules()[3]->make_bd(rect_t(vec2d_t(0, 4), vec2d_t(5, 5))).first);

    polygon1.merge_polygon(polygon);
    polygon2.merge_polygon(polygon1);
    polygon3.merge_polygon(polygon2);


    return 0;

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
