//
// Created by lkj on 2023/4/7.
//

#include "visualizer_t.h"
#include "static_data/chip_t.h"
#include "static_data/module_t.h"
#include <matplot/matplot.h>

void visualizer_t::show_fp(std::vector<bounding_rectangle_t> & bd_rect) {
    srand(time(NULL));
    using namespace matplot;
    int W = chip_t::get_width(), H = chip_t::get_height();
    int digit = std::pow(10, ((std::floor(std::log10(std::max(W,H))))-1));
    gcf()->size(800, int(800.0 * H / W));
    xlim({0,static_cast<double>(W)});
    ylim({0,static_cast<double>(H)});
    yticks(iota(0, digit, H));
    xticks(iota(0, digit, W));
    grid(on);

    std::vector<std::vector<double>> color_map = palette::plasma(); // 256

    for(const auto& i : bd_rect) {
        auto r2 = rectangle(i.getRect().get_left_lower().get_x(), i.getRect().get_left_lower().get_y(),
                                        i.getRect().get_size().get_x(), i.getRect().get_size().get_y());
        uint_fast32_t ind = rand() % 256;
        r2->color({0.8f, float(color_map[ind][0]), float(color_map[ind][1]), float(color_map[ind][2])});
        r2->line_width(0.0);
        r2->fill(true);
        text(i.getRect().get_left_lower().get_x(), i.getRect().get_left_lower().get_y(), i.getLinkModule()->getName());
    }
    show();
    return ;
}
