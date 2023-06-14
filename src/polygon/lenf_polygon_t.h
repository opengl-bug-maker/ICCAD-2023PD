//
// Created by 林士傑 on 2023/6/9.
//

#ifndef ICCAD2023PD_LENF_POLYGON_T_H
#define ICCAD2023PD_LENF_POLYGON_T_H

#include <vector>
#include <map>
#include <set>
#include "polygon/polygon_module_t.h"
#include "utilities/bounding_rectangle_t.h"
#include "utilities/lenf_quadtree_t.h"
#include "utilities/box_t.h"
#include "utilities/rect_t.h"
#include "lenf_unit.h"

class lenf_polygon_t : public box_t{
    rect_t bounding_rect;
    lenf_quadtree_t<lenf_unit> origin_unit_tree;
    std::vector<std::shared_ptr<lenf_unit>> overlap_unit;

    std::map<std::set<std::shared_ptr<lenf_unit>>, std::shared_ptr<lenf_unit>> unit_lib;
//    std::vector<std::shared_ptr<lenf_unit>> units;

//    std::vector<polygon_overlap_area_t> overlap_areas;



    const static std::vector<std::vector<std::vector<std::vector<int>>>> combination_list;

    const std::shared_ptr<lenf_unit>& get_origin_unit();

public:
    explicit lenf_polygon_t(const bounding_rectangle_t& first_rect);

    const rect_t& get_bounding_rect() const override;

    const std::vector<std::shared_ptr<lenf_unit>>& get_rects() const;

    bool is_bounding_collision(const rect_t& rect) const;
    bool is_bounding_collision(const bounding_rectangle_t& rect) const;

    bool is_collision(const rect_t& rect) const;
    bool is_collision(const bounding_rectangle_t& rect) const;

    bool merge_polygon(lenf_polygon_t& polygon);

    void print();
};


#endif //ICCAD2023PD_LENF_POLYGON_T_H
