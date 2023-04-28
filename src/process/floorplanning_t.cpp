//
// Created by 林士傑 on 2023/3/27.
//

#include "floorplanning_t.h"
#include "fp_evaluator_t.h"
#include<string>

using std::string;
//for debug
#include<iostream>

using std::endl;
using std::cout;



unordered_map<const module_t*, int> floorplanning_t::module_to_bd_soft_rect_i_m;
unordered_map<const module_t*, int> floorplanning_t::module_to_bd_fixed_rect_i_m;
vector<vector<vec2d_t>> floorplanning_t::soft_area_to_w_h_m;
uint32_t floorplanning_t::min_w_h[2];
//size_t floorplanning_t::soft_rect_n;
//size_t floorplanning_t::fixed_rect_n;
void floorplanning_t::init() {
    uint16_t  soft_rect_n = chip_t::get_soft_modules().size();
    uint16_t fixed_rect_n = chip_t::get_fixed_modules().size();

    const std::vector<soft_module_t*>& soft_modules = chip_t::get_soft_modules();

    const std::vector<fixed_module_t*>& fixed_modules = chip_t::get_fixed_modules();

    soft_area_to_w_h_m.resize(soft_rect_n);
    for (int i = 0; i < soft_rect_n; ++i) {
        soft_area_to_w_h_m[i] = find_w_h(soft_modules[i]->get_area());
        floorplanning_t::module_to_bd_soft_rect_i_m[soft_modules[i]] = i;
    }
    min_w_h[0] = min_w_h[1] = INT_MAX;
    for(int i = 0; i<soft_rect_n; ++i) {
        for(auto& w_h:soft_area_to_w_h_m[i]){
            floorplanning_t::min_w_h[0] = std::min(floorplanning_t::min_w_h[0], static_cast<uint32_t>(w_h.get_x()));
            floorplanning_t::min_w_h[1] = std::min(floorplanning_t::min_w_h[1], static_cast<uint32_t>(w_h.get_y()));
        }
    }
    cout<< floorplanning_t::min_w_h[0]<<" "<<floorplanning_t::min_w_h[1]<<endl;
}

floorplanning_t::floorplanning_t() {
    wirelength = 0;
    const std::vector<soft_module_t*>& soft_modules = chip_t::get_soft_modules();
    const std::vector<fixed_module_t*>& fixed_modules = chip_t::get_fixed_modules();

    //load and make the soft module (undefined shape)
    soft_is_placed.resize(soft_modules.size());
    soft_area_to_w_h_m.resize(soft_modules.size());
    for (int i = 0; i < soft_modules.size(); ++i) {
        soft_is_placed[i] = false;
        bounding_rectangle_t bd = soft_modules[i]->make_bd();
        soft_area_to_w_h_m[i] = find_w_h(soft_modules[i]->get_area());

        floorplanning_t::module_to_bd_soft_rect_i_m[soft_modules[i]] = i;
        soft_rects.push_back(bd);
    }


    //load and place the fixed modules
    for (int i = 0; i < fixed_modules.size(); ++i) {
        bounding_rectangle_t bd = fixed_modules[i]->make_bd();
        floorplanning_t::module_to_bd_fixed_rect_i_m[fixed_modules[i]] = i;
        fixed_rects.push_back(bd);
        bool success = polygons.add_rect(fixed_rects[i]);
        if ( success== false) {  //if the fixed modules can't be placed, must be wrong.
            fp_status = fail_on_placing_fixed_modules;
            cout << "fail on placing fixed modules" << endl;
            return;
        }
    }

    cal_soft_deg();
    score = fp_evaluator_t::get_score(*this);
    //evaluate();

}


