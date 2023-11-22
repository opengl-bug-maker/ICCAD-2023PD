#include "extended_module_t.h"

extended_module_t::extended_module_t(std::vector<double> lines) {
    this->lines = lines;
}

extended_module_t::extended_module_t(double *address) {
    this->lines.resize(16);
    for(int i = 0; i < 16; ++i){
        this->lines[i] = address[i];
    }
}
double extended_module_t::get_left_x_extend() {
    return this->lines[0];
}

double extended_module_t::get_right_x_extend() {
    return this->lines[3];
}

double extended_module_t::get_top_y_extend() {
    return this->lines[11];
}

double extended_module_t::get_bottom_y_extend() {
    return this->lines[8];
}

double extended_module_t::get_area() {
    return 
        (this->lines[1] - this->lines[0]) * (this->lines[13] - this->lines[12]) // left
        + (this->lines[5] - this->lines[4]) * (this->lines[11] - this->lines[10]) // top
        + this->get_inner_rect().get_area() // center
        + (this->lines[7] - this->lines[6]) * (this->lines[9] - this->lines[8]) // bottom
        + (this->lines[3] - this->lines[2]) * (this->lines[15] - this->lines[14]); // right
}

vec2d_t extended_module_t::get_center() {
    return this->get_outter_rect().get_center();
}

rect_t extended_module_t::get_inner_rect() {
    return rect_t(
        { this->lines[1], this->lines[9] },
        { this->lines[2] - this->lines[1], this->lines[10] - this->lines[9] }
    );
}

rect_t extended_module_t::get_outter_rect() {
    return rect_t(
        { this->lines[0], this->lines[8] },
        { this->lines[3] - this->lines[0], this->lines[11] - this->lines[8] }
    );
}

std::vector<vec2d_t> extended_module_t::to_vec2d() {
    return std::vector<vec2d_t>({
        {this->lines[1], this->lines[9]},
        {this->lines[6], this->lines[9]},
        {this->lines[6], this->lines[8]},
        {this->lines[7], this->lines[8]},
        {this->lines[7], this->lines[9]},
        {this->lines[2], this->lines[9]},
        {this->lines[2], this->lines[14]},
        {this->lines[3], this->lines[14]},
        {this->lines[3], this->lines[15]},
        {this->lines[2], this->lines[15]},
        {this->lines[2], this->lines[10]},
        {this->lines[7], this->lines[10]},
        {this->lines[7], this->lines[11]},
        {this->lines[6], this->lines[11]},
        {this->lines[6], this->lines[10]},
        {this->lines[1], this->lines[10]},
        {this->lines[1], this->lines[13]},
        {this->lines[0], this->lines[13]},
        {this->lines[0], this->lines[12]},
        {this->lines[1], this->lines[12]}
    });
}
