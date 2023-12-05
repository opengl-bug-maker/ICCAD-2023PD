#include "quad_sequence_t.h"
#include <iostream>
#include "process/functional/ILP_solver_t.h"
#include "extended_module_t.h"
#include "utilities/vec2d_t.h"
const double quad_sequence_t::inf = 10;
quad_sequence_t::quad_sequence_t(){
    this->QS.resize(4);
    this->sequence_n = 3;

    QS = {   
        // //0
        // {0, 1, 2},
        // {0, 1, 2},
        // {1, 2, 0},
        // {2, 1, 0}

        //1
        {0, 1, 2}, //1
        {2, 1, 0}, //0
        {1, 0, 2}, //0
        {0, 2, 1}  //1

        // //2
        // {0, 1, 2},
        // {1, 2, 0},
        // {1, 0, 2},
        // {0, 2, 1}

        // //3
        // {0, 1, 2},
        // {1, 2, 0},
        // {0, 2, 1},
        // {1, 0, 2}

        // //4
        // {1, 0, 2},
        // {0, 1, 2},
        // {0, 2, 1},
        // {1, 2, 0}

        // //5
        // {1, 0, 2},
        // {0, 1, 2},
        // {1, 0, 2},
        // {1, 2, 0}
    };
    vec2d_t f(10, 10), g(1, 1);
    
    this->modules_wh = {f, f, f};
    this->modules_ex_wh = {g, g, g};
    //add some connections
    // connections.push_back({0,1,1});
    // connections.push_back({1,2,1});
    // this->connections_n = connections.size();

}

// quad_sequence_t::quad_sequence_t(sequence_pair_t SP){
//     for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
//         this->modules_wh[i] = SP.modules_wh[i];
//         this->modules_ex_wh[i] =  {SP.modules_wh[i].get_x()*0.1, SP.modules_wh[i].get_y()*0.1};
//     }

//     for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
//         for(int j = 0; j<sequence_pair_t::sequence_n; ++j){
//             if(i==j){continue;}
            
//             int _case = -1;
//             vec2d_t pos_i = SP.modules_positions[i];
//             vec2d_t pos_j = SP.modules_positions[j];
//             if(pos_i.get_x()<=pos_j.get_x() && pos_i.get_y()<=pos_j.get_y()){
//                 _case = 1;
//             }
//             if(pos_i.get_x()>=pos_j.get_x() && pos_i.get_y()<=pos_j.get_y()){
//                 _case = 5;
//             }

//             if(_case==1){
                
//             }
//         }

//     }
// }

void quad_sequence_t::to_polygon(){
    build_constraint_graph();

    for(auto& net:sequence_pair_t::connections){
        connections.push_back({net.nodes[0], net.nodes[1], net.w});
    }
    connections_n = this->connections.size();
    //build_constraint_graph_from_SP();
    
    constraint_n = 5000;
    constraint_i = 1; //constraint_counter
    variable_n = 16*sequence_n + 4*connections_n;

    x_modules_offsets = y_modules_offsets = x_t_modules_offsets = y_l_modules_offsets = x_b_modules_offsets = y_r_modules_offsets = vector<int>(sequence_n);
    x_l_nets_offsets = x_r_nets_offsets = y_t_nets_offsets = y_b_nets_offsets = vector<int>(connections_n);
    // [x0,x1,x2,x3, xt0, xt1,xb0, xb1, y0,y1,y2,y3, yl0, yl1,yr0, yr1]
    for(int i = 0; i<sequence_n; ++i){
        x_modules_offsets[i] = 16*i+1;
        x_t_modules_offsets[i] = 16*i+5;
        x_b_modules_offsets[i] = 16*i+7;
        y_modules_offsets[i] = 16*i+9;
        y_l_modules_offsets[i] = 16*i+13;
        y_r_modules_offsets[i] = 16*i+15;
    }
    //[x_e_l, x_e_r, y_e_t, y_e_b]
    for(int i = 0; i<connections_n; ++i){
        x_l_nets_offsets[i] = 1 + 16*sequence_n + 4*i + 0;
        x_r_nets_offsets[i] = 1 + 16*sequence_n + 4*i + 1;
        y_t_nets_offsets[i] = 1 + 16*sequence_n + 4*i + 2;
        y_b_nets_offsets[i] = 1 + 16*sequence_n + 4*i + 3;
    }
    ILP_solver = ILP_solver_t();
    ILP_solver.init("ILP_solver", constraint_n, variable_n);
    ILP_solver.set_min();


    set_constraints_modules_overlap();
    set_constraints_modules_fixed();
    set_constraints_extended_modules();
    set_constraints_avoid_zero_ex_w_h();
    set_constraints_nets();
    set_variables_modules();
    set_variables_nets();
    set_coef();
    ILP_solver.set_obj_coef(coef);
    ILP_solver.load();
    ILP_result = ILP_solver.solve(true);
    //ILP_result = ILP_solver.solve(false);
    cout<< "Is feasible: "<<ILP_result.legal<<endl;
    cout<< "ZZ: "<<ILP_result.z<<endl;

    vector<double> ILP_result_var = ILP_result.var_values;
    for(int i = 0; i<connections.size(); ++i){
        cout<< ILP_result_var[x_l_nets_offsets[i]]<<" "<<ILP_result_var[x_r_nets_offsets[i]]<<" "<<ILP_result_var[y_b_nets_offsets[i]]<<" "<<ILP_result_var[y_t_nets_offsets[i]]<<" "<<endl;
    }
    // for(int i = 0; i<this->sequence_n; ++i){
    //     cout<<"i "<<i<<":"<<endl;
    //     cout<<"x: ";
    //     for(int j = 0; j<4; ++j){
    //         cout<< ILP_result_var[x_modules_offsets[i]+j]<<" ";
    //     }
    //     cout<<endl;
    //     cout<<"y: ";
    //     for(int j = 0; j<4; ++j){
    //         cout<< ILP_result_var[y_modules_offsets[i]+j]<<" ";
    //     }
    //     cout<<endl;
    //     cout<<"x_t : "<< ILP_result_var[x_t_modules_offsets[i]]<<" "<<ILP_result_var[x_t_modules_offsets[i]+1]<<endl;
    //     cout<<"x_b : "<< ILP_result_var[x_b_modules_offsets[i]]<<" "<<ILP_result_var[x_b_modules_offsets[i]+1]<<endl;
    //     cout<<"y_l : "<< ILP_result_var[y_l_modules_offsets[i]]<<" "<<ILP_result_var[y_l_modules_offsets[i]+1]<<endl;
    //     cout<<"y_r : "<< ILP_result_var[y_r_modules_offsets[i]]<<" "<<ILP_result_var[y_r_modules_offsets[i]+1]<<endl;
    // }
    vector<pair<vector<vec2d_t>, string>> result;
    for(int i = 0; i<sequence_n; ++i){
        extended_module_t exm(&ILP_result_var[1+i*16]);
        result.push_back({exm.to_vec2d(), "module_"+to_string(i)});
    }
    this->modules_res = result;
}



