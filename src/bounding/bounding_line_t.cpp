//
// Created by 林士傑 on 2023/6/19.
//

#include "bounding_line_t.h"
#include <cassert>

#pragma region bounding_line_interect_result

bool bounding_line_interect_result_t::operator==(const bounding_line_interect_result_t &bounding_line_interect_result) {
    if(this->difference_pos_line.size() != bounding_line_interect_result.difference_pos_line.size()) return false;
    if(this->difference_neg_line.size() != bounding_line_interect_result.difference_neg_line.size()) return false;
    if(this->intersect_line.size() != bounding_line_interect_result.intersect_line.size()) return false;
    if(this->union_line.size() != bounding_line_interect_result.union_line.size()) return false;
    for(int i = 0; i < this->difference_pos_line.size(); ++i) if(this->difference_pos_line[i] != bounding_line_interect_result.difference_pos_line[i]) return false;
    for(int i = 0; i < this->difference_neg_line.size(); ++i) if(this->difference_neg_line[i] != bounding_line_interect_result.difference_neg_line[i]) return false;
    for(int i = 0; i < this->intersect_line.size(); ++i) if(this->intersect_line[i] != bounding_line_interect_result.intersect_line[i]) return false;
    for(int i = 0; i < this->union_line.size(); ++i) if(this->union_line[i] != bounding_line_interect_result.union_line[i]) return false;
    return true;
}

bool bounding_line_interect_result_t::operator!=(const bounding_line_interect_result_t &bounding_line_interect_result) {
    return !this->operator==(bounding_line_interect_result);
}

#pragma endregion

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

#pragma region bounding_line_anchor_points

void bounding_line_anchor_points::update_anchor(bounding_node_t* &bounding_node) {
    const vec2d_t& p = bounding_node->get_data().get_start();
    if(p.get_x() < this->anchors[anchor_type::left_lower].get_x() || (p.get_x() == this->anchors[anchor_type::left_lower].get_x() && p.get_y() < this->anchors[anchor_type::left_lower].get_y())) {
        this->anchors[anchor_type::left_lower] = p;
        this->anchor_lines[anchor_type::left_lower] = bounding_node;
    }
    if(p.get_x() < this->anchors[anchor_type::left_upper].get_x() || (p.get_x() == this->anchors[anchor_type::left_upper].get_x() && p.get_y() > this->anchors[anchor_type::left_upper].get_y())) {
        this->anchors[anchor_type::left_upper] = p;
        this->anchor_lines[anchor_type::left_upper] = bounding_node;
    }
    if(p.get_y() > this->anchors[anchor_type::upper_left].get_y() || (p.get_y() == this->anchors[anchor_type::upper_left].get_y() && p.get_x() < this->anchors[anchor_type::upper_left].get_x())) {
        this->anchors[anchor_type::upper_left] = p;
        this->anchor_lines[anchor_type::upper_left] = bounding_node;
    }
    if(p.get_y() > this->anchors[anchor_type::upper_right].get_y() || (p.get_y() == this->anchors[anchor_type::upper_right].get_y() && p.get_x() > this->anchors[anchor_type::upper_right].get_x())) {
        this->anchors[anchor_type::upper_right] = p;
        this->anchor_lines[anchor_type::upper_right] = bounding_node;
    }
    if(p.get_x() > this->anchors[anchor_type::right_upper].get_x() || (p.get_x() == this->anchors[anchor_type::right_upper].get_x() && p.get_y() > this->anchors[anchor_type::right_upper].get_y())) {
        this->anchors[anchor_type::right_upper] = p;
        this->anchor_lines[anchor_type::right_upper] = bounding_node;
    }
    if(p.get_x() > this->anchors[anchor_type::right_lower].get_x() || (p.get_x() == this->anchors[anchor_type::right_lower].get_x() && p.get_y() < this->anchors[anchor_type::right_lower].get_y())) {
        this->anchors[anchor_type::right_lower] = p;
        this->anchor_lines[anchor_type::right_lower] = bounding_node;
    }
    if(p.get_y() < this->anchors[anchor_type::lower_right].get_y() || (p.get_y() == this->anchors[anchor_type::lower_right].get_y() && p.get_x() > this->anchors[anchor_type::lower_right].get_x())) {
        this->anchors[anchor_type::lower_right] = p;
        this->anchor_lines[anchor_type::lower_right] = bounding_node;
    }
    if(p.get_y() < this->anchors[anchor_type::lower_left].get_y() || (p.get_y() == this->anchors[anchor_type::lower_left].get_y() && p.get_x() < this->anchors[anchor_type::lower_left].get_x())) {
        this->anchors[anchor_type::lower_left] = p;
        this->anchor_lines[anchor_type::lower_left] = bounding_node;
    }
}

