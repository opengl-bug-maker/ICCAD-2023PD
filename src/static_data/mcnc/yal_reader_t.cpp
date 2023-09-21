//
// Created by 林士傑 on 2023/9/18.
//

#include <exception>
#include <iostream>
#include "yal_reader_t.h"

void yal_reader_t::file_input(std::fstream &file) {
    std::string str;
    while(file >> str){
        if(str == "MODULE"){
            this->modules.push_back(mcnc_module_t::file_input(file));
        }else{
            std::cout << str << "\n";
            std::cout << "ERROROROROROROR" << __FILE__ << " " << __LINE__ << "\n";
        }
    }
}