float floorplanning_t::bd_distance(const bounding_rectangle_t& a, const bounding_rectangle_t& b){
	rect_t rect_a = a.getRect();
	rect_t rect_b = b.getRect();
	vec2d_t dis = rect_a.get_center() - rect_b.get_center();
	return fabs(dis.get_x()) + fabs(dis.get_y()); //must be fabs
}
float floorplanning_t::VE_calculator(const bounding_rectangle_t& bd_rect,pair<const module_t* const, const int> neighbor){
	const module_t* neighbor_module = neighbor.first;
	const bounding_rectangle_t* neighbor_rect;
	int neighbor_rect_i = -1;
	float sum = 0;
	
	if (module_to_bd_soft_rect_i_m.count(neighbor_module)) {
		neighbor_rect_i = module_to_bd_soft_rect_i_m[neighbor_module];
		if (soft_is_placed[neighbor_rect_i] == true) { //if the module is not placed, skip this wirelength
			sum += bd_distance(soft_rects[neighbor_rect_i], bd_rect);
		}
	}
	if (module_to_bd_fixed_rect_i_m.count(neighbor_module)) {
		neighbor_rect_i = module_to_bd_fixed_rect_i_m[neighbor_module];
		sum += bd_distance(fixed_rects[neighbor_rect_i], bd_rect);
	}
	return sum*static_cast<float>(neighbor.second);
}

void floorplanning_t::calculate_wirelength(){
//	if (fp_status == fail_on_placing_fixed_modules) {
//		return;
//	}
	float sum = 0;
	for (int i = 0; i < fixed_rects.size(); ++i) {
		const module_t* fixed_module = fixed_rects[i].getLinkModule();
		const vector<std::pair<const module_t* const, const int>> neighbors 
			= (fixed_module)->getConnections();
		for (const auto neighbor : neighbors) {
			sum += VE_calculator(fixed_rects[i], neighbor);
		}
	}
	for (int i = 0; i < soft_rects.size(); ++i) {
		const module_t* soft_module = soft_rects[i].getLinkModule();
		if (soft_is_placed[i] == false) { continue; }
		const vector<std::pair<const module_t* const, const int>> neighbors
			= (soft_module)->getConnections();
		for (auto& neighbor : neighbors) {
			sum += VE_calculator(soft_rects[i], neighbor);
		}
	}
	
	wirelength = sum / 2;
	//wirelength = sum;
}
void floorplanning_t::print_info(bool position){
	if (fp_status == fail_on_placing_fixed_modules) {
		cout << "fail on placing fixed modules" << endl;
		return;
	}
    if(position){
        cout << "fixed module : " << endl;
        for (int i = 0; i < fixed_rects.size(); ++i) {
            cout << i << " : " << fixed_rects[i].getLinkModule()->getName() << endl;
        }

        cout << "soft module : " << endl;
        for (int i = 0; i < soft_rects.size(); ++i) {
            cout << i << " : " <<soft_rects[i].getLinkModule()->getName()<<" ";

            if(soft_is_placed[i]){
                cout<<"placed at "<<"{"<<soft_rects[i].getRect().get_center().get_x()<<", "
                <<soft_rects[i].getRect().get_center().get_y()<<"}"<<endl;
            }
            else{
                cout<<"not placed"<<endl;
            }
        }
    }

	get_wirelength();
	cout << "current wirelength : " << wirelength << endl;
    //cout<<"current score : "<<get_score()<<endl;
    cout<<"current score : "<<score<<endl;
}
float floorplanning_t::get_wirelength()
{
	calculate_wirelength();
	return wirelength;
}



bool floorplanning_t::place_soft_module(size_t i, vec2d_t lower_left_pos,vec2d_t size) {
	rect_t target_rect(lower_left_pos,size);
	//const soft_module_t* const target_module = dynamic_cast<const soft_module_t*>(soft_rects[i].getLinkModule());
	soft_module_t* target_module = (soft_module_t*)(soft_rects[i].getLinkModule()); //due to the const->non-const :(
	auto result = target_module->make_bd(target_rect);
	bounding_rectangle_t target_bd = result.first;
	bool success = false;

	if (result.second == true) {
		success = polygons.add_rect(target_bd);
	}
	if (success) {
		soft_is_placed[i] = true;
        soft_rects[i] = target_bd; //error here
	}
    else{
        soft_is_placed[i] = false;
    }
    //evaluate();
    score = fp_evaluator_t::get_score(*this);
	return success;
}

