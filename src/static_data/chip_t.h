//
// Created by 林士傑 on 2023/3/8.
//

#ifndef INC_2023PD_CHIP_H
#define INC_2023PD_CHIP_H

#include <string>
#include <vector>
#include <fstream>
#include <exception>
#include <unordered_map>
#include "../utilities/vec2d_t.h"
#include "static_data/mcnc/multi_net_t.h"
//#include "static_data/module_t.h"
//#include "static_data/soft_module_t.h"
//#include "static_data/fixed_module_t.h"

class module_t;
class soft_module_t;
class fixed_module_t;

class similar_chip_t {
public:
    static bool case1();
    static bool case2();
    static bool case3();
    static bool case4();
    static bool case5();
    static bool case6();
    static bool case7();
    static bool case8();
    static bool case9();
    static bool case10();
};

class chip_t {
    friend class similar_chip_t;
    static uint32_t width, height;
    static int width_bias, height_bias;
    static uint32_t softCount, fixedCount;
    static size_t total_modules_count;
    static std::unordered_map<std::string, size_t> moduleNameToIndex;
    static std::unordered_map<module_t*, int> module_to_id_m;

    static std::vector<module_t*> modules;
    static std::vector<soft_module_t*> soft_modules;
    static std::vector<fixed_module_t*> fixed_modules;
    static std::vector<std::vector<uint_fast32_t>> connectionTable;

    static std::vector<multi_net_t*> multi_nets;

    static double module_minimum_length;

    static int similar_case_num;

    static void pd_file_input(std::string fileName);

    static void mcnc_file_input(std::string fileName);
public:
    enum file_type_t{
        mcnc,
        iccad_pd
    };
    static void file_input(std::string fileName, file_type_t);

    static int get_similar_case_num();

    static const size_t get_total_module_n();

    static const uint32_t get_width();

    static const uint32_t get_height();

    static const std::vector<module_t *> &get_modules();

    static const std::vector<soft_module_t *> &get_soft_modules();

    static const std::vector<fixed_module_t *> &get_fixed_modules();

    static const std::vector<std::vector<uint_fast32_t>> &get_connection_table();

    static const std::vector<multi_net_t*> &get_multi_nets();

    static const std::unordered_map<std::string, size_t> &get_name_to_index_mapping();

    static double calculate_wire_length(const std::vector<vec2d_t> all_module_left_lower_position);

    static const double &get_module_minimum_length();

    static void decorate_serialize_testcase();
};


#endif //INC_2023PD_CHIP_H
