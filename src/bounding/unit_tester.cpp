//
// Created by 林士傑 on 2023/6/22.
//

#include <iostream>
#include "unit_tester.h"

template<class T>
bool operator==(const std::vector<T>& vec0,const std::vector<T>& vec1){
    if(vec0.size() != vec1.size()) return false;
    for (int i = 0; i < vec0.size(); ++i) {
        if(vec0[i] != vec1[i]) return false;
    }
    return true;
}

template<class T>
std::ostream& operator<<(std::ostream& o,const std::vector<T>& vec0){
    for(auto ele : vec0){
        o << ele << "\n";
    }
    return o;
}

//region vec2d_t

// bool unit_tester::vec2d_t_add0() {
//     std::cout << __func__ << " ";
//     auto vec2D = vec2d_t(3, 4);
//     auto vec2D1 = vec2d_t(6, 7);
//     auto result = vec2d_t(9, 11);
//     return (vec2D + vec2D1) == result;
// }

// bool unit_tester::vec2d_t_add1() {
//     std::cout << __func__ << " ";
//     auto vec2D = vec2d_t(-876543, 3456);
//     auto vec2D1 = vec2d_t(98765421, -14786543);
//     auto result = vec2d_t(97888878, -14783087);
//     return (vec2D + vec2D1) == result;
// }


// bool unit_tester::vec2d_t_add_equal() {
//     std::cout << __func__ << " ";
//     auto vec2D = vec2d_t(5, 7);
//     vec2D += vec2d_t(-12, 3);
//     auto result = vec2d_t(-7, 10);
//     return vec2D == result;
// }

//endregion

//region line_t

bool unit_tester::line_t_constructor0() {
    std::cout << __func__ << " ";
    line_t line = line_t(vec2d_t(0, 0), vec2d_t(0, 5));
    return line.get_line_type() == line_t::line_type::type_vertical && line.get_line_direction_type() == line_t::line_direction_type::direction_type_north;
}

bool unit_tester::line_t_constructor1() {
    std::cout << __func__ << " ";
    line_t line = line_t(vec2d_t(0, 0), vec2d_t(5, 0));
    return line.get_line_type() == line_t::line_type::type_horizontal && line.get_line_direction_type() == line_t::line_direction_type::direction_type_east;
}

bool unit_tester::line_t_constructor2() {
    std::cout << __func__ << " ";
    line_t line = line_t(vec2d_t(0, 0), vec2d_t(0, -5));
    return line.get_line_type() == line_t::line_type::type_vertical && line.get_line_direction_type() == line_t::line_direction_type::direction_type_south;
}

bool unit_tester::line_t_constructor3() {
    std::cout << __func__ << " ";
    line_t line = line_t(vec2d_t(0, 0), vec2d_t(-5, 0));
    return line.get_line_type() == line_t::line_type::type_horizontal && line.get_line_direction_type() == line_t::line_direction_type::direction_type_west;
}

bool unit_tester::line_t_constructor4() {
    std::cout << __func__ << " ";
    line_t line = line_t(vec2d_t(0, 0), vec2d_t(5, 5));
    return line.get_line_type() == line_t::line_type::type_none && line.get_line_direction_type() == line_t::line_direction_type::direction_type_none;
}

bool unit_tester::line_t_constructor5() {
    std::cout << __func__ << " ";
    line_t line = line_t(vec2d_t(0, 0), vec2d_t(0, 0));
    return line.get_line_type() == line_t::line_type::type_none && line.get_line_direction_type() == line_t::line_direction_type::direction_type_none;
}

bool unit_tester::line_t_is_vertical0() {
    std::cout << __func__ << " ";
    line_t line0 = line_t(vec2d_t(0, 0), vec2d_t(1, 0));
    line_t line1 = line_t(vec2d_t(0, 0), vec2d_t(0, 1));
    return line0.is_vertical(line1) == true;
}

bool unit_tester::line_t_is_vertical1() {
    std::cout << __func__ << " ";
    line_t line0 = line_t(vec2d_t(0, 0), vec2d_t(1, 0));
    line_t line1 = line_t(vec2d_t(0, 0), vec2d_t(1, 0));
    return line0.is_vertical(line1) == false;
}