void bounding_line_anchor_points::set_anchor_line(bounding_line_anchor_points::anchor_type anchor, bounding_node_t* &bounding_node) {
    this->anchor_lines[anchor] = bounding_node;
    this->anchors[anchor] = bounding_node->get_data().get_start();
}

const bounding_node_t* bounding_line_anchor_points::get_anchor_line(bounding_line_anchor_points::anchor_type anchor) const {
    return this->anchor_lines[anchor];
}

const std::vector<bounding_node_t*> bounding_line_anchor_points::get_anchor_lines() const {
    return this->anchor_lines;
}

const vec2d_t &bounding_line_anchor_points::get_anchor_point(bounding_line_anchor_points::anchor_type anchor) const {
    return this->anchors[anchor];
}

#pragma endregion

#pragma region bounding_line

#pragma region basic

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
    this->update();
}

bounding_line_t::bounding_line_t(const rect_t &rect, bool clockwise) : bounding_line_t(rect.to_bounding_point(), clockwise) {
    
}

bounding_line_t::bounding_line_t(const std::vector<vec2d_t>& points, bool clockwise) : clockwise(clockwise) {
    assert(points.size() > 1);
    if(clockwise){
        if(points.front() != points.back()) this->lines.add_at_tail(bounding_line_element_t(points.back(), points.front()));
        for(int i = 1; i < points.size(); ++i) {
            if(points[i - 1] == points[i]) continue;
            this->lines.add_at_tail(bounding_line_element_t(points[i - 1], points[i]));
        }
    }else{
        if(points.front() != points.back()) this->lines.add_at_head(bounding_line_element_t(points.front(), points.back()));
        for(int i = 1; i < points.size(); ++i) {
            if(points[i - 1] == points[i]) continue;
            this->lines.add_at_head(bounding_line_element_t(points[i], points[i - 1]));
        }
    }
    this->update();
}

bounding_line_t bounding_line_t::operator=(const bounding_line_t &bounding_line) {
    this->clockwise = bounding_line.clockwise;
    this->area = bounding_line.area;
    this->lines = bounding_line.lines;
    return *this;
}

bool bounding_line_t::operator==(const bounding_line_t &bounding_line) {
    if(this->get_clockwise() != bounding_line.get_clockwise()) return false;
    if(this->get_area() != bounding_line.get_area()) return false;
    if(this->get_bounding_rect() != bounding_line.get_bounding_rect()) return false;
    if(this->lines != bounding_line.lines) return false;
    return true;
}

bool bounding_line_t::operator!=(const bounding_line_t &bounding_line) {
    return !this->operator==(bounding_line);
}

#pragma endregion

#pragma region basic_func

void bounding_line_t::print() const {
    this->lines.print();
}

void bounding_line_t::print_reverse() const {
    this->lines.print_reverse();
}

void bounding_line_t::plot(std::string plot_name) const {
    visualizer_t::polygon_detail(this->get_nodes(), plot_name);
}

std::ostream &operator<<(std::ostream &os, const bounding_line_t &bounding_line) {
    for(auto vec2d : bounding_line.get_nodes()){
        os << vec2d << ", ";
    }
    return os;
}

#pragma endregion

#pragma region updating

void bounding_line_t::reduce_line() {
    circular_T_node_t<bounding_line_element_t>* cur = this->lines.begin();
    std::optional<line_t> line = std::nullopt;
    while(cur->get_next() != this->lines.get_tail()) {
        if((line = cur->get_data().merge(cur->get_next()->get_data()))) {
            this->lines.delete_node(cur->get_next());
            cur = this->lines.delete_node_get_prev(cur);
            this->lines.concat_next(cur, bounding_line_element_t(line.value()));
            line = std::nullopt;
        }
        cur = cur->get_next();
    }
    if((line = this->lines.get_tail()->get_prev()->get_data().merge(this->lines.begin()->get_data()))) {
        this->lines.delete_node(this->lines.get_tail()->get_prev());
        this->lines.delete_node(this->lines.begin());
        this->lines.add_at_head(line.value());
    }
}

