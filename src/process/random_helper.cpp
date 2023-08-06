//
// Created by RayChang on 2023/7/14.
//

#include "random_helper.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
using std::cout;
using std::endl;
int random_helper::rand_table_n = 1e8 + 1;
int random_helper::rand_table_i = 0;
vector<int> random_helper::rand_table;
void random_helper::shuffle(vector<vector<int>>& a) {
    for(int i = 0; i<a.size(); ++i){
        int x = random_helper::get_rand()%a.size();
        std::swap(a[i], a[x]);
    }
}
void random_helper::shuffle(vector<int>& a) {
    for(int i = 0; i<a.size(); ++i){
        int x = random_helper::get_rand()%a.size();
        std::swap(a[i], a[x]);
    }
}
vector<int> random_helper::get_random_int_vec(int n) {
    vector<int> a(n);
    for(int i = 0; i<n; ++i){
        a[i] = i;
    }
    for(int i = 0; i<a.size(); ++i){
        int x = random_helper::get_rand()%a.size();
        std::swap(a[i], a[x]);
    }
    return a;
}

vector<int> random_helper::rand_list(int n) {
    vector<int> res(n);
    for(int i = 0; i<n; ++i){
        res[i] = i;
    }
    for(int i = 0; i<n; ++i){
        int x = random_helper::get_rand()%n;
        std::swap(res[i], res[x]);
    }
    return res;
}

void random_helper::set_seed() {
    srand(103);
    rand_table.resize(rand_table_n);
    for(int i = 0; i < rand_table_n; ++i){
        rand_table[i] = rand();
    }
}

int random_helper::get_rand() {
    int ret = rand_table[random_helper::rand_table_i];
    random_helper::rand_table_i =  (random_helper::rand_table_i+1)%(random_helper::rand_table_n);
    return ret;
}
