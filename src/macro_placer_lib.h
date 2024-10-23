
#include<vector>
using namespace std;
class SP_result{
public:
    vector<vec2d_t> modules_wh, modules_pos;
    void print();
};

class macro_placer_t{
public:
    SP_result result;
    vector<vector<int>> matrix;
    const int matrix_size = 100;
    macro_placer_t(string);
    void fill_matrix();
    void normalization_by_chip_size();
    void print_matrix();
};

