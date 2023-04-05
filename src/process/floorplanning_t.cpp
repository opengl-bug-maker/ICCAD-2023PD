#include "floorplanning_t.h"
#include "floorplanning_t.h"
#include "floorplanning_t.h"
//
// Created by 林士傑 on 2023/3/27.
//

#include "floorplanning_t.h"
#include<string>
using std::string;
//for debug
#include<iostream>
#define FP_DEBUG

using std::endl;
using std::cerr;


float floorplanning_t::bd_distance(const bounding_rectangle_t& a, const bounding_rectangle_t& b) {
	rect_t rect_a = a.getRect();
	rect_t rect_b = b.getRect();
	vec2d_t dis = rect_a.get_center() - rect_b.get_center();
#ifdef FP_DEBUG
	cerr << a.getLinkModule()->getName()<<" "<< b.getLinkModule()->getName() <<" "<< abs(dis.get_x()) + abs(dis.get_y()) << endl;
#endif
	return abs(dis.get_x()) + abs(dis.get_y());
}
uint64_t floorplanning_t::VE_calculator(const bounding_rectangle_t& bd_rect,pair<const module_t* const, const int> neighbor) {
	const module_t* neighbor_module = neighbor.first;
	const bounding_rectangle_t* neighbor_rect;
	int neigbor_rect_i = -1;
	float sum = 0;
	
	if (module_to_bd_soft_rect_i_m.count(neighbor_module)) {
		neigbor_rect_i = module_to_bd_soft_rect_i_m[neighbor_module];
		if (soft_is_placed[neigbor_rect_i] == true) { //if the module is not placed, skip this wirelength
			sum += bd_distance(soft_rects[neigbor_rect_i], bd_rect);
		}
	}
	if (module_to_bd_fixed_rect_i_m.count(neighbor_module)) {
		neigbor_rect_i = module_to_bd_fixed_rect_i_m[neighbor_module];
		sum += bd_distance(fixed_rects[neigbor_rect_i], bd_rect);
	}
	return sum*static_cast<float>(neighbor.second);
}

void floorplanning_t::calculate_wirelength(){
	if (fp_status == fail_on_placing_fixed_modules) {
		return;
	}
	float sum = 0;
	for (int i = 0; i < fixed_rect_n; ++i) {
		const module_t* fixed_module = fixed_rects[i].getLinkModule();
		const vector<std::pair<const module_t* const, const int>> neighbors 
			= (fixed_module)->getConnections();
		for (const auto neighbor : neighbors) {
			sum += VE_calculator(fixed_rects[i], neighbor);
		}
	}
	for (int i = 0; i < soft_rect_n; ++i) {
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
void floorplanning_t::print_info(){
	if (fp_status == fail_on_placing_fixed_modules) {
		cerr << "fail on placing fixed modules" << endl;
		return;
	}
	cerr << "fixed module : " << endl;
	for (int i = 0; i < fixed_rect_n; ++i) {
		cerr << i << " : " << fixed_rects[i].getLinkModule()->getName() << endl;
	}

	cerr << "soft module : " << endl;
	for (int i = 0; i < soft_rect_n; ++i) {
		string placed = soft_is_placed[i] ? "placed" : "not placed";
		cerr << i << " : " << soft_rects[i].getLinkModule()->getName()<<" "<<placed << endl;
	}
	cerr << endl;
	get_wirelength();
	cerr << "current wirelength : " << wirelength << endl;;
}
//
floorplanning_t::floorplanning_t() {
	wirelength = 0;
	soft_rect_n = chip_t::get_soft_modules().size();
	fixed_rect_n = chip_t::get_fixed_modules().size();
	
	const std::vector<soft_module_t*>& soft_modules = chip_t::get_soft_modules();

	const std::vector<fixed_module_t*>& fixed_modules = chip_t::get_fixed_modules();

	//load and make the soft module (undefined shape)
	soft_is_placed.resize(soft_rect_n);

	for (int i = 0; i < soft_rect_n; ++i) {
		soft_is_placed[i] = false;
		bounding_rectangle_t bd = soft_modules[i]->make_bd();
		module_to_bd_soft_rect_i_m[soft_modules[i]] = i;
		soft_rects.push_back(bd);
	}


	//load and place the fixed modules
	for (int i = 0; i < fixed_rect_n; ++i) {
		bounding_rectangle_t bd = fixed_modules[i]->make_bd();
		module_to_bd_fixed_rect_i_m[fixed_modules[i]] = i;
		fixed_rects.push_back(bd);

		bool success = polygons.add_rect(fixed_rects[i]);
		if ( success== false) {  //if the fixed modules can't be placed, must be wrong.
			fp_status = fail_on_placing_fixed_modules;
			cerr << "fail on placing fixed modules" << endl;
			return;
		}
	}

	/*
	* for(each soft_rect_i in soft_rects){ //V^2
	*	for(each soft_rect_j in soft_rects){
	*		 sum+=|soft_rect_i.center-soft_rect_j.center|*connectivity
	*	}
	* }
	* 
	* for(each soft_rect in soft_rects){ // V*E
	*		for(each neighbor in soft_rect){
	*		 sum+=|soft_rect.center-"neighbor.center" |*connectivity
	*	}
	* }
	*/
}

// adj(module* a, module* b) = connectivity
float floorplanning_t::get_wirelength()
{
	calculate_wirelength();
	return wirelength;
}



bool floorplanning_t::place_soft_module(size_t i, vec2d_t center,vec2d_t size) {
	rect_t target_rect(center,size);
	//const soft_module_t* const target_module = dynamic_cast<const soft_module_t*>(soft_rects[i].getLinkModule());
	soft_module_t* target_module = (soft_module_t*)(soft_rects[i].getLinkModule()); //due to the const->non-const :(
	auto result = target_module->make_bd(target_rect);
	bounding_rectangle_t target_bd = result.first;
	bool success = false;
	//cerr << "result :" << result.second << endl;

	if (result.second == true) {
		success = polygons.add_rect(target_bd);
		//cerr << "add :" << success << endl;
	}
	if (success) {
		soft_is_placed[i] = true;
	}
	soft_rects[i] = target_bd; //error here
	auto c = target_bd.getRect().get_center();
	return success;
}

const size_t& floorplanning_t::get_soft_rect_n()
{
	return soft_rects.size();
}