const size_t floorplanning_t::get_soft_rect_n() const
{
	return soft_rects.size();
}

vector<int> floorplanning_t::get_unplaced_id() {
    vector<int> ret;
    for(int i = 0; i<soft_rects.size(); ++i){
        if(soft_is_placed[i]==0){
            ret.push_back(i);
        }
    }
    return ret;
}


vector<vec2d_t> floorplanning_t::find_w_h(uint32_t area){
    uint32_t  from = sqrt(area/2);
    vector<vec2d_t> ret;
    for(uint32_t i = from; i<= sqrt(area); ++i){ //i will be the short edge
        if(area%i==0){
            int x = i, y = area/i;
            if(x*2>=y){
                ret.push_back({x, y});
                if(x!=y){
                    ret.push_back({y, x});
                }

            }
        }
    }
    if(ret.size()==0){
        int x = ceil(sqrt(area));
        return {{x,x}};
    }
    return ret;
}

pair<vector<bounding_rectangle_t>, vector<bool>> floorplanning_t::prepare_quad()  {
    vector<bounding_rectangle_t> bd_rects;
    vector<bool> is_placed;
    for(int i = 0; i<fixed_rects.size(); ++i){
        bd_rects.push_back(fixed_rects[i]);
        is_placed.push_back(true);
    }
    for(int i = 0; i<soft_rects.size(); ++i){
        bd_rects.push_back(soft_rects[i]);
        is_placed.push_back(soft_is_placed[i]);
    }
    return {bd_rects, is_placed};
}

//void floorplanning_t::evaluate() {
//    size_t deg_c = 0;
//    int wh = chip_t::get_width()+chip_t::get_height();
//    for(size_t i = 0; i<soft_rects.size(); ++i){
//        if(soft_is_placed[i]==false){
//            deg_c+= (soft_deg[i]*wh)+wh;
//        }
//    }
//    score = get_wirelength()+deg_c;
//}

const float floorplanning_t::get_score() const{
    return score;
}

const vector<uint32_t>& floorplanning_t::get_soft_deg() {
    return soft_deg;
}

void floorplanning_t::GUI_validation() {
    vector<bounding_rectangle_t> bd;
    vector<bounding_rectangle_t> placed_soft;
    bd.insert(bd.end(), this->fixed_rects.begin(), this->fixed_rects.end());
    //cout<<bd.size()<<endl;
    for(int i = 0; i<soft_rects.size(); ++i){
        if(soft_is_placed[i]){
            //cout<<soft_rects[i].getRect().get_size()<<" "<<soft_rects[i].getRect().get_center()<<endl;
            placed_soft.push_back(soft_rects[i]);
        }
    }
    bd.insert(bd.end(), placed_soft.begin(), placed_soft.end());
    visualizer_t::show_fp(bd);
}

void floorplanning_t::cal_soft_deg() {
    soft_deg.resize(soft_rects.size());
    for(int i = 0; i<soft_rects.size(); ++i){
        uint32_t deg = 0;
        const std::vector<std::pair<const module_t *const, const int>> neighbors = soft_rects[i].getLinkModule()->getConnections();
        for(auto& neighbor:neighbors){
            deg+=neighbor.second;
        }
        soft_deg[i] = deg;
    }
}

polygon_forest_t &floorplanning_t::get_polygon_forest() {
    return this->polygons;
}

const vector<bool> & floorplanning_t::get_soft_is_placed() const{
    return soft_is_placed;
}

const size_t floorplanning_t::get_fixed_rect_n() const {
    return fixed_rects.size();
}