void bounding_line_t::update_area() {
    double area = 0;
    circular_T_node_t<bounding_line_element_t>* cur = this->lines.get_head();
    while(cur = cur->get_next(), cur != this->lines.get_tail()) {
        area += cur->get_data().ori_dot_area();
    }
    this->area = area / 2.0;
}

void bounding_line_t::update_clockwise() {
    if(area < 0) {
        this->clockwise = true;
    } else {
        this->clockwise = false;
    }
}

void bounding_line_t::update_bounding() {
    circular_T_node_t<bounding_line_element_t>* cur = this->lines.begin();
    vec2d_t left_lower(1e9);
    vec2d_t right_upper(-1e9);
    while(cur != this->lines.get_tail()) {
        left_lower.set_x(std::min(left_lower.get_x(), cur->get_data().get_little_x()));
        left_lower.set_y(std::min(left_lower.get_y(), cur->get_data().get_little_y()));
        right_upper.set_x(std::max(right_upper.get_x(), cur->get_data().get_large_x()));
        right_upper.set_y(std::max(right_upper.get_y(), cur->get_data().get_large_y()));
        cur = cur->get_next();
    }
    this->bounding_rect = rect_t(left_lower, right_upper - left_lower);
}

void bounding_line_t::update_root() {
    circular_T_node_t<bounding_line_element_t>* lower_left = this->lines.begin();
    circular_T_node_t<bounding_line_element_t>* cur = this->lines.get_head();
    while(cur = cur->get_next(), cur != this->lines.get_tail()) {
        if(cur->get_data().get_start().get_y() < lower_left->get_data().get_start().get_y()) {
            lower_left = cur;
        } else if (cur->get_data().get_start().get_y() == lower_left->get_data().get_start().get_y()) {
            if(cur->get_data().get_start().get_x() < lower_left->get_data().get_start().get_x()) {
                lower_left = cur;
            }
        }
    }
    this->lines.drop();
    this->lines.get_tail()->set_prev(lower_left->get_prev());
    lower_left->get_prev()->set_next(this->lines.get_tail());
    lower_left->set_prev(this->lines.get_head());
    this->lines.get_head()->set_next(lower_left);
}

void bounding_line_t::update() {
    this->reduce_line();
    this->update_area();
    this->update_clockwise();
    this->update_bounding();
    this->update_root();
}

#pragma endregion

#pragma region primary_func

bool bounding_line_t::rough_collision(const bounding_line_t &bounding_line) const {
    return this->get_bounding_rect().is_touch(bounding_line.get_bounding_rect());
}

bool bounding_line_t::collision(const bounding_line_t &bounding_line) const {
    if(!rough_collision(bounding_line)) return false;

    auto it0 = this->lines.get_head();
    while(it0 = it0->get_next(), it0 != this->lines.end()) {
        auto it1 = bounding_line.lines.get_head();
        while(it1 = it1->get_next(), it1 != bounding_line.lines.end()) {
            bounding_line_element_t l0 = it0->get_data();
            bounding_line_element_t l1 = it1->get_data();
            auto collt = l0.is_intersect(l1);
            if(collt == line_t::collision_vertical) {
                return true;
            } else if (collt == line_t::collision_horizontal) {
                if(l0.get_line_direction_type() != l1.get_line_direction_type()) {
                    return true;
                }
            }
        }
    }
    return false;
}

