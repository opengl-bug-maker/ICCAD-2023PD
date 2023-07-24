//
// Created by RayChang on 2023/7/14.
//

#ifndef ICCAD2023PD_RANDOM_HELPER_H
#define ICCAD2023PD_RANDOM_HELPER_H

#include <vector>
using std::vector;
class random_helper {
public:
    static void shuffle(vector<vector<int>>&);

    static vector<int> get_random_int_vec(int n);

    static void shuffle(vector<int> &a);

    static vector<int> rand_list(int n);
};


#endif //ICCAD2023PD_RANDOM_HELPER_H
