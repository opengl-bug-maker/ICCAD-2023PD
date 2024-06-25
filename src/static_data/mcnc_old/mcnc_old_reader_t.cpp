#include "mcnc_old_reader_t.h"
#include <iostream>

mcnc_old_module_t *mcnc_old_reader_t::get_module_from_name(std::string name) {
    for(mcnc_old_module_t* m : this->modules) {
        if(m->name == name) return m;
    }
    for(mcnc_old_module_t* m : this->fix_modules) {
        if(m->name == name) return m;
    }
    return nullptr;
}

int mcnc_old_reader_t::is_terminal_name(std::string name) {
    for (int i = 0; i < this->terminals.size(); ++i) {
        if (this->terminals[i]->name == name) {
            return i;
        }
    }
    return -1;
}

void mcnc_old_reader_t::file_input(std::fstream &blocks_file, std::fstream &nets_file, std::fstream &fixeds_file) {
    this->modules.clear();
    this->connections.clear();

    //blocks file

    int soft_num = 0;
    int hard_num = 0;
    int termi = 0;
    std::string line, word, charac, type_word;
    for (int i = 0 ; i < 5 ; i++){
        std::getline(blocks_file, line);
    }
    while(blocks_file.eof() == 0) {
        blocks_file >> word;
        if (word == "NumSoftRectangularBlocks"){
            blocks_file >> charac;
            blocks_file >> soft_num;
        }else if (word == "NumHardRectilinearBlocks"){
            blocks_file >> charac;
            blocks_file >> hard_num;
        }else if(word == "NumTerminals"){
            blocks_file >> charac;
            blocks_file >> termi;
        }else {
            blocks_file >> type_word;
            if(type_word == "softrectangular"){
                int area = 0;
                blocks_file >> area;
                std::getline(blocks_file, line);
                mcnc_old_soft_moudle_t *m = new mcnc_old_soft_moudle_t();
                m->name = word;
                m->min_area = area;
                mcnc_old_terminal_t* terminal = new mcnc_old_terminal_t();
                terminal->name = word + "pin";
                terminal->x = 0;
                terminal->y = 0;
                m->pins.push_back(terminal);
                this->modules.push_back(m);
            } else if (type_word == "hardrectilinear"){
                int bounding_point;
                blocks_file >> bounding_point;
                double left = 1e9, right = -1e9, upper = -1e9, lower = 1e9;
                for (int i = 0; i < bounding_point; ++i) {
                    char t;
                    int x, y;
                    blocks_file >> t >> x >> t >> y >> t;
                    if(x < left) left = x;
                    if(x > right) right = x;
                    if(y < lower) lower = y;
                    if(y > upper) upper = y;
                }
                mcnc_old_fixed_module_t* fm = new mcnc_old_fixed_module_t();
                fm->name = word;
                fm->rect = rect_t({left, lower}, {right - left, upper - lower});
                mcnc_old_terminal_t* terminal = new mcnc_old_terminal_t();
                terminal->name = fm->name + "pin";
                terminal->x = 0;
                terminal->y = 0;
                fm->pins.push_back(terminal);
                this->fix_modules.push_back(fm);
            } else if (type_word == "terminal") {
                mcnc_old_terminal_t* terminal = new mcnc_old_terminal_t();
                terminal->name = word;
                this->terminals.push_back(terminal);
            } else {
                perror("Error: Invalid type_word");
            }
        }
    }
    // std::cout << "\nmodule\n";
    // for(auto sm : this->modules) {
    //     std::cout << sm->name << " | ";
    // }
    // std::cout << "\nmodule\n";
    // std::cout << "\nmodule\n";
    // for(auto sm : this->fix_modules) {
    //     std::cout << sm->name << " | ";
    // }
    // std::cout << "\nmodule\n";
    mcnc_old_fixed_module_t* board = new mcnc_old_fixed_module_t();
    board->name = "board";
    board->rect = rect_t({0, 0}, {0, 0});

    // pl file
    line = "";
    word = "";
    for (int i = 0; i < 5; ++i) {
        std::getline(fixeds_file, line);
    }

    while(fixeds_file >> word) {
        int x, y;
        fixeds_file >> x >> y;
        this->terminals[this->is_terminal_name(word)]->x = x;
        this->terminals[this->is_terminal_name(word)]->y = y;
    }
    for(auto terminal : this->terminals) {
        board->pins.push_back(terminal);
    }
    this->fix_modules.push_back(board);


    //nets file
    int net_num = 0;
    int temp;
    line = "";
    word = "";
    charac = "";
    type_word = "";
    for (int i = 0 ; i < 5 ; i++){
        std::getline(nets_file, line);
    }
    while(nets_file.eof() == 0) {
        nets_file >> word;
        if (word == "NumNets"){
            nets_file >> charac;
            nets_file >> net_num;
        }else if (word == "NumPins"){
            std::getline(nets_file, line);    
        }else if(word == "NetDegree"){
            int deg_num = 0;
            nets_file >> charac;
            nets_file >> deg_num;
            mcnc_old_connection_t* connection = new mcnc_old_connection_t();
            for(int i = 0; i < deg_num; i++){
                nets_file >> word;
                if(word == "#"){
                    std::getline(nets_file, line);
                    i--;
                    continue;
                }
                std::string pin_type;
                nets_file >> pin_type;
                mcnc_old_connection_pin_t* connection_pin = new mcnc_old_connection_pin_t();
                connection_pin->is_terminal = this->is_terminal_name(word) >= 0;
                if(!connection_pin->is_terminal) {
                    connection_pin->belong_module = this->get_module_from_name(word);
                    connection_pin->name = connection_pin->belong_module->pins[0]->name;
                    // std::cout << "check0" << word << "|\n";
                    // std::cout << "check" << this->get_module_from_name(word)->name << "\n";
                    // std::cout << "check2" << connection_pin->belong_module->name << "\n";
                    char c;
                    nets_file >> c >> c >> connection_pin->x >> c >> connection_pin->y;
                } else {
                    connection_pin->belong_module = this->fix_modules.back();
                    connection_pin->name = word;
                    connection_pin->x = this->terminals[this->is_terminal_name(word)]->x;
                    connection_pin->y = this->terminals[this->is_terminal_name(word)]->y;
                }
                connection->modules.push_back(connection_pin);
            }
            this->connections.push_back(connection);
        }
    }

    // std::cout << "\nconnection\n";
    // for(auto conn : this->connections) {
    //     for(auto cp : conn->modules) {
    //         std::cout << cp->belong_module->name << " | ";
    //     }
    //     std::cout << "\n";
    // }
    // std::cout << "\nconnection end\n";

    // //fixed file
    // fixeds_file >> this->chipw >> this->chiph;
    // std::string name;
    // while(fixeds_file >> name) {
    //     mcnc_old_fixed_module_t* fm = new mcnc_old_fixed_module_t();
    //     int llx, lly, w, h;
    //     fixeds_file >> llx >> lly >> w >> h;
    //     fm->name = name;
    //     fm->rect = rect_t({llx, lly}, {w, h});
    //     fm->pins.push_back(fm->name + "pin");
    //     mcnc_old_soft_moudle_t* m = this->get_module_from_name(fm->name);
    //     if(m != nullptr) this->modules.erase(std::find(this->modules.begin(), this->modules.end(), m));
    // }

}