bool unit_tester::line_t_is_parallel0() {
    std::cout << __func__ << " ";
    line_t line0 = line_t(vec2d_t(0, 0), vec2d_t(1, 0));
    line_t line1 = line_t(vec2d_t(0, 0), vec2d_t(0, 1));
    return line0.is_parallel(line1) == false;
}

bool unit_tester::line_t_is_parallel1() {
    std::cout << __func__ << " ";
    line_t line0 = line_t(vec2d_t(0, 0), vec2d_t(1, 0));
    line_t line1 = line_t(vec2d_t(0, 0), vec2d_t(1, 0));
    return line0.is_parallel(line1) == true;
}

bool unit_tester::line_t_is_intersect0() {
    std::cout << __func__ << " ";
    line_t line0 = line_t(vec2d_t(0, 0), vec2d_t(1, 0));
    line_t line1 = line_t(vec2d_t(0, 0), vec2d_t(1, 0));
    return line0.is_intersect(line1) == line_t::line_collision_type::collision_horizontal;
}

bool unit_tester::line_t_is_intersect1() {
    std::cout << __func__ << " ";
    line_t line0 = line_t(vec2d_t(0, 0), vec2d_t(1, 0));
    line_t line1 = line_t(vec2d_t(1, 0), vec2d_t(2, 0));
    return line0.is_intersect(line1) == line_t::line_collision_type::collision_type_none;
}

bool unit_tester::line_t_is_touch0() {
    std::cout << __func__ << " ";
    line_t line0 = line_t(vec2d_t(0, 0), vec2d_t(1, 0));
    line_t line1 = line_t(vec2d_t(0, 0), vec2d_t(1, 0));
    return line0.is_touch(line1) == line_t::line_collision_type::collision_horizontal;
}

bool unit_tester::line_t_is_touch1() {
    std::cout << __func__ << " ";
    line_t line0 = line_t(vec2d_t(0, 0), vec2d_t(1, 0));
    line_t line1 = line_t(vec2d_t(1, 0), vec2d_t(2, 0));
    return line0.is_touch(line1) == line_t::line_collision_type::collision_horizontal;
}

bool unit_tester::line_t_parallel_cut_reverse0() {
    std::cout << __func__ << " ";
    line_t line0 = line_t(vec2d_t(3, 0), vec2d_t(0, 0));
    line_t line1 = line_t(vec2d_t(1, 2), vec2d_t(0, 2));
    auto coli = line0.parallel_cut_reverse(line1);
    if(coli.has_value()) return false;
    return true;
}

bool unit_tester::line_t_parallel_cut_reverse1() {
    std::cout << __func__ << " ";
    line_t line0 = line_t(vec2d_t(0, 0), vec2d_t(0, 3));
    line_t line1 = line_t(vec2d_t(0, 1), vec2d_t(0, 2));
    auto coli = line0.parallel_cut_reverse(line1);
    if(!coli.has_value()) return false;
    std::vector<line_t> result = std::vector<line_t> {
            line_t(vec2d_t(0, 2), vec2d_t(0, 3)),
            line_t(vec2d_t(0, 1), vec2d_t(0, 2)),
            line_t(vec2d_t(0, 0), vec2d_t(0, 1))
    };
    if(coli->size() != result.size()) return false;
    for (int i = 0; i < coli->size(); ++i) {
        if(coli.value()[i] != result[i]) return false;
    }

    return true;
}

bool unit_tester::line_t_parallel_cut_reverse2() {
    std::cout << __func__ << " ";
    line_t line0 = line_t(vec2d_t(0, 1), vec2d_t(0, 2));
    line_t line1 = line_t(vec2d_t(0, 0), vec2d_t(0, 3));
    auto coli = line0.parallel_cut_reverse(line1);
    if(!coli.has_value()) return false;
    std::vector<line_t> result = std::vector<line_t> {
            line_t(vec2d_t(0, 1), vec2d_t(0, 2))
    };
    if(coli->size() != result.size()) return false;
    for (int i = 0; i < coli->size(); ++i) {
        if(coli.value()[i] != result[i]) return false;
    }
    return true;
}