bounding_line_interect_result_t bounding_line_t::merge(bounding_line_t bounding_line0, bounding_line_t bounding_line1) {
    if(!bounding_line0.collision(bounding_line1)) {
        bounding_line0.update();
        bounding_line1.update();
        return {{}, {}, {bounding_line0}, {bounding_line1}};
    }
    auto it0 = bounding_line0.lines.begin();
    while(it0 != bounding_line0.lines.end()) {
        auto it1 = bounding_line1.lines.begin();
        while(it1 != bounding_line1.lines.end()) {
            bounding_line_element_t l0 = it0->get_data();
            bounding_line_element_t l1 = it1->get_data();
            auto collt = l0.is_intersect(l1);
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
                    bounding_line0.lines.concat_next(it0, bounding_line_element_t(new_point, l0.get_end()));
                    time++;
                }
                if(new_point != l0.get_start()) {
                    bounding_line0.lines.concat_next(it0, bounding_line_element_t(l0.get_start(), new_point));
                    time++;
                }
            } else if (collt == line_t::collision_horizontal) {
                if(l0.get_line_direction_type() != l1.get_line_direction_type()) {
                    std::optional<std::vector<line_t>> lines0 = l0.parallel_cut_reverse(l1);
                    std::optional<std::vector<line_t>> lines1 = l1.parallel_cut_reverse(l0);
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
        return {{}, {}, {bounding_line0}, {bounding_line1}};
    }

    if(concat_pair.front().first->get_data().get_end() == concat_pair.front().second->get_data().get_start()) {
        concat_pair.push_back(concat_pair.front());
        concat_pair.erase(concat_pair.begin());
    }

    int size = concat_pair.size();
    if(size % 2 != 0) {
        //something wrong
        std::cout << "why QQ \n";
    }

    size >>= 1;
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

        if(concat_pair[i * 2].second->get_data().get_start() == concat_pair[i * 2].first->get_data().get_end()) {
            fir_pair = concat_pair[i * 2 + 1];
            sec_pair = concat_pair[i * 2];
        } else {
            fir_pair = concat_pair[i * 2];
            sec_pair = concat_pair[i * 2 + 1];
        }

        sec_pair.first->set_next(sec_pair.second);
        sec_pair.second->set_prev(sec_pair.first);

        bounding_lines[i].lines.get_head()->set_next(fir_pair.first);
        bounding_lines[i].lines.get_tail()->set_prev(fir_pair.second);

        fir_pair.first->set_prev(bounding_lines[i].lines.get_head());
        fir_pair.second->set_next(bounding_lines[i].lines.get_tail());

        bounding_lines[i].update();
    }
    bounding_line_interect_result_t bdire;
    for(auto bd : bounding_lines) {
        if(bd.get_clockwise()) {
            bdire.difference_pos_line.push_back(bd);
        } else {
            bdire.difference_neg_line.push_back(bd);
        }
    }
    return bdire;
}

#pragma endregion

#pragma region secondary_func

bool bounding_line_t::vaild_for_80percent() const {
    return this->get_area() * 5 >= this->get_bounding_rect().get_area();
}

int bounding_line_t::get_edge_count() const {
    int edge_count = 0;
    circular_T_node_t<bounding_line_element_t>* it = this->lines.get_head();
    while(it = it->get_next(), it != this->lines.get_tail()) {
        edge_count++;
    }
    return edge_count;
}

int bounding_line_t::get_vertex_count() const {
    int vertex_count = 0;
    circular_T_node_t<bounding_line_element_t>* it = this->lines.get_head();
    while(it = it->get_next(), it != this->lines.get_tail()) {
        vertex_count++;
    }
    return vertex_count;
}

int bounding_line_t::get_vertex270_count() const {
    int vertex270_count = 0;
    circular_T_node_t<bounding_line_element_t>* it = this->lines.get_head();
    while(it = it->get_next(), it->get_next() != this->lines.get_tail()) {
        if(it->get_data().get_line_turn_direction_type(it->get_next()->get_data()) == line_t::line_turn_direction_type::turn_direction_type_left) {
            vertex270_count++;
        }
    }
    if(this->lines.get_tail()->get_prev()->get_data().get_line_turn_direction_type(this->lines.begin()->get_data()) == line_t::line_turn_direction_type::turn_direction_type_left) {
        vertex270_count++;
    }
    return vertex270_count;
}

#pragma endregion

#pragma region getter

std::vector<vec2d_t> bounding_line_t::get_nodes() const {
    std::vector<vec2d_t> vec;
    for(const auto& l : this->lines.to_vector()){
        vec.push_back(l.get_start());
    }
    return vec;
}

double bounding_line_t::get_area() const {
    return std::abs(this->area);
}

bool bounding_line_t::get_clockwise() const {
    return this->clockwise;
}

const rect_t bounding_line_t::get_bounding_rect() const {
    return this->bounding_rect;
}

#pragma endregion

#pragma endregion