void quad_sequence_t::to_polygon(sequence_pair_t SP){
    //build_constraint_graph();

    for(auto& net:sequence_pair_t::connections){
        connections.push_back({net.nodes[0], net.nodes[1], net.w});
    }
    connections_n = this->connections.size();
    build_constraint_graph_from_SP(SP);
    
    constraint_n = 5000;
    constraint_i = 1; //constraint_counter
    variable_n = 16*sequence_n + 4*connections_n;

    x_modules_offsets = y_modules_offsets = x_t_modules_offsets = y_l_modules_offsets = x_b_modules_offsets = y_r_modules_offsets = vector<int>(sequence_n);
    x_l_nets_offsets = x_r_nets_offsets = y_t_nets_offsets = y_b_nets_offsets = vector<int>(connections_n);
    // [x0,x1,x2,x3, xt0, xt1,xb0, xb1, y0,y1,y2,y3, yl0, yl1,yr0, yr1]
    for(int i = 0; i<sequence_n; ++i){
        x_modules_offsets[i] = 16*i+1;
        x_t_modules_offsets[i] = 16*i+5;
        x_b_modules_offsets[i] = 16*i+7;
        y_modules_offsets[i] = 16*i+9;
        y_l_modules_offsets[i] = 16*i+13;
        y_r_modules_offsets[i] = 16*i+15;
    }
    //[x_e_l, x_e_r, y_e_t, y_e_b]
    for(int i = 0; i<connections_n; ++i){
        x_l_nets_offsets[i] = 1 + 16*sequence_n + 4*i + 0;
        x_r_nets_offsets[i] = 1 + 16*sequence_n + 4*i + 1;
        y_t_nets_offsets[i] = 1 + 16*sequence_n + 4*i + 2;
        y_b_nets_offsets[i] = 1 + 16*sequence_n + 4*i + 3;
    }
    ILP_solver = ILP_solver_t();
    ILP_solver.init("ILP_solver", constraint_n, variable_n);
    ILP_solver.set_min();


    set_constraints_modules_overlap();
    set_constraints_modules_fixed();
    set_constraints_extended_modules();
    set_constraints_avoid_zero_ex_w_h();
    set_constraints_nets();
    set_variables_modules();
    set_variables_nets();
    set_coef();
    ILP_solver.set_obj_coef(coef);
    ILP_solver.load();
    ILP_result = ILP_solver.solve(true);
    //ILP_result = ILP_solver.solve(false);
    cout<< "Is feasible: "<<ILP_result.legal<<endl;
    cout<< "ZZ: "<<ILP_result.z<<endl;

    vector<double> ILP_result_var = ILP_result.var_values;
    for(int i = 0; i<connections.size(); ++i){
        cout<< ILP_result_var[x_l_nets_offsets[i]]<<" "<<ILP_result_var[x_r_nets_offsets[i]]<<" "<<ILP_result_var[y_b_nets_offsets[i]]<<" "<<ILP_result_var[y_t_nets_offsets[i]]<<" "<<endl;
    }
    // for(int i = 0; i<this->sequence_n; ++i){
    //     cout<<"i "<<i<<":"<<endl;
    //     cout<<"x: ";
    //     for(int j = 0; j<4; ++j){
    //         cout<< ILP_result_var[x_modules_offsets[i]+j]<<" ";
    //     }
    //     cout<<endl;
    //     cout<<"y: ";
    //     for(int j = 0; j<4; ++j){
    //         cout<< ILP_result_var[y_modules_offsets[i]+j]<<" ";
    //     }
    //     cout<<endl;
    //     cout<<"x_t : "<< ILP_result_var[x_t_modules_offsets[i]]<<" "<<ILP_result_var[x_t_modules_offsets[i]+1]<<endl;
    //     cout<<"x_b : "<< ILP_result_var[x_b_modules_offsets[i]]<<" "<<ILP_result_var[x_b_modules_offsets[i]+1]<<endl;
    //     cout<<"y_l : "<< ILP_result_var[y_l_modules_offsets[i]]<<" "<<ILP_result_var[y_l_modules_offsets[i]+1]<<endl;
    //     cout<<"y_r : "<< ILP_result_var[y_r_modules_offsets[i]]<<" "<<ILP_result_var[y_r_modules_offsets[i]+1]<<endl;
    // }
    vector<pair<vector<vec2d_t>, string>> result;
    for(int i = 0; i<sequence_n; ++i){
        extended_module_t exm(&ILP_result_var[1+i*16]);
        result.push_back({exm.to_vec2d(), "module_"+to_string(i)});
    }
    this->modules_res = result;
}

