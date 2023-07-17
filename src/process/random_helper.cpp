//
// Created by RayChang on 2023/7/14.
//

#include "random_helper.h"
#include <cstdlib>
#include <ctime>
void random_helper::shuffle(vector<vector<int>>& a) {
    for(int i = 0; i<a.size(); ++i){
        int x = rand()%a.size();
        std::swap(a[i], a[x]);
    }
}

vector<int> random_helper::get_random_int_vec(int n) {
    vector<int> a(n);
    for(int i = 0; i<n; ++i){
        a[i] = i;
    }
    for(int i = 0; i<a.size(); ++i){
        int x = rand()%a.size();
        std::swap(a[i], a[x]);
    }
    return a;
}
