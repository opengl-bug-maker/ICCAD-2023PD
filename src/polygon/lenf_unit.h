//
// Created by 林士傑 on 2023/6/9.
//

#ifndef ICCAD2023PD_LENF_UNIT_H
#define ICCAD2023PD_LENF_UNIT_H

#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <set>
#include "polygon/polygon_overlap_area_t.h"
#include "utilities/bounding_rectangle_t.h"

class lenf_unit : public box_t{
    bounding_rectangle_t module_rect;

    uint32_t max_area;
    std::map<std::shared_ptr<lenf_unit>, uint32_t> area_from_where;
    std::vector<std::shared_ptr<lenf_unit>> connections;

    bool if_found = false;

//    const uint32_t demand_area;
//    uint32_t free_to_share_area, shared_area;

//    std::vector<std::reference_wrapper<polygon_module_t>> get_neighbor(const polygon_overlap_area_t&) const;
//    std::pair<bool, uint32_t> request_area(const uint32_t& required_area);

public:

    explicit lenf_unit(const bounding_rectangle_t& bounding_rect);

    lenf_unit(const lenf_unit& polygon_module);
    lenf_unit operator=(const lenf_unit& r);

    const rect_t &get_bounding_rect() const override;

    const bounding_rectangle_t& get_module_bounding_rectangle() const;

    void add_connection(const std::shared_ptr<lenf_unit>& connection);

    void set_area(std::set<std::shared_ptr<lenf_unit>> area_keys);

    void fix_max_area(int area);

    std::map<std::shared_ptr<lenf_unit>, uint32_t> set_overlap_take(const std::shared_ptr<lenf_unit>& requester, uint32_t area);

    void set_area_from_where(const std::map<std::shared_ptr<lenf_unit>, uint32_t> &areaFromWhere);

    int fix_area(std::shared_ptr<lenf_unit> robber, int value);




//    bool connect(lenf_unit& module);
};


#endif //ICCAD2023PD_LENF_UNIT_H
