//
// Created by lkj on 2023/4/7.
//

#include "visualizer_t.h"
#include "static_data/chip_t.h"
#include "static_data/module_t.h"
#include <thread>
#include <iostream>
#include <sstream>
#include <iomanip>

void my_out(const std::string& str){
#ifdef USING_VIS
    std::cout << str << std::endl;
#endif
}

void visualizer_t::gnup_th(const std::vector<std::pair<std::vector<vec2d_t>,std::string>> polys_pts){
    srand(time(NULL));
    int width = chip_t::get_width();
    int height = chip_t::get_height();
    FILE* pipe = popen(true ? "gnuplot -persist" : "gnuplot", "w");
//    fputs("set title \"A!!\"\n", pipe);
    fputs("set grid\n", pipe);
    fputs(("set xrange [0:" + std::to_string(width) + "]\n").c_str(), pipe);
    fputs(("set yrange [0:" + std::to_string(height) + "]\n").c_str(), pipe);

    std::string str = "";
    std::stringstream stream;
    for(int m = 0; m < polys_pts.size(); m++){
        str = "set object " + std::to_string(m + 1) + " polygon from ";
        std::vector<vec2d_t> x_y = {};
        for(int i = 0; i < polys_pts[m].first.size(); i++){
            str += std::to_string(polys_pts[m].first[i].get_x());
            str += ",";
            str += std::to_string(polys_pts[m].first[i].get_y());
            str += " to ";
        }
        str += std::to_string(polys_pts[m].first[0].get_x());
        str += ",";
        str += std::to_string(polys_pts[m].first[0].get_y());
        str += " \n";
        fputs(str.c_str(), pipe);

        const uint32_t r = (rand() % 0xFF);
        const uint32_t g = (rand() % 0xFF);
        const uint32_t b = (rand() % 0xFF);
        const uint32_t rgb = r << 16 | g << 8 | b;
        stream.str("");
        stream << "0x"
               << std::setfill ('0') << std::setw(6)
               << std::hex << rgb;

        str = "set object " + std::to_string(m + 1) + " fc rgb " + stream.str() + " "
              "fillstyle transparent solid 0.2 "
              "border lc rgb " + stream.str() + " lw 0.3" " \n";
        fputs(str.c_str(), pipe);
        stream.clear();
        fputs(("set label \"" + polys_pts[m].second + "\" at " +
                std::to_string(polys_pts[m].first[0].get_x()) + "," +
                std::to_string(polys_pts[m].first[0].get_y()) + "\n").c_str(), pipe);
    }

    fputs("plot 0\n", pipe);
    if(pipe != nullptr) { pclose(pipe); }
}

void visualizer_t::join(const std::vector<std::pair<std::vector<vec2d_t>,std::string>>& polys_pts){
    std::thread th(visualizer_t::gnup_th, polys_pts);
    th.detach();
    return;
}

void visualizer_t::show_fp(const std::vector<bounding_rectangle_t>& bd_rect) {
    std::vector<std::pair<std::vector<vec2d_t>,std::string>> bd_rect_pt;
    for(auto & i : bd_rect){
        const double rx = i.getRect().get_right_upper().get_x();
        const double lx = i.getRect().get_left_lower().get_x();
        const double uy = i.getRect().get_right_upper().get_y();
        const double ly = i.getRect().get_left_lower().get_y();
        bd_rect_pt.push_back({std::vector<vec2d_t>{{lx, ly}, {rx, ly}, {rx, uy}, {lx,uy}},
                              i.getLinkModule()->getName()});
    }
    join(bd_rect_pt);
    return ;
}