bool unit_tester::line_t_parallel_cut_reverse3() {
    std::cout << __func__ << " ";
    line_t line0 = line_t(vec2d_t(0, 2), vec2d_t(0, 1));
    line_t line1 = line_t(vec2d_t(0, 0), vec2d_t(0, 3));
    auto coli = line0.parallel_cut_reverse(line1);
    return false;
}

//endregion

//region bounding_line_t

// bool unit_tester::bounding_line_t_merge0() {
//     bounding_line_t boundingLine0 = bounding_line_t({
//                                                             vec2d_t(0, 0),
//                                                             vec2d_t(3, 0),
//                                                             vec2d_t(3, 3),
//                                                             vec2d_t(0, 3),
//                                                             vec2d_t(0, 2),
//                                                             vec2d_t(-1, 2),
//                                                             vec2d_t(-1, 1),
//                                                             vec2d_t(0, 1)
//                                                     }, false);

//     bounding_line_t boundingLine1 = bounding_line_t({
//                                                             vec2d_t(-1, 1),
//                                                             vec2d_t(-1, 2),
//                                                             vec2d_t(3, 2),
//                                                             vec2d_t(3, 1)
//                                                     });
// //    std::cout << "want ";
// //    boundingLine0.print();
// //    boundingLine1.print();
// //    boundingLine0.print_reverse();
// //    std::cout << "want ";
// //    boundingLine1.print_reverse();
//     std::cout << boundingLine0.merge(boundingLine1);
// //    std::cout << boundingLine0.merge(boundingLine1);
//     return false;
// }

// bool unit_tester::bounding_line_t_merge1() {
//     bounding_line_t boundingLine0 = bounding_line_t({
//                                                             vec2d_t(3, 1),
//                                                             vec2d_t(3, 2),
//                                                             vec2d_t(-1, 2),
//                                                             vec2d_t(-1, 1)
//                                                     }, false);

//     bounding_line_t boundingLine1 = bounding_line_t({
//                                                             vec2d_t(-1, 1),
//                                                             vec2d_t(-1, 2),
//                                                             vec2d_t(3, 2),
//                                                             vec2d_t(3, 1)
//                                                     });
//     std::cout << boundingLine0.merge(boundingLine1);
//     return false;
// }

// bool unit_tester::bounding_line_t_merge2() {
//     bounding_line_t boundingLine0 = bounding_line_t({
//                                                             vec2d_t(30, 10),
//                                                             vec2d_t(30, 20),
//                                                             vec2d_t(10, 20),
//                                                             vec2d_t(10, 10)
//                                                     }, false);

//     bounding_line_t boundingLine1 = bounding_line_t({
//                                                             vec2d_t(-1, 1),
//                                                             vec2d_t(-1, 2),
//                                                             vec2d_t(3, 2),
//                                                             vec2d_t(3, 1)
//                                                     });
//     std::cout << boundingLine0.merge(boundingLine1);
//     return false;
// }

// bool unit_tester::bounding_line_t_merge3() {
//     bounding_line_t boundingLine0 = bounding_line_t({
//                                                             vec2d_t(0, 0),
//                                                             vec2d_t(3, 0),
//                                                             vec2d_t(3, 3),
//                                                             vec2d_t(0, 3)
//                                                     }, false);

//     bounding_line_t boundingLine1 = bounding_line_t({
//                                                             vec2d_t(0, 0),
//                                                             vec2d_t(0, 3),
//                                                             vec2d_t(3, 3),
//                                                             vec2d_t(3, 0)
//                                                     });
//     std::cout << boundingLine0.merge(boundingLine1);
//     return false;
// }

// bool unit_tester::bounding_line_t_merge4() {
//     bounding_line_t boundingLine0 = bounding_line_t({
//                                                             vec2d_t(0, 0),
//                                                             vec2d_t(2, 0),
//                                                             vec2d_t(2, 1),
//                                                             vec2d_t(5, 1),
//                                                             vec2d_t(5, 2),
//                                                             vec2d_t(4, 2),
//                                                             vec2d_t(4, 3),
//                                                             vec2d_t(5, 3),
//                                                             vec2d_t(5, 4),
//                                                             vec2d_t(4, 4),
//                                                             vec2d_t(4, 5),
//                                                             vec2d_t(3, 5),
//                                                             vec2d_t(3, 4),
//                                                             vec2d_t(1, 4),
//                                                             vec2d_t(1, 3),
//                                                             vec2d_t(0, 3)
//                                                     }, false);

