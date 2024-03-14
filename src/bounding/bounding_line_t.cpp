//
// Created by 林士傑 on 2023/6/19.
//

#include "bounding_line_t.h"

#pragma region bounding_line_element

bounding_line_element_t::bounding_line_element_t() : line_t() {

}

bounding_line_element_t::bounding_line_element_t(const line_t &line) : line_t(line) {

}

bounding_line_element_t::bounding_line_element_t(const vec2d_t &start, const vec2d_t &end) : line_t(start, end) {

}

bool bounding_line_element_t::operator==(const bounding_line_element_t &bounding_line_element) const {
    return line_t::operator==(bounding_line_element);
}

bool bounding_line_element_t::operator!=(const bounding_line_element_t &bounding_line_element) const {
    return line_t::operator!=(bounding_line_element);
}

#pragma endregion

#pragma region bounding_line

bounding_line_t::bounding_line_t() {
    this->lines = circular_T_list_t<bounding_line_element_t>();
}

bounding_line_t::bounding_line_t(circular_T_list_t<bounding_line_element_t> circular_list, bool clockwise) : clockwise(clockwise) {
    this->lines = circular_list;
}

bounding_line_t::bounding_line_t(const std::vector<bounding_line_element_t>& lines, bool clockwise) : clockwise(clockwise) {
    for(const auto& line: lines){
        this->lines.add_at_tail(line);
    }
}

bounding_line_t::bounding_line_t(const std::vector<vec2d_t>& points, bool clockwise) : clockwise(clockwise) {
    assert(points.size() > 1);
    if(clockwise){
        this->lines.add_at_tail(bounding_line_element_t(points.back(), points.front()));
        for(int i = 1; i < points.size(); ++i) {
            this->lines.add_at_tail(bounding_line_element_t(points[i - 1], points[i]));
        }
    }else{
        this->lines.add_at_head(bounding_line_element_t(points.front(), points.back()));
        for(int i = 1; i < points.size(); ++i) {
            this->lines.add_at_head(bounding_line_element_t(points[i], points[i - 1]));
        }
    }
}

bounding_line_t bounding_line_t::operator=(const bounding_line_t &bounding_line) {
    this->clockwise = bounding_line.clockwise;
    this->area = bounding_line.area;
    this->lines = bounding_line.lines;
    return *this;
}

// const line_t &bounding_line_t::get_left_most_line() {
//     return this->lines.get_root()->get_data();
// }

// const line_t &bounding_line_t::get_top_most_line() {
//     return this->lines.get_root()->get_data();
// }

// const line_t &bounding_line_t::get_right_most_line() {
//     return this->lines.get_root()->get_data();
// }

// const line_t &bounding_line_t::get_bottom_most_line() {
//     return this->lines.get_root()->get_data();
// }

bool bounding_line_t::touch(const bounding_line_t &bounding_line) const {
    return false;
}

