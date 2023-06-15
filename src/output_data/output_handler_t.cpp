//
// Created by 林士傑 on 2023/3/27.
//

#include "output_handler_t.h"

int output_handler_t::whpl;
std::vector<output_utility_t> output_handler_t::utilities;

void output_handler_t::parse_polygon(polygon_t &polygon) {
    for (auto rect : polygon.get_rects()) {
        utilities.push_back(output_utility_t(rect->get_module_bounding_rectangle().getLinkModule()->getName(), {
            rect->get_module_bounding_rectangle().getRect().get_left_lower(),
            vec2d_t(rect->get_module_bounding_rectangle().getRect().get_right_upper().get_x(),
                    rect->get_module_bounding_rectangle().getRect().get_left_lower().get_y()),
            rect->get_module_bounding_rectangle().getRect().get_right_upper(),
            vec2d_t(rect->get_module_bounding_rectangle().getRect().get_left_lower().get_x(),
                    rect->get_module_bounding_rectangle().getRect().get_right_upper().get_y())
        }));
    }
}

void output_handler_t::set_FP(floorplanning_t &floorplanning) {
    polygon_forest_t& polygonForest = floorplanning.get_polygon_forest();
//    polygon_forest_t polygonForest;
    output_handler_t::whpl = floorplanning.get_wirelength();//float to int?
    for(auto poly : polygonForest.get_polygons()){
        output_handler_t::parse_polygon(poly);
    }
}

int output_handler_t::WHPL() {
    return output_handler_t::whpl;
}

std::vector<output_utility_t> output_handler_t::to_real() {
    return output_handler_t::utilities;
}

void output_handler_t::test(polygon_forest_t& polygonForest) {
//    polygon_forest_t& polygonForest ;
//    output_handler_t::whpl = floorplanning.get_wirelength();//float to int?
    for(auto poly : polygonForest.get_polygons()){
        output_handler_t::parse_polygon(poly);
    }
    auto aa = output_handler_t::to_real();
    auto b = aa[0].to_string();
    auto bb = aa[1].to_string();
    std::cout << b << std::endl;
    std::cout << bb << std::endl;
    int i = 0;
    i++;
}