//     bounding_line_t boundingLine1 = bounding_line_t({
//                                                             vec2d_t(1, 1),
//                                                             vec2d_t(1, 4),
//                                                             vec2d_t(4, 4),
//                                                             vec2d_t(4, 1)
//                                                     });
//     auto origin = boundingLine0.get_nodes();
//     auto module = boundingLine1.get_nodes();
//     auto ans = boundingLine0.merge(boundingLine1);
//     std::cout << ans;
// //    auto vectors = std::vector<std::pair<std::vector<vec2d_t>,std::string>>();
//     auto vectors = {
//             std::make_pair(ans[0].get_nodes(), std::string("A")),
//             std::make_pair(ans[1].get_nodes(), std::string("B")),
//             std::make_pair(ans[2].get_nodes(), std::string("C")),
// //            std::make_pair(origin, std::string("origin")),
// //            std::make_pair(module, std::string("module"))

//     };
//     visualizer_t::draw_bounding_line(vectors);
//     return false;
// }

// bool unit_tester::bounding_line_t_merge5() {
//     bounding_line_t boundingLine0 = bounding_line_t({
//                                                             vec2d_t(0, 1),
//                                                             vec2d_t(4, 1),
//                                                             vec2d_t(4, 4),
//                                                             vec2d_t(0, 4)
//                                                     }, false);

//     bounding_line_t boundingLine1 = bounding_line_t({
//                                                             vec2d_t(1, 0),
//                                                             vec2d_t(1, 5),
//                                                             vec2d_t(3, 5),
//                                                             vec2d_t(3, 0)
//                                                     });
//     auto origin = boundingLine0.get_nodes();
//     auto module = boundingLine1.get_nodes();
//     auto ans = boundingLine0.merge(boundingLine1);
//     std::cout << ans;
// //    auto vectors = std::vector<std::pair<std::vector<vec2d_t>,std::string>>();
//     auto vectors = {
//             std::make_pair(ans[0].get_nodes(), std::string("A")),
//             std::make_pair(ans[1].get_nodes(), std::string("B")),
// //            std::make_pair(ans[2].get_nodes(), std::string("C")),
// //            std::make_pair(origin, std::string("origin")),
// //            std::make_pair(module, std::string("module"))

//     };
//     visualizer_t::draw_bounding_line(vectors);
//     return false;
// }

//endregion

//region circular_base_list_t

// bool unit_tester::circular_base_list_t_concat_next0() {
//     std::cout << __func__ << " ";
//     circular_T_list_t circularList = circular_T_list_t<int>();
//     circularList.add_at_head(1);
//     circularList.concat_next(circularList.get_root()->get_next(), 2);
//     auto a = circularList.get_root();
//     auto b = a->get_next();
//     auto c = b->get_next();
//     if(circularList.get_length() != 2) return false;
//     if(a->get_data() != 0) return false;
//     if(b->get_data() != 1) return false;
//     if(c->get_data() != 2) return false;

//     if(a->get_prev() != c) return false;
//     if(c->get_prev() != b) return false;
//     if(b->get_prev() != a) return false;
//     if(a->get_next() != b) return false;
//     if(b->get_next() != c) return false;
//     if(c->get_next() != b) return false;
//     return true;
// }

// bool unit_tester::circular_base_list_t_concat_next1() {
//     std::cout << __func__ << " ";
//     circular_T_list_t circularList = circular_T_list_t<int>();
//     circularList.add_at_tail(1);
//     circularList.add_at_tail(2);
//     circularList.concat_next(circularList.get_root()->get_next(), 3);
//     auto a = circularList.get_root();
//     auto b = a->get_next();
//     auto c = b->get_next();
//     auto d = c->get_next();

//     if(circularList.get_length() != 3) return false;
//     if(a->get_data() != 0) return false;
//     if(b->get_data() != 1) return false;
//     if(c->get_data() != 3) return false;
//     if(d->get_data() != 2) return false;