void quad_sequence_t::build_constraint_graph_from_SP(sequence_pair_t SP){
    this->G.clear();
    this->G.resize(8);
    for(int i = 0; i<8; ++i){
        G[i].resize(sequence_n);
    }


    // vector<vector<int>> map = vector<vector<int>>(4, vector<int>(sequence_n)); // map[i][j] -> index of sequence numeber j  in sequence i 

    // for(int i = 0; i<4; ++i){
    //     for(int j = 0; j<sequence_n; ++j){
    //         map[i][QS[i][j]] = j;
    //     }
    // }
    // for(int i = 0; i<4;  ++i){
    //     for(int j = 0 ;j<sequence_n; ++j){
    //         cout<< map[i][j]<<" ";
    //     }
    //     cout<<endl;
    // }
    vector<int> v_map(sequence_n), h_map(sequence_n);
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        h_map[SP.h_sequence[i]] = i;
        v_map[SP.v_sequence[i]] = i;
    }
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        this->modules_wh[i] = SP.modules_wh[i];
        this->modules_ex_wh[i] =  {SP.modules_wh[i].get_x()*0.1, SP.modules_wh[i].get_y()*0.1};
    }
    // for(int i = 0; i< this->sequence_n; ++i){
    //     cout<< this->modules_wh[i]<<" "<<this->modules_ex_wh[i]<<endl;
    // }
    for(int i = 0; i<sequence_n; ++i){
        cout<< SP.modules_positions[i]<<endl;
    }
    for(int i = 0; i<sequence_n; ++i){
        for(int j = 0; j<sequence_n; ++j){
            if(i==j){continue;}
            
            bool is_7_or_0 = false;
            bool is_4_or_3 = false;
            vec2d_t pos_i = SP.modules_positions[i];
            vec2d_t pos_j = SP.modules_positions[j];
            bool h = 0, v = 0;
            if(h_map[i]<h_map[j]){h = 1;}
            if(v_map[i]<v_map[j]){v = 1;}

            if(h&&v){
                is_7_or_0 = true;
            }
            if((h)&&(!v)){
                is_4_or_3 = true;
            }
            if(is_7_or_0){
                if(pos_j.get_y()<pos_i.get_y()){
                    G[7][j].push_back(i);
                    
                }
                else{
                    G[0][j].push_back(i); //wtf
                    //G[1][j].push_back(i); //wtf
                }
            }
            if(is_4_or_3){
                if(pos_j.get_x()<pos_i.get_x()){
                    
                    G[3][j].push_back(i);
                }
                else{
                    
                    G[4][j].push_back(i);
                }
            }
                
        }

    }




    // for(int i = 0; i<sequence_n; ++i){
    //     for(int j = 0; j<sequence_n; ++j){
    //         if(i==j){continue;}
    //         // relation between i and j
    //         int before[4];
    //         int v = 0; //a decimal way to represent before[4]
    //         for(int k = 0; k<4; ++k){
    //             before[k] = map[k][i]<map[k][j];
    //         }
    //         for(int k = 0; k<4; ++k){
    //             v+=static_cast<int>(before[k])*(pow(2, k));
    //         }
    //         for(int k = 0; k<8; ++k){
    //             if(v==k){
    //                 G[k][i].push_back(j);
    //             }
    //         }

    //     }
    // }

    for(int i = 0; i<8; ++i){
        cout<<"i : "<<i<<endl;
        for(int j = 0; j<sequence_n; ++j){
            cout<<j<<": ";
            for(auto& k:G[i][j]){
                cout<<k<<" ";
            }
            cout<<endl;
        }
        cout<<endl;
    }
    
}

void quad_sequence_t::build_constraint_graph(){
    this->G.resize(8);
    this->sequence_n = sequence_pair_t::sequence_n;
    vector<vector<int>> map = vector<vector<int>>(4, vector<int>(sequence_n)); // map[i][j] -> index of sequence numeber j  in sequence i 
    for(int i = 0; i<8; ++i){
        G[i].resize(sequence_n);
    }
    for(int i = 0; i<4; ++i){
        for(int j = 0; j<sequence_n; ++j){
            map[i][QS[i][j]] = j;
        }
    }
    // for(int i = 0; i<4;  ++i){
    //     for(int j = 0 ;j<sequence_n; ++j){
    //         cout<< map[i][j]<<" ";
    //     }
    //     cout<<endl;
    // }
    for(int i = 0; i<sequence_n; ++i){
        for(int j = 0; j<sequence_n; ++j){
            if(i==j){continue;}
            // relation between i and j
            int before[4];
            int v = 0; //a decimal way to represent before[4]
            for(int k = 0; k<4; ++k){
                before[k] = map[k][i]<map[k][j];
            }
            for(int k = 0; k<4; ++k){
                v+=static_cast<int>(before[k])*(pow(2, k));
            }
            for(int k = 0; k<8; ++k){
                if(v==k){
                    G[k][i].push_back(j);
                }
            }

        }
    }

    for(int i = 0; i<8; ++i){
        cout<<"i : "<<i<<endl;
        for(int j = 0; j<sequence_n; ++j){
            cout<<j<<": ";
            for(auto& k:G[i][j]){
                cout<<k<<" ";
            }
            cout<<endl;
        }
        cout<<endl;
    }
    
}

