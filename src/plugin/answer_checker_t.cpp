//
// Created by 林士傑 on 2023/3/27.
//

#include "answer_checker_t.h"

unsigned long long int answer_checker_t::hpwl;
std::vector<unit> answer_checker_t::units;

bool answer_checker_t::read_file(std::string output_file) {
    std::fstream output;
    output.open(output_file);

    std::string name;

    output >> name;
    if(name != "HPWL"){
        output.close();
        throw checker_exception("HPWL NOT FOUND");
    }
    output >> answer_checker_t::hpwl;

    while(output >> name){
        unsigned long index;
        try {
            index = chip_t::get_name_to_index_mapping().at(name);
        } catch (std::exception e){
            throw checker_exception("Module - " + name + " : NOT FOUND");
        }

        int times; output >> times;
        auto u = unit(name, chip_t::get_modules()[index]);
        int temp;
        vec2d_t v;
        while(times--){
            output >> temp;
            v.set_x(temp);
            output >> temp;
            v.set_y(temp);
            u.addPoint(v);
        }
        try {
            u.is_valid();
        } catch (checker_exception e){
            output.close();
            throw e;
        }
    }

    output.close();
    return true;
}

bool answer_checker_t::test(std::string output_file) {
    try {
        read_file(output_file);
    } catch (checker_exception e){
        std::cout << e.what << std::endl;
        return false;
    }
    return true;
}

checker_exception::checker_exception(const std::string &what) : what(what) {}

unit::unit(const std::string &name, module_t* link) : name(name), link(link), bounding_rect(vec2d_t::default_position, vec2d_t::default_position) {}

void unit::addPoint(vec2d_t vec2D) {
    this->points.push_back(vec2D);
}

void unit::is_valid(){
    if(this->points.size() < 4) throw checker_exception("point count : " + std::to_string(this->points.size()) + ", too less.");

    std::vector<line_t> lines;
    lines.reserve(this->points.size());

    //generate lines
    for(int i = 0; i < this->points.size() - 1; ++i) {
        lines.emplace_back(this->points[i], this->points[i + 1]);
    }
    lines.emplace_back(this->points.back(), this->points.front());

    //check type
    for(auto li : lines){
        if(li.getType() == line_t::line_type::none) throw checker_exception(this->link->getName() + " have invalid line.");
    }

    //check intersect
    for (int i = 0; i < lines.size(); ++i) {
        for (int j = i + 2; j < lines.size(); ++j) {
            if(lines[i].intersect(lines[j]).first){
                throw checker_exception(this->link->getName() + " : line intersect!");
            }
        }
    }

    //check corner
    line_t::line_type type = lines.front().getType();
    for(auto li : lines){
        if(li.getType() == type){
            if(type == line_t::line_type::vertical) type = line_t::line_type::horizontal;
            if(type == line_t::line_type::horizontal) type = line_t::line_type::vertical;
        }else{
            throw checker_exception(this->link->getName() + " has point not at corner.");
        }
    }
    if (type != lines.front().getType()) throw checker_exception(this->link->getName() + " has point not at corner.");

    //compute bounding
    double minx = 10e9, maxx = 0, miny = 10e9, maxy = 0;
    for(auto p : this->points){
        minx = std::min(minx, p.get_x());
        miny = std::min(miny, p.get_x());
        maxx = std::max(maxx, p.get_y());
        maxy = std::max(maxy, p.get_y());
    }
    this->bounding_rect = rect_t(vec2d_t(minx, miny), vec2d_t(maxx, maxy));

    //compute area
    for (const auto & line : lines) {
        this->area += line.getStart().get_x() * line.getAnEnd().get_y() - line.getStart().get_y() * line.getAnEnd().get_x();
    }
    this->area = abs(this->area) / 2;

    //area >= min_area
    if(this->area < this->link->get_area()) throw checker_exception(this->link->getName() + " area not enough.");

    //area >= 80% bounding rect
    if(this->area * 5 < this->bounding_rect.get_area() * 4) throw checker_exception(this->link->getName() + " area < 80% bounding rect");

    //bounding rect length 1 : 2
    double min = std::min(this->bounding_rect.get_size().get_x(), this->bounding_rect.get_size().get_y());
    double max = std::max(this->bounding_rect.get_size().get_x(), this->bounding_rect.get_size().get_y());
    if(max > min * 2) throw checker_exception(this->link->getName() + " length not 1 : 2");
}