//     if(a->get_prev() != d) return false;
//     if(d->get_prev() != c) return false;
//     if(c->get_prev() != b) return false;
//     if(b->get_prev() != a) return false;
//     if(a->get_next() != b) return false;
//     if(b->get_next() != c) return false;
//     if(c->get_next() != d) return false;
//     if(d->get_next() != b) return false;
//     return true;
// }

// bool unit_tester::circular_base_list_t_concat_next2() {
//     std::cout << __func__ << " ";
//     circular_T_list_t circularList = circular_T_list_t<int>();
//     circularList.add_at_tail(1);
//     circularList.add_at_tail(2);
//     circularList.concat_next(circularList.get_root()->get_next()->get_next(), 3);
//     auto a = circularList.get_root();
//     auto b = a->get_next();
//     auto c = b->get_next();
//     auto d = c->get_next();

//     if(circularList.get_length() != 3) return false;
//     if(a->get_data() != 0) return false;
//     if(b->get_data() != 1) return false;
//     if(c->get_data() != 2) return false;
//     if(d->get_data() != 3) return false;

//     if(a->get_prev() != d) return false;
//     if(d->get_prev() != c) return false;
//     if(c->get_prev() != b) return false;
//     if(b->get_prev() != a) return false;
//     if(a->get_next() != b) return false;
//     if(b->get_next() != c) return false;
//     if(c->get_next() != d) return false;
//     if(d->get_next() != b) return false;
//     return true;
// }

// bool unit_tester::circular_base_list_t_concat_next3() {
//     std::cout << __func__ << " ";
//     circular_T_list_t circularList = circular_T_list_t<int>();
//     circularList.add_at_tail(1);
//     circularList.add_at_tail(2);
//     circularList.add_at_tail(3);
//     circularList.concat_next(circularList.get_root()->get_next(), 4);
//     auto a = circularList.get_root();
//     auto b = a->get_next();
//     auto c = b->get_next();
//     auto d = c->get_next();
//     auto e = d->get_next();

//     if(circularList.get_length() != 4) return false;
//     if(a->get_data() != 0) return false;
//     if(b->get_data() != 1) return false;
//     if(c->get_data() != 4) return false;
//     if(d->get_data() != 2) return false;
//     if(e->get_data() != 3) return false;

//     if(a->get_prev() != e) return false;
//     if(e->get_prev() != d) return false;
//     if(d->get_prev() != c) return false;
//     if(c->get_prev() != b) return false;
//     if(b->get_prev() != a) return false;

//     if(a->get_next() != b) return false;
//     if(b->get_next() != c) return false;
//     if(c->get_next() != d) return false;
//     if(d->get_next() != e) return false;
//     if(e->get_next() != b) return false;
//     return true;
// }

// bool unit_tester::circular_base_list_t_concat_next4() {
//     std::cout << __func__ << " ";
//     circular_T_list_t circularList = circular_T_list_t<int>();
//     circularList.add_at_tail(1);
//     circularList.add_at_tail(2);
//     circularList.add_at_tail(3);
//     circularList.concat_next(circularList.get_root()->get_next()->get_next(), 4);
//     auto a = circularList.get_root();
//     auto b = a->get_next();
//     auto c = b->get_next();
//     auto d = c->get_next();
//     auto e = d->get_next();

//     if(circularList.get_length() != 4) return false;
//     if(a->get_data() != 0) return false;
//     if(b->get_data() != 1) return false;
//     if(c->get_data() != 2) return false;
//     if(d->get_data() != 4) return false;
//     if(e->get_data() != 3) return false;

//     if(a->get_prev() != e) return false;
//     if(e->get_prev() != d) return false;
//     if(d->get_prev() != c) return false;
//     if(c->get_prev() != b) return false;
//     if(b->get_prev() != a) return false;

//     if(a->get_next() != b) return false;
//     if(b->get_next() != c) return false;
//     if(c->get_next() != d) return false;
//     if(d->get_next() != e) return false;
//     if(e->get_next() != b) return false;
//     return true;
// }