void quad_sequence_t::set_constraints_modules_overlap(){
    for(int i = 0; i<this->G[0].size(); ++i){
        for(int j = 0; j<this->G[0][i].size(); ++j){
            int v = i, u = G[0][i][j]; //wtf
            ILP_solver.set_constraint_upb(constraint_i, 2, {x_modules_offsets[u]+2, x_modules_offsets[v]+0}, {1, -1}, "ux2 - vx0", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {x_modules_offsets[u]+3, x_modules_offsets[v]+1}, {1, -1}, "ux3 - vx1", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {y_r_modules_offsets[u]+1, y_l_modules_offsets[v]+0}, {1, -1}, "ury1 - vly0", 0);
            constraint_i++;
        }

    }
    for(int i = 0; i<this->G[1].size(); ++i){
        for(int j = 0; j<this->G[1][i].size(); ++j){
            int u = i, v = G[1][i][j];
            ILP_solver.set_constraint_upb(constraint_i, 2, {x_modules_offsets[u]+2, x_modules_offsets[v]+1}, {1, -1}, "ux2 - vx1", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {y_modules_offsets[u]+2, y_l_modules_offsets[v]+0}, {1, -1}, "uy2 - vly0", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {y_r_modules_offsets[u]+1, y_modules_offsets[v]+1}, {1, -1}, "ury1 - vy1", 0);
            constraint_i++;
        }

    }
    for(int i = 0; i<this->G[2].size(); ++i){
        for(int j = 0; j<this->G[2][i].size(); ++j){
            int u = i, v = G[2][i][j];
            ILP_solver.set_constraint_upb(constraint_i, 2, {y_modules_offsets[u]+2, y_modules_offsets[v]+1}, {1, -1}, "uy2 - vy1", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {x_t_modules_offsets[u]+1, x_modules_offsets[v]+1}, {1, -1}, "utx1 - vx1", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {x_modules_offsets[u]+2, x_b_modules_offsets[v]+0}, {1, -1}, "ux2 - vbx0", 0);
            constraint_i++;
        }

    }
    for(int i = 0; i<this->G[3].size(); ++i){
        for(int j = 0; j<this->G[3][i].size(); ++j){
            int u = i, v = G[3][i][j];
            ILP_solver.set_constraint_upb(constraint_i, 2, {y_modules_offsets[u]+2, y_modules_offsets[v]+0}, {1, -1}, "uy2 - vy0", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {y_modules_offsets[u]+3, y_modules_offsets[v]+1}, {1, -1}, "uy3 - vy1", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {x_t_modules_offsets[u]+1, x_b_modules_offsets[v]+0}, {1, -1}, "utx1 - ubx0", 0);
            constraint_i++;
        }

    }
    for(int i = 0; i<this->G[4].size(); ++i){
        for(int j = 0; j<this->G[4][i].size(); ++j){
            int u = i, v = G[4][i][j];
            ILP_solver.set_constraint_upb(constraint_i, 2, {y_modules_offsets[u]+2, y_modules_offsets[v]+0}, {1, -1}, "uy2 - vy0", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {y_modules_offsets[u]+3, y_modules_offsets[v]+1}, {1, -1}, "uy3 - vy1", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {x_b_modules_offsets[v]+1, x_t_modules_offsets[u]+0}, {1, -1}, "vbx1 - utx0", 0);
            constraint_i++;
        }

    }
    for(int i = 0; i<this->G[5].size(); ++i){
        for(int j = 0; j<this->G[5][i].size(); ++j){
            int u = i, v = G[5][i][j];
            ILP_solver.set_constraint_upb(constraint_i, 2, {y_modules_offsets[u]+2, y_modules_offsets[v]+1}, {1, -1}, "uy2 - vy1", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {x_modules_offsets[v]+2, x_t_modules_offsets[u]+0}, {1, -1}, "vx2 - utx0", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {x_b_modules_offsets[v]+1, x_modules_offsets[u]+1}, {1, -1}, "vbx1 - ux1", 0);
            constraint_i++;
        }
    }
    for(int i = 0; i<this->G[6].size(); ++i){
        for(int j = 0; j<this->G[6][i].size(); ++j){
            int u = i, v = G[6][i][j];
            ILP_solver.set_constraint_upb(constraint_i, 2, {x_modules_offsets[v]+2, x_modules_offsets[u]+1}, {1, -1}, "vx2 - ux1", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {y_modules_offsets[u]+2, y_r_modules_offsets[v]+0}, {1, -1}, "uy2 - vry0", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {y_l_modules_offsets[u]+1, y_modules_offsets[v]+1}, {1, -1}, "uly1 - vy1", 0);
            constraint_i++;
        }

    }
    for(int i = 0; i<this->G[7].size(); ++i){
        for(int j = 0; j<this->G[7][i].size(); ++j){
            int u = i, v = G[7][i][j];
            ILP_solver.set_constraint_upb(constraint_i, 2, {x_modules_offsets[v]+2, x_modules_offsets[u]+0}, {1, -1}, "vx2 - ux0", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {x_modules_offsets[v]+3, x_modules_offsets[u]+1}, {1, -1}, "vx3 - ux1", 0);
            constraint_i++;
            ILP_solver.set_constraint_upb(constraint_i, 2, {y_l_modules_offsets[u]+1, y_r_modules_offsets[v]+0}, {1, -1}, "uly1 - vry0", 0);
            constraint_i++;
        }
    }
}