std::vector<bounding_line_t> merge(bounding_line_t bounding_line0, bounding_line_t bounding_line1) {
    // std::cout << "before merge\n";
    // bounding_line0.print();
    // bounding_line1.print();
    auto it0 = bounding_line0.lines.begin();
    while(it0 != bounding_line0.lines.end()) {
        auto it1 = bounding_line1.lines.begin();
        while(it1 != bounding_line1.lines.end()) {
            bounding_line_element_t l0 = it0->get_data();
            bounding_line_element_t l1 = it1->get_data();
            auto collt = l0.is_intersect(l1);
            // std::cout << collt << " ";
            // std::cout << l0 << " " << l1 << std::endl;
            if(collt == line_t::collision_vertical) {
                vec2d_t new_point = l0.intersect_point(l1);
                int time = 0;
                it1 = bounding_line1.lines.delete_node_get_prev(it1);
                if(new_point != l1.get_end()) {
                    bounding_line1.lines.concat_next(it1, bounding_line_element_t(new_point, l1.get_end()));
                    time++;
                }
                if(new_point != l1.get_start()) {
                    bounding_line1.lines.concat_next(it1, bounding_line_element_t(l1.get_start(), new_point));
                    time++;
                }
                it0 = bounding_line0.lines.delete_node_get_prev(it0);
                if(new_point != l0.get_end()) {
                    // std::cout << "concat " << bounding_line_element_t(new_point, l0.get_end()) << "\n";
                    bounding_line0.lines.concat_next(it0, bounding_line_element_t(new_point, l0.get_end()));
                    time++;
                }
                if(new_point != l0.get_start()) {
                    // std::cout << "concat " << bounding_line_element_t(l0.get_start(), new_point) << "\n";
                    bounding_line0.lines.concat_next(it0, bounding_line_element_t(l0.get_start(), new_point));
                    time++;
                }
            } else if (collt == line_t::collision_horizontal) {
                // std::cout << "hor coll\n";

                if(l0.get_line_direction_type() != l1.get_line_direction_type()) {
                    std::optional<std::vector<line_t>> lines0 = l0.parallel_cut_reverse(l1);
                    std::optional<std::vector<line_t>> lines1 = l1.parallel_cut_reverse(l0);
                    // if(lines0) for(auto l : lines0.value()) std::cout << l << " ";
                    // std::cout << "\n";
                    // if(lines1) for(auto l : lines1.value()) std::cout << l << " ";
                    // std::cout << "\n";
                    if(lines0 && lines1) {
                        it0 = bounding_line0.lines.delete_node_get_prev(it0);
                        it1 = bounding_line1.lines.delete_node_get_prev(it1);
                        if(lines0.value().size() != 1 || lines1.value().size() != 1) {
                            for(auto l : lines0.value()) bounding_line0.lines.concat_next(it0, l);
                            for(auto l : lines1.value()) bounding_line1.lines.concat_next(it1, l);
                        }
                    }
                }
            }
            it1 = it1->get_next();
        }
        it0 = it0->get_next();
    }
    // std::cout << "end slicing\n";
    // bounding_line0.print();
    // bounding_line1.print();


    std::vector<std::pair<circular_T_node_t<bounding_line_element_t>*, circular_T_node_t<bounding_line_element_t>*>> concat_pair;
    it0 = bounding_line0.lines.get_head();
    while(it0 = it0->get_next(), it0 != bounding_line0.lines.end()) {
        auto it1 = bounding_line1.lines.get_head();
        int times = 0;
        while(it1 = it1->get_next(), it1 != bounding_line1.lines.end()) {
            if(it0->get_data().get_end() == it1->get_data().get_start() || it0->get_data().get_start() == it1->get_data().get_end()) {
                concat_pair.push_back({it0, it1});
                times++;
            }
            if(times == 2) {
                if(concat_pair.back().first->get_data().get_end() != concat_pair.back().second->get_data().get_start()) {
                    auto temp0 = concat_pair.back();
                    concat_pair.pop_back();
                    auto temp1 = concat_pair.back();
                    concat_pair.pop_back();
                    concat_pair.push_back(temp0);
                    concat_pair.push_back(temp1);
                }
            }
        }
    }
    if(concat_pair.empty()) {
        bounding_line0.update();
        bounding_line1.update();
        return {bounding_line0, bounding_line1};
    }

    if(concat_pair.front().first->get_data().get_end() == concat_pair.front().second->get_data().get_start()) {
        concat_pair.push_back(concat_pair.front());
        concat_pair.erase(concat_pair.begin());
    }

    // std::cout << "pair \n";
    // for(auto p : concat_pair) {
    //     std::cout << p.first->get_data() << " " << p.second->get_data() << "\n";
    // }

    int size = concat_pair.size();
    // std::cout << "concat_size " << size << std::endl;
    if(size % 2 != 0) {
        //something wrong
        std::cout << "why QQ \n";
    }

    // std::vector<circular_T_node_t<bounding_line_element_t>*> ggbs;

    size >>= 1;
    // std::cout << "new list\n";
    std::vector<bounding_line_t> bounding_lines;
    bounding_lines.reserve(size);
    for(int i = 0; i < size; ++i) {
        bounding_lines.emplace_back(bounding_line_t());
    }

    bounding_line0.lines.drop();
    bounding_line1.lines.drop();
    for(int i = 0; i < size; ++i) {
        std::pair<circular_T_node_t<bounding_line_element_t>*, circular_T_node_t<bounding_line_element_t>*> fir_pair;
        std::pair<circular_T_node_t<bounding_line_element_t>*, circular_T_node_t<bounding_line_element_t>*> sec_pair;
        // std::cout << "ori first pair " << concat_pair[i * 2].first << " " << concat_pair[i * 2].first->get_data() << " " << concat_pair[i * 2].second << " " << concat_pair[i * 2].second->get_data() << std::endl;
        // std::cout << "ori second pair " << concat_pair[i * 2 + 1].first << " " << concat_pair[i * 2 + 1].first->get_data() << " " << concat_pair[i * 2 + 1].second << " " << concat_pair[i * 2 + 1].second->get_data() << std::endl;
        // std::cout << concat_pair[i * 2].second->get_data().get_start() << "\n";
        // std::cout << concat_pair[i * 2].first->get_data().get_end() << "\n";
        if(concat_pair[i * 2].second->get_data().get_start() == concat_pair[i * 2].first->get_data().get_end()) {
            // std::cout << "switch pair\n";
            fir_pair = concat_pair[i * 2 + 1];
            sec_pair = concat_pair[i * 2];
        } else {
            // std::cout << "none switch pair\n";
            fir_pair = concat_pair[i * 2];
            sec_pair = concat_pair[i * 2 + 1];
        }
        // std::cout << "\ncheck pair1\n";
        // for(auto p : concat_pair) {
        //     std::cout << p.first->get_data() << " " << p.second->get_data() << "\n";
        // }
        // std::cout << "check pair1\n\n";

        // std::cout << "first pair " << fir_pair.first << " " << fir_pair.first->get_data() << " " << fir_pair.second << " " << fir_pair.second->get_data() << std::endl;
        // std::cout << "second pair " << sec_pair.first << " " << sec_pair.first->get_data() << " " << sec_pair.second << " " << sec_pair.second->get_data() << std::endl;

        // concat_pair[i * 2 + 1].second->get_prev()->set_next(nullptr);
        sec_pair.first->set_next(sec_pair.second);
        sec_pair.second->set_prev(sec_pair.first);
        // std::cout << sec_pair.first->get_data() << " next -> " << sec_pair.second->get_data() << std::endl;
        // std::cout << sec_pair.second->get_data() << " prev -> " << sec_pair.first->get_data() << std::endl;

        // std::cout << "\ncheck pair2\n";
        // for(auto p : concat_pair) {
        //     std::cout << p.first->get_data() << " " << p.second->get_data() << "\n";
        // }
        // std::cout << "check pair2\n\n";

        bounding_lines[i].lines.get_head()->set_next(fir_pair.first);
        // std::cout << fir_pair.first->get_data() << " is " << bounding_lines[i].lines.get_head()->get_next()->get_data() << std::endl;
        // std::cout << fir_pair.second->get_data() << " is " << bounding_lines[i].lines.get_tail()->get_prev()->get_data() << std::endl;
        bounding_lines[i].lines.get_tail()->set_prev(fir_pair.second);

        // std::cout << "\ncheck pair4\n";
        // for(auto p : concat_pair) {
        //     std::cout << p.first->get_data() << " " << p.second->get_data() << "\n";
        // }
        // std::cout << "check pair4\n\n";
        
        // std::cout << fir_pair.second->get_data() << " is " << bounding_lines[i].lines.get_tail()->get_prev()->get_data() << std::endl;
        fir_pair.first->set_prev(bounding_lines[i].lines.get_head());
        // std::cout << fir_pair.first->get_data() << " head " << bounding_lines[i].lines.get_head() << std::endl;
        fir_pair.second->set_next(bounding_lines[i].lines.get_tail());
        // std::cout << fir_pair.second->get_data() << " tail " << bounding_lines[i].lines.get_tail() << std::endl;

        // bounding_lines.push_back(bounding_lines[i]);


        // std::cout <<"head : " << bounding_lines[i].lines.get_head() << std::endl;
        // std::cout <<"tail : " << bounding_lines[i].lines.get_tail() << std::endl;
        // std::cout << "\ncheck line\n";
        // circular_T_node_t<bounding_line_element_t>* he = bounding_lines[i].lines.get_head();
        // for(int i = 0; i < 30; ++i) {
        //     std::cout << he << " " << he->get_data() << "\n";
        //     he = he->get_next();
        // }
        // std::cout << "check line\n\n";

        // std::cout << " update " << i << std::endl;
        // bounding_lines[i].print();
        bounding_lines[i].update();
        // bounding_lines.back().print();
        // bounding_lines.back().update();
        // std::cout << " update " << i << " done " << std::endl;
    }
    // std::cout << "clear\n";
    return bounding_lines;
}