// bool unit_tester::circular_base_list_t_concat_next5() {
//     std::cout << __func__ << " ";
//     circular_T_list_t circularList = circular_T_list_t<int>();
//     circularList.add_at_tail(1);
//     circularList.add_at_tail(2);
//     circularList.add_at_tail(3);
//     circularList.concat_next(circularList.get_root()->get_next()->get_next()->get_next(), 4);
//     auto a = circularList.get_root();
//     auto b = a->get_next();
//     auto c = b->get_next();
//     auto d = c->get_next();
//     auto e = d->get_next();

//     if(circularList.get_length() != 4) return false;
//     if(a->get_data() != 0) return false;
//     if(b->get_data() != 1) return false;
//     if(c->get_data() != 2) return false;
//     if(d->get_data() != 3) return false;
//     if(e->get_data() != 4) return false;

//     if(a->get_prev() != e) return false;
//     if(e->get_prev() != d) return false;
//     if(d->get_prev() != c) return false;
//     if(c->get_prev() != b) return false;
//     if(b->get_prev() != a) return false;

//     if(a->get_next() != b) return false;
//     if(b->get_next() != c) return false;
//     if(c->get_next() != d) return false;
//     if(d->get_next() != e) return false;
//     if(e->get_next() != b) return false;
//     return true;
// }
// bool unit_tester::circular_base_list_t_add_at_tail0() {
//     std::cout << __func__ << " ";
//     circular_T_list_t circularList = circular_T_list_t<int>();
//     circularList.add_at_tail(1);
//     circularList.add_at_tail(3);
//     circularList.add_at_tail(2);
//     circularList.add_at_tail(4);
//     auto cir_vec = circularList.to_vector_reverse();
//     auto result = std::vector<int>{4, 2, 3, 1};
//     return cir_vec == result;
// }

// bool unit_tester::circular_base_list_t_add_at_tail1() {
//     std::cout << __func__ << " ";
//     auto circularList = circular_T_list_t<int>();
//     circularList.add_at_tail(1000);
//     circularList.add_at_tail(-1420);
//     circularList.add_at_tail(-10);
//     circularList.add_at_tail(10);
//     auto cir_vec = circularList.to_vector_reverse();
//     auto result = std::vector<int>{10, -10, -1420, 1000};
//     return cir_vec == result;
// }

// bool unit_tester::circular_base_list_t_add_at_head0() {
//     std::cout << __func__ << " ";
//     auto circularList = circular_T_list_t<int>();
//     circularList.add_at_head(1000);
//     circularList.add_at_head(-1420);
//     circularList.add_at_head(-10);
//     circularList.add_at_head(10);
//     auto cir_vec = circularList.to_vector_reverse();
//     auto result = std::vector<int>{1000, -1420, -10, 10};
//     return cir_vec == result;
// }

// bool unit_tester::circular_base_list_t_add_at_head1() {
//     std::cout << __func__ << " ";
//     auto circularList = circular_T_list_t<int>();
//     circularList.add_at_head(1);
//     circularList.add_at_head(-1);
//     circularList.add_at_head(-10);
//     circularList.add_at_head(102);
//     auto cir_vec = circularList.to_vector_reverse();
//     auto result = std::vector<int>{1, -1, -10, 102};
//     return cir_vec == result;
// }

// bool unit_tester::circular_base_list_t_delete_node_get_next0() {
//     std::cout << __func__ << " ";
//     auto circularList = circular_T_list_t<int>();
//     circularList.add_at_head(1);
//     circularList.add_at_head(-1);
//     circularList.add_at_head(-10);
//     circularList.add_at_head(102);
//     circularList.add_at_head(-10234);
//     auto next = circularList.delete_node_get_next(circularList.get_root()->get_next());
//     if(next->get_data() != 102) return false;
//     std::vector<int> result = std::vector<int>{ 1, -1, -10, 102};
//     return circularList.to_vector_reverse() == result;
// }

// bool unit_tester::circular_base_list_t_delete_node_get_next1() {
//     std::cout << __func__ << " ";
//     auto circularList = circular_T_list_t<int>();
//     circularList.add_at_head(1);
//     circularList.add_at_head(-1);
//     auto next = circularList.delete_node_get_next(circularList.get_root()->get_next());
//     if(next->get_data() != 1) return false;
//     std::vector<int> result = std::vector<int>{ 1 };
//     return circularList.to_vector_reverse() == result;
// }