void quad_sequence_t::set_constraints_modules_fixed(){

    for(int i = 0; i<this->sequence_n; ++i){
        if(this->seq_is_fix[i]){
            vec2d_t pos = chip_t::get_modules()[i]->get_left_lower();
            vec2d_t wh = chip_t::get_modules()[i]->get_size();
            ILP_solver.set_constraint_fx(constraint_i, 1, {1+x_modules_offsets[i]}, {1}, "x1 = x_pos", (int)pos.get_x());
            constraint_i++;
            ILP_solver.set_constraint_fx(constraint_i, 1, {2+x_modules_offsets[i]}, {1}, "x2 = x_pos+w", (int)(pos.get_x()+wh.get_x()));
            constraint_i++;

            ILP_solver.set_constraint_fx(constraint_i, 1, {1+y_modules_offsets[i]}, {1}, "y1 = y_pos", (int)pos.get_y());
            constraint_i++;
            ILP_solver.set_constraint_fx(constraint_i, 1, {2+y_modules_offsets[i]}, {1}, "y2 = y_pos+h", (int)(pos.get_y()+wh.get_y()));
            constraint_i++;


            ILP_solver.set_constraint_fx(constraint_i, 2, {x_modules_offsets[i]+1, x_modules_offsets[i]+0}, {1, -1}, "x1 - x0  = 0", 0);
            constraint_i++;
            ILP_solver.set_constraint_fx(constraint_i, 2, {x_modules_offsets[i]+3, x_modules_offsets[i]+2}, {1, -1}, "x3 - x2  = 0", 0);
            constraint_i++;


            ILP_solver.set_constraint_fx(constraint_i, 2, {y_modules_offsets[i]+1, y_modules_offsets[i]+0}, {1, -1}, "y1 - y0  = 0", 0);
            constraint_i++;
            ILP_solver.set_constraint_fx(constraint_i, 2, {y_modules_offsets[i]+3, y_modules_offsets[i]+2}, {1, -1}, "y3 - y2  = 0", 0);
            constraint_i++;



            ILP_solver.set_constraint_fx(constraint_i, 2, {x_t_modules_offsets[i]+1, x_t_modules_offsets[i]+0}, {1, -1}, "xt1 - xt0  = 0", 0);
            constraint_i++;
            ILP_solver.set_constraint_fx(constraint_i, 2, {x_b_modules_offsets[i]+1, x_b_modules_offsets[i]+0}, {1, -1}, "xb1 - xb0  = 0", 0);
            constraint_i++;

            ILP_solver.set_constraint_fx(constraint_i, 2, {y_l_modules_offsets[i]+1, y_l_modules_offsets[i]+0}, {1, -1}, "yl1 - yl0  = 0", 0);
            constraint_i++;
            ILP_solver.set_constraint_fx(constraint_i, 2, {y_r_modules_offsets[i]+1, y_r_modules_offsets[i]+0}, {1, -1}, "yr1 - yr0  = 0", 0);
            constraint_i++;
        }
    }
}

