//
// Created by 林士傑 on 2023/3/27.
//

#include "genetic_algo_t.h"
//for debug
#include "iostream"
using std::cout;
using std::endl;
floorplanning_t& genetic_algo_t::get_best_fp() {
	return floorplannings[0];
}

void genetic_algo_t::run() {

    //first, let all fp grow
    clock_t start = clock();
    const int epoch = 100;
    for(size_t t = 0; t<epoch; ++t){
        for(auto& fp:floorplannings){
            growing(fp);
        }
        selection();
        if(get_best_fp().get_unplaced_id().size()==0){
             return;
        }
        for(int i = 0; i<100; ++i){
            floorplannings.push_back(mutation(floorplannings[i]));
        }
        while(floorplannings.size()<floorplanning_n){ //2000
            floorplannings.push_back(floorplanning_t());
        }
    }
    selection();
    clock_t end = clock();
    cout<<"GA execution time : "<<double(end-start)/CLOCKS_PER_SEC*1000<<" ms"<<endl;
}

genetic_algo_t::genetic_algo_t() {
    srand(time(NULL));
    floorplannings.resize(floorplanning_n);
}

void genetic_algo_t::print_info(bool detail) {
    cout<<"there are "<<floorplannings.size()<<" floorplannings"<<endl;
    for(size_t i =0; i<floorplannings.size(); ++i){
        cout<<"fp : "<<i<<" wirelength : "<<floorplannings[i].get_wirelength()<<" "<<endl;
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
    vec2d_t target_shape, target_lower_left_pos;
    for(int& id:selected_id){
        pair<vector<bounding_rectangle_t>, vector<bool>> bd_placed = fp.prepare_quad();
        quadratic_calculator.set_floopplanning(bd_placed.first,bd_placed.second);
        target_lower_left_pos = quadratic_calculator.get_coor_random(id);
        //target_center = quadratic_calculator.get_coor(id);

        //target_center = {rand()%chip_t::get_width(), rand()%chip_t::get_height()};
        vector<vec2d_t> shape_choices = fp.soft_area_to_w_h_m[id];
        size_t shape_id = rand()%(shape_choices.size());
        target_shape = shape_choices[shape_id];
        if(target_lower_left_pos.get_x()+target_shape.get_x()>chip_t::get_width()||target_lower_left_pos.get_x()<0){continue;}
        if(target_lower_left_pos.get_y()+target_shape.get_y()>chip_t::get_height()||target_lower_left_pos.get_y()<0){continue;}
        bool success = fp.place_soft_module(id,target_lower_left_pos, target_shape);
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

floorplanning_t genetic_algo_t::crossover(const floorplanning_t& fp1, const floorplanning_t& fp2) {
    floorplanning_t ret;
    for(int i = 0; i<fp1.soft_rects.size(); ++i){
        vec2d_t distance = fp1.soft_rects[i].getRect().get_center()-fp2.soft_rects[i].getRect().get_center();
        double hm_distance =  abs(distance.get_x())+abs(distance.get_y());
        if(hm_distance<0.1*(chip_t::get_width()+chip_t::get_height())){
            int x = rand()%2;
            if(x){
                ret.place_soft_module(i, fp1.soft_rects[i].getRect().get_left_lower(), fp1.soft_rects[i].getRect().get_size());
            }
            else{
                ret.place_soft_module(i, fp2.soft_rects[i].getRect().get_left_lower(), fp2.soft_rects[i].getRect().get_size());
            }

        }
    }
    return ret;
}

void genetic_algo_t::crossover_process() {
    for(int i = 0; i<greater_floorplanning_n; ++i){
        int x = rand()%greater_floorplanning_n;
        std::swap(floorplannings[i], floorplannings[x]);
    }

    for(int i = 0; i<greater_floorplanning_n; ++i){
        for(int j = i+1; j<greater_floorplanning_n; ++j){
            floorplanning_t fp = crossover(floorplannings[i], floorplannings[j]);
            floorplannings.push_back(fp);
        }
    }

}

floorplanning_t genetic_algo_t::mutation(const floorplanning_t& fp) {
    // discretization x
    std::set<uint16_t> x_coordinates;
    std::map<uint16_t, rect_t> m;
    floorplanning_t new_fp;
    x_coordinates.insert(chip_t::get_width());
    for(auto& bd_rect:fp.soft_rects){
        uint16_t x = bd_rect.getRect().get_left_lower().get_x();
        x_coordinates.insert(x);
    }
    for(auto& bd_rect:fp.fixed_rects){
        uint16_t x = bd_rect.getRect().get_left_lower().get_x();
        x_coordinates.insert(x);
    }
    for(size_t i = 0; i<fp.get_soft_rect_n(); ++i){
        if(fp.get_soft_is_placed()[i]==false){ continue;}
        rect_t origin_rect = fp.soft_rects[i].getRect();
        auto it = x_coordinates.upper_bound(origin_rect.get_right_upper().get_x());// find a x coordinate at right side
        bool fnd = 0;
        if(it!=x_coordinates.end()){
            rect_t target_rect( {*it-origin_rect.get_size().get_x(),origin_rect.get_left_lower().get_y()}, origin_rect.get_size());
            bool move_success = new_fp.place_soft_module(i,target_rect.get_left_lower(), target_rect.get_size());
            if(move_success){
                fnd = 1;
            }
        }
        if(!fnd){
            bool add_origin_success = new_fp.place_soft_module(i, origin_rect.get_left_lower(), origin_rect.get_size());
        }

    }
    return new_fp;
}