// bool unit_tester::circular_base_list_t_delete_node_get_next2() {
//     std::cout << __func__ << " ";
//     auto circularList = circular_T_list_t<int>();
//     circularList.add_at_head(1);
//     auto next = circularList.delete_node_get_next(circularList.get_root()->get_next());
//     if(next->get_data() != 0) return false;
//     std::vector<int> result = std::vector<int>{ };
//     auto a = circularList.to_vector_reverse();
//     return circularList.to_vector_reverse() == result;
// }

// bool unit_tester::circular_base_list_t_delete_node_get_next3() {
//     std::cout << __func__ << " ";
//     auto circularList = circular_T_list_t<int>();
//     circularList.add_at_head(1);
//     circularList.add_at_head(-1);
//     auto next = circularList.delete_node_get_next(circularList.get_root()->get_next()->get_next());
//     if(next->get_data() != -1) return false;
//     std::vector<int> result = std::vector<int>{ -1 };
//     return circularList.to_vector_reverse() == result;
// }

bool unit_tester::circular_base_list_t_delete_node_get_next4() {
    std::cout << __func__ << " ";
    return false;
}

bool unit_tester::circular_base_list_t_delete_node_get_next5() {
    std::cout << __func__ << " ";
    return false;
}

bool unit_tester::circular_base_list_t_delete_node_get_next6() {
    std::cout << __func__ << " ";
    return false;
}

bool unit_tester::circular_base_list_t_to_vector_reverse0() {
    std::cout << __func__ << " ";
    auto circularList = circular_T_list_t<int>();
    circularList.add_at_head(1);
    circularList.add_at_head(-1);
    auto result = std::vector<int> {1, -1};
    return circularList.to_vector_reverse() == result;
}

bool unit_tester::circular_base_list_t_to_vector_reverse1() {
    std::cout << __func__ << " ";
    auto circularList = circular_T_list_t<int>();
    circularList.add_at_head(1);
    circularList.add_at_head(-1);
    circularList.add_at_head(-1);
    circularList.add_at_head(-1);
    auto result = std::vector<int> {1, -1, -1, -1};
    return circularList.to_vector_reverse() == result;
}

bool unit_tester::circular_base_list_t_to_vector_reverse2() {
    std::cout << __func__ << " ";
    auto circularList = circular_T_list_t<int>();
    auto result = std::vector<int> {};
    return circularList.to_vector_reverse() == result;
}

bool unit_tester::circular_base_list_t_to_vector0() {
    std::cout << __func__ << " ";
    auto circularList = circular_T_list_t<int>();
    circularList.add_at_head(1);
    circularList.add_at_head(-1);
    auto result = std::vector<int> {-1, 1};
    return circularList.to_vector() == result;
}

bool unit_tester::circular_base_list_t_to_vector1() {
    std::cout << __func__ << " ";
    auto circularList = circular_T_list_t<int>();
    circularList.add_at_head(1);
    circularList.add_at_head(-1);
    circularList.add_at_head(-1);
    circularList.add_at_head(-1);
    auto result = std::vector<int> {-1, -1, -1, 1};
    return circularList.to_vector() == result;
}

bool unit_tester::circular_base_list_t_to_vector2() {
    std::cout << __func__ << " ";
    auto circularList = circular_T_list_t<int>();
    auto result = std::vector<int> {};
    return circularList.to_vector() == result;
}

bool unit_tester::circular_base_list_t_to_vector_multi_round0() {
    std::cout << __func__ << " ";
    auto circularList = circular_T_list_t<int>();
    circularList.add_at_head(1);
    circularList.add_at_head(-1);
    auto result = std::vector<int> {-1, 1};
    return circularList.to_vector_multi_round(1) == result;
}

bool unit_tester::circular_base_list_t_to_vector_multi_round1() {
    std::cout << __func__ << " ";
    auto circularList = circular_T_list_t<int>();
    circularList.add_at_head(1);
    circularList.add_at_head(-1);
    auto result = std::vector<int> {-1, 1, -1, 1};
    return circularList.to_vector_multi_round(2) == result;
}