void quad_sequence_t::set_constraints_extended_modules(){
    //x0 < x1 < x2 < x3
    //x1 < xt0 < xt1 < x2
    //x1 < xb0 < xb1 < x2
    for(int i = 0; i<sequence_n; ++i){
        //x0 < x1 < x2 < x3
        ILP_solver.set_constraint_upb(constraint_i, 2, {x_modules_offsets[i]+0, x_modules_offsets[i]+1}, {1, -1}, "x0 - x1", 0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {x_modules_offsets[i]+1, x_modules_offsets[i]+2}, {1, -1}, "x1 - x2", 0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {x_modules_offsets[i]+2, x_modules_offsets[i]+3}, {1, -1}, "x2 - x3", 0);
        constraint_i++;

        //x1 < xt0 < xt1 < x2
        ILP_solver.set_constraint_upb(constraint_i, 2, {x_modules_offsets[i]+1, x_t_modules_offsets[i]+0}, {1, -1}, "x1 - xt0", 0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {x_t_modules_offsets[i]+0, x_t_modules_offsets[i]+1}, {1, -1}, "xt0 - xt1", 0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {x_t_modules_offsets[i]+1, x_modules_offsets[i]+2}, {1, -1}, "xt1 - x2", 0);
        constraint_i++;


        //x1 < xb0 < xb1 < x2
        ILP_solver.set_constraint_upb(constraint_i, 2, {x_modules_offsets[i]+1, x_b_modules_offsets[i]+0}, {1, -1}, "x1 - xb0", 0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {x_b_modules_offsets[i]+0, x_b_modules_offsets[i]+1}, {1, -1}, "xb0 - xb1", 0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {x_b_modules_offsets[i]+1, x_modules_offsets[i]+2}, {1, -1}, "xb1 - x2", 0);
        constraint_i++;

    }

    //y0 < y1 < y2 < y3
    //y1 < yl0 < yl1 < y2
    //y1 < yr0 < yr1 < y2
    for(int i = 0; i<sequence_n; ++i){
        //y0 < y1 < y2 < y3
        ILP_solver.set_constraint_upb(constraint_i, 2, {y_modules_offsets[i]+0, y_modules_offsets[i]+1}, {1, -1}, "y0 - y1", 0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {y_modules_offsets[i]+1, y_modules_offsets[i]+2}, {1, -1}, "y1 - y2", 0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {y_modules_offsets[i]+2, y_modules_offsets[i]+3}, {1, -1}, "y2 - y3", 0);
        constraint_i++;


        //y1 < yl0 < yl1 < y2
        ILP_solver.set_constraint_upb(constraint_i, 2, {y_modules_offsets[i]+1, y_l_modules_offsets[i]+0}, {1, -1}, "y1 - yl0", 0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {y_l_modules_offsets[i]+0, y_l_modules_offsets[i]+1}, {1, -1}, "yl0 - yl1", 0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {y_l_modules_offsets[i]+1, y_modules_offsets[i]+2}, {1, -1}, "yl1 - y2", 0);
        constraint_i++;


        //y1 < yr0 < yr1 < y2
        ILP_solver.set_constraint_upb(constraint_i, 2, {y_modules_offsets[i]+1, y_r_modules_offsets[i]+0}, {1, -1}, "y1 - yr0", 0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {y_r_modules_offsets[i]+0, y_r_modules_offsets[i]+1}, {1, -1}, "yr0 - yr1", 0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {y_r_modules_offsets[i]+1, y_modules_offsets[i]+2}, {1, -1}, "yr1 - y2", 0);
        constraint_i++;

    }

    
    // size
    // x1 - x0 = lw
    // x2 - x1 = w
    // x3 - x2 = rw 
    for(int i = 0; i<sequence_n; ++i){
        ILP_solver.set_constraint_upb(constraint_i, 2, {x_modules_offsets[i]+1, x_modules_offsets[i]+0}, {1, -1}, "x1 - x0", modules_ex_wh[i].get_x());
        constraint_i++;
        ILP_solver.set_constraint_fx(constraint_i, 2, {x_modules_offsets[i]+2, x_modules_offsets[i]+1}, {1, -1}, "x2 - x1", modules_wh[i].get_x());
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {x_modules_offsets[i]+3, x_modules_offsets[i]+2}, {1, -1}, "x3 - x2", modules_ex_wh[i].get_x());
        constraint_i++;
    }

    // y1 - y0 = bh
    // y2 - y1 = h
    // y3 - y2 = th
    for(int i = 0; i<sequence_n; ++i){
        ILP_solver.set_constraint_upb(constraint_i, 2, {y_modules_offsets[i]+1, y_modules_offsets[i]+0}, {1, -1}, "y1 - y0", modules_ex_wh[i].get_y());
        constraint_i++;
        ILP_solver.set_constraint_fx(constraint_i, 2, {y_modules_offsets[i]+2, y_modules_offsets[i]+1}, {1, -1}, "y2 - y1", modules_wh[i].get_y());
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 2, {y_modules_offsets[i]+3, y_modules_offsets[i]+2}, {1, -1}, "y3 - y2", modules_ex_wh[i].get_y());
        constraint_i++;
    }
}

