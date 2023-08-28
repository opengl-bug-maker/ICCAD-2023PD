//
// Created by 林士傑 on 2023/8/28.
//

#include "carver.h"
#include "static_data/chip_t.h"
#include "static_data/module_t.h"
#include "static_data/soft_module_t.h"
#include "static_data/fixed_module_t.h"

polygon_forest_t carver::carving(polygon_forest_t& polygon_forest) {
    this->rects = std::vector<rect_t>(chip_t::get_total_module_n(), rect_t(vec2d_t::default_position, vec2d_t::default_position));
    for(auto poly : polygon_forest.get_polygons()){
        if(poly.get_rects().size() != 1){
            auto why = 0;
//            std::cout << "whywhywhywhywhyhwhyhwyhwhyhwhy";
        }
        for(auto po : poly.get_rects()){
            this->rects[chip_t::get_name_to_index_mapping().at(po->module_rect.getLinkModule()->getName())] = po->module_rect.getRect();
        }
    }
    for (int i = 0; i < chip_t::get_soft_modules().size(); ++i) {
        //carving
        auto limit_area = chip_t::get_modules()[i]->get_area();
        auto minus_rect = this->rects[i];
        auto new_length = 0;
        if(this->rects[i].get_size().get_y() > this->rects[i].get_size().get_x()){
            minus_rect = rect_t(this->rects[i].get_left_lower(), this->rects[i].get_size() - vec2d_t(1, 0));
            while(minus_rect.get_size().get_y() < 2 * minus_rect.get_size().get_x() && minus_rect.get_area() >= limit_area){
                this->rects[i] = minus_rect;
                minus_rect = rect_t(this->rects[i].get_left_lower(), this->rects[i].get_size() - vec2d_t(1, 0));
            }
            new_length = (int)(limit_area / this->rects[i].get_size().get_x()) + 1;
            this->rects[i] = rect_t(this->rects[i].get_left_lower(), vec2d_t(this->rects[i].get_size().get_x(), new_length));
        }else{
            minus_rect = rect_t(this->rects[i].get_left_lower(), this->rects[i].get_size() - vec2d_t(0, 1));
            while(minus_rect.get_size().get_x() < 2 * minus_rect.get_size().get_y() && minus_rect.get_area() >= limit_area){
                this->rects[i] = minus_rect;
                minus_rect = rect_t(this->rects[i].get_left_lower(), this->rects[i].get_size() - vec2d_t(0, 1));
            }
            new_length = (int)(limit_area / this->rects[i].get_size().get_y()) + 1;
            this->rects[i] = rect_t(this->rects[i].get_left_lower(), vec2d_t(new_length, this->rects[i].get_size().get_y()));
        }
    }
    for (int i = 0; i < chip_t::get_total_module_n(); ++i) {
        this->polygon_forest.add_rect(chip_t::get_modules()[i]->make_bd(this->rects[i]).first);
    }
    return this->polygon_forest;
}

int carver::push_test(int index, rect_t rect) {
    if( rect.get_left_lower().get_x() < 0 ||
        rect.get_left_lower().get_y() < 0 ||
        rect.get_right_upper().get_x() > chip_t::get_width() ||
        rect.get_right_upper().get_y() > chip_t::get_height()){
        return -2;
    }
    for(int i = 0; i < this->rects.size(); i++){
        if(i == index) continue;
        if(rect.is_collision(this->rects[i])) return i;
    }
    return -1;
}

bool carver::push(int index, int dir) {
//    std::cout << "push : " << index << " " << dir << " | " << this->rects[index] << " start push============\n";
//    std::cout.flush();
    if(index >= chip_t::get_soft_modules().size()){
//        std::cout << "fixed out\n";
        return false;
    }
    auto next_rect = rect_t(vec2d_t::default_position, vec2d_t::default_position);
    switch (dir) {
        case 0:
            next_rect = rect_t(this->rects[index].get_left_lower() - vec2d_t(1, 0), this->rects[index].get_size());
            break;
        case 1:
            next_rect = rect_t(this->rects[index].get_left_lower() + vec2d_t(1, 0), this->rects[index].get_size());
            break;
        case 2:
            next_rect = rect_t(this->rects[index].get_left_lower() - vec2d_t(0, 1), this->rects[index].get_size());
            break;
        case 3:
            next_rect = rect_t(this->rects[index].get_left_lower() + vec2d_t(0, 1), this->rects[index].get_size());
            break;
        default:
            //why
            break;
    }
    double origin_value = 0;
    double new_value = 0;
    for(int i = 0; i < chip_t::get_total_module_n(); ++i){
        if(this->rects[i] == rect_t(vec2d_t::default_position, vec2d_t::default_position)) continue;
        auto ori_dis = this->rects[index].get_center() - this->rects[i].get_center();
        auto new_dis = next_rect.get_center() - this->rects[i].get_center();
        origin_value += (double)chip_t::get_connection_table()[index][i] * (std::fabs(ori_dis.get_x()) + std::fabs(ori_dis.get_y()));
        new_value += (double)chip_t::get_connection_table()[index][i] * (std::fabs(new_dis.get_x()) + std::fabs(new_dis.get_y()));
    }
    if(new_value > origin_value) return false;
    int test = this->push_test(index, next_rect);
    if(test == -2){
//        std::cout << "coll"
        return false;
    }
    if(test == -1){
        this->rects[index] = next_rect;
    }else{
        auto old_rect = this->rects[index];
        this->rects[index] = rect_t(vec2d_t::default_position, vec2d_t::default_position);
        auto result = this->push(test, dir);
        if(!result){
            this->rects[index] = old_rect;
            return false;
        }
        this->rects[index] = next_rect;
        return true;
    }
    auto re = this->push(index, dir);
    if(!re){
        return false;
    }
//    this->rects[index] = next_rect;
    return true;
}

polygon_forest_t carver::pushing(polygon_forest_t& polygon_forest) {
    this->rects = std::vector<rect_t>(chip_t::get_total_module_n(), rect_t(vec2d_t::default_position, vec2d_t::default_position));
    for(auto poly : polygon_forest.get_polygons()){
        if(poly.get_rects().size() != 1){
            auto why = 0;
//            std::cout << "whywhywhywhywhyhwhyhwyhwhyhwhy";
        }
        for(auto po : poly.get_rects()){
            this->rects[chip_t::get_name_to_index_mapping().at(po->module_rect.getLinkModule()->getName())] = po->module_rect.getRect();
        }
    }
    for (int i = 0; i < chip_t::get_soft_modules().size(); ++i) {
        for (int j = 0; j < 4; ++j) {
            push(i, j);
        }
    }
    this->polygon_forest = polygon_forest_t();
    for (int i = 0; i < chip_t::get_total_module_n(); ++i) {
        this->polygon_forest.add_rect(chip_t::get_modules()[i]->make_bd(this->rects[i]).first);
    }
    return this->polygon_forest;
}
