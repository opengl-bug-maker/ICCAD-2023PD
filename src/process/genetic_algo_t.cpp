//
// Created by 林士傑 on 2023/3/27.
//

#include "genetic_algo_t.h"
#include <algorithm>
#include <iterator>
#include <random>
//for debug
#include "iostream"
using std::cerr;
using std::endl;
floorplanning_t& genetic_algo_t::get_best_fp() {
	floorplanning_t fp;
	return fp;
}

void genetic_algo_t::run() {
    //first, let all fp grow
    floorplannings[0].print_info(true);
    growing(floorplannings[0]);
    floorplannings[0].print_info(true);
}

genetic_algo_t::genetic_algo_t() {
    srand(time(NULL));
    floorplannings.resize(floorplanning_n);
}

void genetic_algo_t::print_info() {
    cerr<<"there are "<<floorplanning_n<<" floorplannings"<<endl;
    for(size_t i =0; i<floorplanning_n; ++i){
        cerr<<"fp : "<<i<<" wirelength : "<<floorplannings[i].get_wirelength()<<" "<<endl;
    }
}

void genetic_algo_t::growing(floorplanning_t& fp) {
    vector<int> unplaced_id = fp.get_unplaced_id();
    vector<int> selected_id;   //choose some bounding rectangles to place
    std::sample(unplaced_id.begin(), unplaced_id.end(), std::back_inserter(selected_id), growing_rate,
                std::mt19937{std::random_device{}()}); //choose some id randomly
    //sample a shape
    vec2d_t target_shape, target_center;
    for(int& id:selected_id){
        pair<vector<bounding_rectangle_t>, vector<bool>> bd_placed = fp.prepare_quad();
        quadratic_calculator.set_floopplanning(bd_placed.first,bd_placed.second);
        //target_center = quadratic_calculator.get_coor_random(id);
        //target_center = quadratic_calculator.get_coor(id);
        target_center = {rand()%chip_t::get_width(), rand()%chip_t::get_height()};
        vector<vec2d_t> shape_choices = fp.soft_area_to_w_h_m[id];
        size_t shape_id = rand()%(shape_choices.size());
        target_shape = shape_choices[shape_id];
        if(target_center.get_x()+target_shape.get_half_x()>=chip_t::get_width()||target_center.get_x()-target_shape.get_half_x()<0){continue;}
        if(target_center.get_y()+target_shape.get_half_y()>=chip_t::get_height()||target_center.get_y()-target_shape.get_half_y()<0){continue;}
        bool success = fp.place_soft_module(id,target_center, target_shape);
    }

}

floorplanning_t genetic_algo_t::get_fp(size_t i) {
    return floorplannings[i];
}