bool unit_tester::circular_base_list_t_to_vector_multi_round2() {
    std::cout << __func__ << " ";
    auto circularList = circular_T_list_t<int>();
    circularList.add_at_head(1);
    circularList.add_at_head(-1);
    circularList.add_at_head(-1);
    circularList.add_at_head(-1);
    auto result = std::vector<int> {-1, -1, -1, 1, -1, -1, -1, 1, -1, -1, -1, 1};
    return circularList.to_vector_multi_round(3) == result;
}

bool unit_tester::circular_base_list_t_memory_control() {
    for (int i = 0; i < 1E8; ++i) {
        auto list = circular_T_list_t<int>();
        list.add_at_tail(0);
        list.add_at_tail(0);
        list.add_at_tail(0);
        list.add_at_tail(0);
        list.add_at_tail(0);
        list.add_at_tail(0);
        list.add_at_tail(0);
        list.add_at_tail(0);
        list.add_at_tail(0);
        list.add_at_tail(0);
    }
    system("pause");
    return true;
}

//endregion

//region bounding_line_handler_t

//endregion

void unit_tester::runner() {
    auto funcs = {
//            &unit_tester::vec2d_t_add0,
//            &unit_tester::vec2d_t_add1,
//            &unit_tester::vec2d_t_add_equal,
//            &unit_tester::line_t_constructor0,
//            &unit_tester::line_t_constructor1,
//            &unit_tester::line_t_constructor2,
//            &unit_tester::line_t_constructor3,
//            &unit_tester::line_t_constructor4,
//            &unit_tester::line_t_constructor5,
//            &unit_tester::line_t_is_vertical0,
//            &unit_tester::line_t_is_vertical1,
//            &unit_tester::line_t_is_parallel0,
//            &unit_tester::line_t_is_parallel1,
//            &unit_tester::line_t_is_intersect0,
//            &unit_tester::line_t_is_intersect1,
//            &unit_tester::line_t_is_touch0,
//            &unit_tester::line_t_is_touch1,
//            &unit_tester::line_t_parallel_cut_reverse0,
//            &unit_tester::line_t_parallel_cut_reverse1,
//            &unit_tester::line_t_parallel_cut_reverse2,
//            &unit_tester::bounding_line_t_merge0,
//            &unit_tester::bounding_line_t_merge1,
//            &unit_tester::bounding_line_t_merge2,
//            &unit_tester::bounding_line_t_merge3,
            &unit_tester::bounding_line_t_merge4,
//            &unit_tester::bounding_line_t_merge5,
//            &unit_tester::circular_base_list_t_concat_next0,
//            &unit_tester::circular_base_list_t_concat_next1,
//            &unit_tester::circular_base_list_t_concat_next2,
//            &unit_tester::circular_base_list_t_concat_next3,
//            &unit_tester::circular_base_list_t_concat_next4,
//            &unit_tester::circular_base_list_t_concat_next5,
//            &unit_tester::circular_base_list_t_to_vector_reverse0,
//            &unit_tester::circular_base_list_t_to_vector_reverse1,
//            &unit_tester::circular_base_list_t_to_vector_reverse2,
//            &unit_tester::circular_base_list_t_to_vector0,
//            &unit_tester::circular_base_list_t_to_vector1,
//            &unit_tester::circular_base_list_t_to_vector2,
//            &unit_tester::circular_base_list_t_to_vector_multi_round0,
//            &unit_tester::circular_base_list_t_to_vector_multi_round1,
//            &unit_tester::circular_base_list_t_to_vector_multi_round2,
//            &unit_tester::circular_base_list_t_delete_node_get_next0,
//            &unit_tester::circular_base_list_t_delete_node_get_next1,
//            &unit_tester::circular_base_list_t_delete_node_get_next2,
//            &unit_tester::circular_base_list_t_delete_node_get_next3,
//            &unit_tester::circular_base_list_t_add_at_tail0,

//            &unit_tester::circular_base_list_t_add_at_tail1,
//            &unit_tester::circular_base_list_t_add_at_head0
//            &unit_tester::circular_base_list_t_add_at_head1,
//            &unit_tester::circular_base_list_t_memory_control
    };

    for(auto func : funcs){
        std::cout << "run " << " : " << ((this->*func)() ? "true" : "false" ) << std::endl;
    }
}