void quad_sequence_t::set_variables_modules(){


    string var_name;
    for(int i = 0;i<sequence_n; ++i){
        var_name = std::to_string(i) + "_x0";
        glp_set_col_name(ILP_solver.ILP, x_modules_offsets[i]+0, var_name.c_str());
        ILP_solver.set_variable_double_range(x_modules_offsets[i]+0, 0.0,board_width);

        var_name = std::to_string(i) + "_x1";
        glp_set_col_name(ILP_solver.ILP, x_modules_offsets[i]+1, var_name.c_str());
        ILP_solver.set_variable_double_range(x_modules_offsets[i]+1, 0.0,board_width);
        
        var_name = std::to_string(i) + "_x2";
        glp_set_col_name(ILP_solver.ILP, x_modules_offsets[i]+2, var_name.c_str());
        ILP_solver.set_variable_double_range(x_modules_offsets[i]+2, 0.0,board_width);

        var_name = std::to_string(i) + "_x3";
        glp_set_col_name(ILP_solver.ILP, x_modules_offsets[i]+3, var_name.c_str());
        ILP_solver.set_variable_double_range(x_modules_offsets[i]+3 , 0.0,board_width);
        
        
        var_name = std::to_string(i) + "_xt0";
        glp_set_col_name(ILP_solver.ILP, x_t_modules_offsets[i]+0, var_name.c_str());
        ILP_solver.set_variable_double_range_int(x_t_modules_offsets[i]+0, 0.0,board_width);
        //ILP_solver.set_variable_double_range(x_t_modules_offsets[i]+0, 0.0,board_width);

        var_name = std::to_string(i) + "_xt1";
        glp_set_col_name(ILP_solver.ILP, x_t_modules_offsets[i]+1, var_name.c_str());
        ILP_solver.set_variable_double_range_int(x_t_modules_offsets[i]+1, 0.0,board_width);
        //ILP_solver.set_variable_double_range(x_t_modules_offsets[i]+1, 0.0,board_width);


        var_name = std::to_string(i) + "_xb0";
        glp_set_col_name(ILP_solver.ILP, x_b_modules_offsets[i]+0, var_name.c_str());        
        //ILP_solver.set_variable_double_range(x_b_modules_offsets[i]+0, 0.0,board_width);
        ILP_solver.set_variable_double_range_int(x_b_modules_offsets[i]+0, 0.0,board_width);

        var_name = std::to_string(i) + "_xb1";
        glp_set_col_name(ILP_solver.ILP, x_b_modules_offsets[i]+1, var_name.c_str());
        //ILP_solver.set_variable_double_range(x_b_modules_offsets[i]+1, 0.0,board_width);
        ILP_solver.set_variable_double_range_int(x_b_modules_offsets[i]+1, 0.0,board_width);



        var_name = std::to_string(i) + "_y0";
        glp_set_col_name(ILP_solver.ILP, y_modules_offsets[i]+0, var_name.c_str());
        ILP_solver.set_variable_double_range(y_modules_offsets[i]+0, 0.0,board_height);

        var_name = std::to_string(i) + "_y1";
        glp_set_col_name(ILP_solver.ILP, y_modules_offsets[i]+1, var_name.c_str());
        ILP_solver.set_variable_double_range(y_modules_offsets[i]+1, 0.0,board_height);
        var_name = std::to_string(i) + "_y2";
        glp_set_col_name(ILP_solver.ILP, y_modules_offsets[i]+2, var_name.c_str());
        ILP_solver.set_variable_double_range(y_modules_offsets[i]+2, 0.0,board_height);

        var_name = std::to_string(i) + "_y3";
        glp_set_col_name(ILP_solver.ILP, y_modules_offsets[i]+3, var_name.c_str());
        ILP_solver.set_variable_double_range(y_modules_offsets[i]+3, 0.0,board_height);


        var_name = std::to_string(i) + "_yr0";
        glp_set_col_name(ILP_solver.ILP, y_r_modules_offsets[i]+0, var_name.c_str());
        //ILP_solver.set_variable_double_range(y_r_modules_offsets[i]+0, 0.0,board_height);
         ILP_solver.set_variable_double_range_int(y_r_modules_offsets[i]+0, 0.0,board_height);

        var_name = std::to_string(i) + "_yr1";
        glp_set_col_name(ILP_solver.ILP, y_r_modules_offsets[i]+1, var_name.c_str());
        //ILP_solver.set_variable_double_range(y_r_modules_offsets[i]+1, 0.0,board_height);
        ILP_solver.set_variable_double_range_int(y_r_modules_offsets[i]+1, 0.0,board_height);

    
        var_name = std::to_string(i) + "_yl0";
        glp_set_col_name(ILP_solver.ILP, y_l_modules_offsets[i]+0, var_name.c_str());
        ILP_solver.set_variable_double_range_int(y_l_modules_offsets[i]+0, 0.0,board_height);
        //ILP_solver.set_variable_double_range(y_l_modules_offsets[i]+0, 0.0,board_height);

        var_name = std::to_string(i) + "_yl1";
        glp_set_col_name(ILP_solver.ILP, y_l_modules_offsets[i]+1, var_name.c_str());
        //ILP_solver.set_variable_double_range(y_l_modules_offsets[i]+1, 0.0,board_height);
        ILP_solver.set_variable_double_range_int(y_l_modules_offsets[i]+1, 0.0,board_height);
    }
}

void quad_sequence_t::set_variables_nets(){
    for(int i = 0; i<connections.size(); ++i){
        string var_name1 = "x_e_l"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, x_l_nets_offsets[i], var_name1.c_str());
        ILP_solver.set_variable_double_range(x_l_nets_offsets[i], 0.0, board_width);

        string var_name2 = "x_e_r"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, x_r_nets_offsets[i], var_name2.c_str());
        ILP_solver.set_variable_double_range(x_r_nets_offsets[i], 0.0, board_width);

        string var_name3 = "y_e_l"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, y_t_nets_offsets[i], var_name3.c_str());
        ILP_solver.set_variable_double_range(y_t_nets_offsets[i], 0.0, board_height);

        string var_name4 = "y_e_l"+ std::to_string(i);
        glp_set_col_name(ILP_solver.ILP, y_b_nets_offsets[i], var_name4.c_str());
        ILP_solver.set_variable_double_range(y_b_nets_offsets[i], 0.0, board_height);
    }
}

void quad_sequence_t::set_constraints_nets(){

    for(int i = 0; i<connections.size(); ++i){
        int u = connections[i][0], v = connections[i][1], w = connections[i][2];
        //  (u_x3+u_x0)/2 - x^e_r <=0 
        ILP_solver.set_constraint_upb(constraint_i, 3, {x_modules_offsets[u]+3,x_modules_offsets[u]+0, x_r_nets_offsets[i]}, {0.5,0.5, -1}, "u_x3 - x^e_r <=0", 0);
        constraint_i++;
        // //  x^e_l - u_x0 <=0 
        ILP_solver.set_constraint_upb(constraint_i, 3, {x_l_nets_offsets[i], x_modules_offsets[u]+0, x_modules_offsets[u]+3}, {1, -0.5, -0.5}, "x^e_l - u_x0 <=0", 0);
        constraint_i++;


        // //  (u_x3+u_x0)/2 - x^e_r <=0 
        ILP_solver.set_constraint_upb(constraint_i, 3, {x_modules_offsets[v]+3,x_modules_offsets[v]+0, x_r_nets_offsets[i]}, {0.5,0.5, -1}, "u_x3 - x^e_r <=0", 0);
        constraint_i++;
        //  x^e_l - u_x0 <=0 
        ILP_solver.set_constraint_upb(constraint_i, 3, {x_l_nets_offsets[i], x_modules_offsets[v]+0, x_modules_offsets[v]+3}, {1, -0.5, -0.5}, "x^e_l - u_x0 <=0", 0);
        constraint_i++;



                
        ILP_solver.set_constraint_upb(constraint_i, 3, {y_modules_offsets[u]+3, y_modules_offsets[u]+0, y_t_nets_offsets[i]}, {0.5, 0.5, -1}, "u_y3 - y^e_t <=0", 0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 3, {y_b_nets_offsets[i],y_modules_offsets[u]+3, y_modules_offsets[u]+0}, {1, -0.5, -0.5}, "u^e_b - u_y0 <=0", 0);
        constraint_i++;
        

        ILP_solver.set_constraint_upb(constraint_i, 3, {y_modules_offsets[v]+3, y_modules_offsets[v]+0, y_t_nets_offsets[i]}, {0.5, 0.5, -1}, "u_y3 - y^e_t <=0", 0);
        constraint_i++;
        ILP_solver.set_constraint_upb(constraint_i, 3, {y_b_nets_offsets[i],y_modules_offsets[v]+3, y_modules_offsets[v]+0}, {1, -0.5, -0.5}, "u^e_b - u_y0 <=0", 0);
        constraint_i++;
    }

}

