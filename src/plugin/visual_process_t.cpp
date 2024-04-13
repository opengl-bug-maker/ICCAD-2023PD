#include "visual_process_t.h"
#include "static_data/chip_t.h"
#include <thread>

visual_unit_polygon_t::visual_unit_polygon_t(const std::vector<vec2d_t> &nodes) : visual_unit_polygon_t(nodes, "", false) {
}

visual_unit_polygon_t::visual_unit_polygon_t(const std::vector<vec2d_t>& nodes, bool vertex_label) : visual_unit_polygon_t(nodes, "", vertex_label) {
}

visual_unit_polygon_t::visual_unit_polygon_t(const std::vector<vec2d_t> &nodes, std::string name) : visual_unit_polygon_t(nodes, name, false) {
}

visual_unit_polygon_t::visual_unit_polygon_t(const std::vector<vec2d_t> &nodes, std::string name, bool vertex_label)  : nodes(nodes), name(name), vertex_label(vertex_label) {
}

std::string visual_unit_polygon_t::to_string() const {
    std::string str = "set object " + std::to_string(visual_unit_t::index) + " polygon from ";
    for(const auto& vec : this->nodes) {
        str += std::to_string(vec.get_x()) + "," + std::to_string(vec.get_y()) + " to ";
    }
    str += std::to_string(this->nodes.front().get_x()) + "," + std::to_string(this->nodes.front().get_y()) + " \n";
    return str;
}

std::string visual_unit_polygon_t::to_setting() const {
    std::stringstream stream;
    const uint32_t r = (rand() % 0xBF);
    const uint32_t g = (rand() % 0xBF);
    const uint32_t b = (rand() % 0xBF);
    const uint32_t rgb = r << 16 | g << 8 | b;
    stream.str("");
    stream << "0x"
            << std::setfill ('0') << std::setw(6)
            << std::hex << rgb;

    std::string str = "set object " + std::to_string(visual_unit_t::index) + " fc rgb " + stream.str() + " "
            "fillstyle transparent solid 0.2 "
            "border lc rgb " + stream.str() + " lw 0.3" " \n";
    return str;
}

std::string visual_unit_polygon_t::to_label() const {
    std::string str = "set label \"" + this->name + "\" at " +
            std::to_string(this->nodes.front().get_x()) + "," +
            std::to_string(this->nodes.front().get_y()) + "\n";
    if(this->vertex_label) {
        int index = 0;
        for(const auto& vertex : this->nodes) {
            str += "set label \"" + std::to_string(index++) + "\" at " +
                std::to_string(vertex.get_x()) + "," +
                std::to_string(vertex.get_y()) + "\n";
        }
    }
    return str;
}

visual_unit_line_t::visual_unit_line_t(const line_t &line) : line(line) {
}

std::string visual_unit_line_t::to_string() const {
    std::string str = "set arrow from "
        + std::to_string(this->line.get_start().get_x()) + "," + std::to_string(this->line.get_start().get_y())
        + " to "
        + std::to_string(this->line.get_end().get_x()) + "," + std::to_string(this->line.get_end().get_y()) + " nohead\n";
    return str;
}

std::string visual_unit_line_t::to_setting() const {
    return std::string();
}

std::string visual_unit_line_t::to_label() const {
    std::string str = "set label \"" + std::to_string(visual_unit_t::index) + "\" at " +
            std::to_string(((this->line.get_start() + this->line.get_end()) / 2).get_x()) + "," +
            std::to_string(((this->line.get_start() + this->line.get_end()) / 2).get_y()) + "\n";
    return str;
}

visual_process_t::~visual_process_t() {
    for(auto& unit : this->units) {
        if(unit != nullptr) {
            delete unit;
            unit = nullptr;
        }
    }
}

void visual_process_t::push_unit(visual_unit_t *unit) {
    this->units.push_back(unit);
    unit->index = this->units.size();
}

void visual_process_t::thread_plot() {
    auto temp = this;
    int width = chip_t::get_width();
    int height = chip_t::get_height();
    double canvas_size = 600.0f * double(width) / double(height);
    FILE* pipe = popen(true ? "gnuplot -persist" : "gnuplot", "w");
    fputs(("set term qt size " + std::to_string(canvas_size) + ",600\n").c_str(), pipe);
    fputs(("set term qt title \"" + this->title + "\"\n").c_str(), pipe);
    fputs("set grid\n", pipe);
    fputs("set mxtics 5\n", pipe);
    fputs("set mytics 5\n", pipe);
    fputs(("set xrange [0:" + std::to_string(width) + "]\n").c_str(), pipe);
    fputs(("set yrange [0:" + std::to_string(height) + "]\n").c_str(), pipe);

    for(const auto& unit : this->units) {
        fputs(unit->to_string().c_str(), pipe);
    }

    for(const auto& unit : this->units) {
        fputs(unit->to_setting().c_str(), pipe);
    }

    for(const auto& unit : this->units) {
        fputs(unit->to_label().c_str(), pipe);
    }

    fputs("plot 0\n", pipe);
    if(pipe != nullptr) { pclose(pipe); }
}


void visual_process_t::plot() {
    std::thread th(&visual_process_t::thread_plot, this);
    th.join();
    return;
}
