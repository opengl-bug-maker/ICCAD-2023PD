//
// Created by 林士傑 on 2023/3/27.
//

#include "genetic_algo_t.h"
//for debug
#include "iostream"
using std::cerr;
using std::endl;
floorplanning_t& genetic_algo_t::get_best_fp() {
	return floorplannings[0];
}

void genetic_algo_t::run() {
    //test growing
//    floorplannings[0].print_info(true);
//    growing(floorplannings[0]);
//    floorplannings[0].print_info(true);


//
//    for(int i = 0; i<10; ++i){
//        growing(floorplannings[0]);
//    }
//    for(int i = 0; i<10; ++i){
//        growing(floorplannings[1]);
//    }
    //first, let all fp grow
    clock_t start = clock();
    const int epoch = 10;
    for(size_t t = 0; t<epoch; ++t){
        for(auto& fp:floorplannings){
            growing(fp);
        }

        selection();

        //clock_t s3 = clock();
        while(floorplannings.size()<floorplanning_n){ //2000
            floorplannings.push_back(floorplanning_t());
        }
    }
    selection();
    clock_t end = clock();
    cerr<<"GA execution time : "<<double(end-start)/CLOCKS_PER_SEC*1000<<" ms"<<endl;
}

genetic_algo_t::genetic_algo_t() {
    srand(time(NULL));
    floorplannings.resize(floorplanning_n);
}

void genetic_algo_t::print_info(bool detail) {
    cerr<<"there are "<<floorplannings.size()<<" floorplannings"<<endl;
    for(size_t i =0; i<floorplannings.size(); ++i){
        cerr<<"fp : "<<i<<" wirelength : "<<floorplannings[i].get_wirelength()<<" "<<endl;
    }
}

void genetic_algo_t::growing(floorplanning_t& fp) {
    vector<int> unplaced_id = fp.get_unplaced_id();
    vector<int> selected_id;   //choose some bounding rectangles to place



//    std::sample(unplaced_id.begin(), unplaced_id.end(), std::back_inserter(selected_id), growing_rate,
//                std::mt19937{std::random_device{}()}); //choose some id randomly //too slow :(

    for(int t = 0;  t<unplaced_id.size(); ++t){
        int x = rand()%unplaced_id.size();
        std::swap(unplaced_id[x], unplaced_id[t]);
    }
    selected_id = vector<int>(unplaced_id.begin(), unplaced_id.begin()+std::min(growing_rate, static_cast<int>(unplaced_id.size())));

    //sample a shape
    vec2d_t target_shape, target_center;
    for(int& id:selected_id){
        pair<vector<bounding_rectangle_t>, vector<bool>> bd_placed = fp.prepare_quad();
        quadratic_calculator.set_floopplanning(bd_placed.first,bd_placed.second);
        target_center = quadratic_calculator.get_coor_random(id);
        //target_center = quadratic_calculator.get_coor(id);

        //target_center = {rand()%chip_t::get_width(), rand()%chip_t::get_height()};
        vector<vec2d_t> shape_choices = fp.soft_area_to_w_h_m[id];
        size_t shape_id = rand()%(shape_choices.size());
        target_shape = shape_choices[shape_id];
        if(target_center.get_x()+target_shape.get_half_x()>chip_t::get_width()||target_center.get_x()-target_shape.get_half_x()<0){continue;}
        if(target_center.get_y()+target_shape.get_half_y()>chip_t::get_height()||target_center.get_y()-target_shape.get_half_y()<0){continue;}
        bool success = fp.place_soft_module(id,target_center, target_shape);
    }

}

floorplanning_t genetic_algo_t::get_fp(size_t i) {
    return floorplannings[i];
}

void genetic_algo_t::selection() {
    std::sort(floorplannings.begin(), floorplannings.end(), [&](floorplanning_t&fp1, floorplanning_t& fp2){
        return fp1.get_score()<fp2.get_score();
    });
    while(floorplannings.size()>greater_floorplanning_n){
        floorplannings.pop_back();
    }
}