void quad_sequence_t::set_constraints_avoid_zero_ex_w_h(){
    for(int i = 0; i<sequence_n; ++i){
        // //y_3 - y_2 <= inf * (x_t_1 - x_t_0)
        //y_3-y_2 <= 10*(x_t_1 - x_t_0)
        ILP_solver.set_constraint_upb(constraint_i, 4, {x_t_modules_offsets[i]+1, x_t_modules_offsets[i]+0, y_modules_offsets[i]+3, y_modules_offsets[i]+2}, {-inf, inf, 1, -1}, "y_3 - y_2 - inf * (x_t_1 - x_t_0)", 0);
        constraint_i++;

        //y_1 - y_0 <= inf * (x_b_1 - x_b_0)
        // 0 <= y_1 - y_0 <= 1
        ILP_solver.set_constraint_upb(constraint_i, 4, {x_b_modules_offsets[i]+1, x_b_modules_offsets[i]+0, y_modules_offsets[i]+1, y_modules_offsets[i]+0}, {-inf, inf, 1, -1}, "y_1 - y_0 - inf * (x_b_1 - x_b_0)", 0);
        constraint_i++;
        
        //x_1 - x_0 <= inf * (y_l_1 - y_l_0)
        ILP_solver.set_constraint_upb(constraint_i, 4, {y_l_modules_offsets[i]+1, y_l_modules_offsets[i]+0, x_modules_offsets[i]+1, x_modules_offsets[i]+0}, {-inf, inf, 1, -1}, "x_1 - x_0 - inf * (y_l_1 - y_l_0)", 0);
        constraint_i++;

        // //x_3 - x_2 <= inf * (y_r_1 - y_r_0)
        ILP_solver.set_constraint_upb(constraint_i, 4, {y_r_modules_offsets[i]+1, y_r_modules_offsets[i]+0, x_modules_offsets[i]+3, x_modules_offsets[i]+2}, {-inf, inf, 1, -1}, "x_3 - x_2 - inf * (y_r_1 - y_r_0)", 0);
        constraint_i++;
    }
}

void quad_sequence_t::set_coef(){
    this->coef.resize(variable_n+1);
    for(int i = 1; i<=variable_n; ++i){
        this->coef[i] = 0;
    }

    for(int i = 0; i<connections.size(); ++i){
        coef[x_r_nets_offsets[i]] = connections[i][2];
        coef[x_l_nets_offsets[i]] = -connections[i][2];

        coef[y_t_nets_offsets[i]] = connections[i][2];
        coef[y_b_nets_offsets[i]] = -connections[i][2];
    }

    // for(int i = 0; i<sequence_n; ++i){
    //     coef[x_t_modules_offsets[i]+1] = -0.01;
    //     coef[x_t_modules_offsets[i]+0] = 0.01;

    //     coef[x_b_modules_offsets[i]+1] = -0.001;
    //     coef[x_b_modules_offsets[i]+0] = 0.001;

    //     coef[y_r_modules_offsets[i]+1] = -0.01;
    //     coef[y_r_modules_offsets[i]+0] = 0.01;

    //     coef[y_l_modules_offsets[i]+1] = -0.01;
    //     coef[y_l_modules_offsets[i]+0] = 0.01;
    // }
}

void quad_sequence_t::set_sequences(sequence_pair_t SP){
    vector<int> rv_h = SP.h_sequence;
    vector<int> h = SP.h_sequence;
    vector<int> rv_v = SP.v_sequence;
    vector<int> v = SP.v_sequence;
    std::reverse(rv_h.begin(), rv_h.end());
    std::reverse(rv_v.begin(), rv_v.end());
    this->QS[0] = rv_v;
    this->QS[1] = rv_v;
    this->QS[2] = rv_h;
    this->QS[3] = h;

    this->sequence_n = sequence_pair_t::sequence_n;
    this->seq_is_fix = SP.seq_is_fix;
    this->modules_wh.resize(this->sequence_n);
    this->modules_ex_wh.resize(this->sequence_n);
    for(int i = 0; i<  this->sequence_n; ++i){
        this->modules_wh[i] = SP.modules_wh[i];
        this->modules_ex_wh[i] = SP.modules_wh[i]*0.1;
    }
    for(int i = 0; i<this->seq_is_fix.size(); ++i){
        cout<< this->seq_is_fix[i]<<" ";
    }
    cout<<endl;

}