void bounding_line_t::print_reverse() const {
    this->lines.print_reverse();
}

void bounding_line_t::print() const {
    this->lines.print();
}

std::vector<vec2d_t> bounding_line_t::get_nodes() const {
    std::vector<vec2d_t> vec;
    for(const auto& l : this->lines.to_vector_reverse()){
        vec.push_back(l.get_start());
    }
    return vec;
}

void bounding_line_t::update_area() {
    double area = 0;
    circular_T_node_t<bounding_line_element_t>* cur = this->lines.begin();
    while(cur->get_next() != this->lines.get_tail()) {
        area += cur->get_data().dot(cur->get_next()->get_data());
        cur = cur->get_next();
    }
    area += this->lines.get_tail()->get_prev()->get_data().dot(this->lines.get_head()->get_next()->get_data());
    this->area = area;
}

void bounding_line_t::update_clockwise() {
    if(area < 0) {
        this->clockwise = true;
    } else {
        this->clockwise = false;
    }
}

void bounding_line_t::update() {
    update_area();
    update_clockwise();
}

double bounding_line_t::get_area() const {
    return std::abs(this->area);
}

bool bounding_line_t::get_clockwise() const {
    return this->clockwise;
}

std::ostream &operator<<(std::ostream &os, const bounding_line_t &bounding_line) {
    for(auto vec2d : bounding_line.get_nodes()){
        os << vec2d << ", ";
    }
    return os;
}

#pragma endregion