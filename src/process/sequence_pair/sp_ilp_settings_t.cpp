#include "sp_ilp_settings_t.h"
#include "sequence_pair_t.h"


using namespace std;
sp_ilp_settings_find_position_t::sp_ilp_settings_find_position_t(sequence_pair_t * sp){
    this->sp = sp;
}

void sp_ilp_settings_find_position_t::set_constraints_modules_overlap_h()
{
    for(int i = 0; i<this->sp->constraint_graph_h.size(); ++i){
        int from = this->sp->constraint_graph_h[i].from, to = this->sp->constraint_graph_h[i].to,w = this->sp->constraint_graph_h[i].w;
        string constraint_name = "h_c"+ std::to_string(i);

        if(this->sp->seq_is_fix[from]){
            this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2, {from+this->sp->x_module_offset, to+this->sp->x_module_offset}, {1, -1}, constraint_name, -w);
        }
        else{
            double area = this->sp->modules_area[from];
            vector<double> w(5);
            for(int j = 0; j<5; ++j){
                w[j] = this->sp->soft_area_to_w_h_m_5[from][j].get_x();
            }
            this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 7,
                                  {from+this->sp->x_module_offset, to+this->sp->x_module_offset,
                                           this->sp->shape_types[0] + from,
                                           this->sp->shape_types[1] + from,
                                           this->sp->shape_types[2] + from,
                                           this->sp->shape_types[3] + from,
                                           this->sp->shape_types[4] + from},
                                      {1, -1, w[0],w[1] ,w[2],w[3], w[4]}, constraint_name, -1e-5);
        }
        this->sp->constraint_i++;
    }
}
void sp_ilp_settings_find_position_t::set_constraints_modules_allow_overlap_h(){
    for(int i = 0; i<this->sp->constraint_graph_h.size(); ++i){
        int from = this->sp->constraint_graph_h[i].from, to = this->sp->constraint_graph_h[i].to,w = this->sp->constraint_graph_h[i].w;
        double overlap_var = 0.0;
        double from_area = this->sp->modules_area[from], to_area = this->sp->modules_area[to];
        double ratio = from_area/to_area;
        if( (ratio>=area_overlap_contraint || ratio  <= 1/area_overlap_contraint) && this->sp->is_transitive_h[from][to]==0){
            overlap_var = -1;
        }
        if(sequence_pair_t::seq_is_fix[from] && this->sp->modules_area[from]>=this->sp->modules_area[to]){
            overlap_var = 0;
        }
        if(sequence_pair_t::seq_is_fix[to] && this->sp->modules_area[to]>=this->sp->modules_area[from]){
            overlap_var = 0;
        }
        string constraint_name = "h_c"+ std::to_string(i);
        if(this->sp->seq_is_fix[from]){
            this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 3, {from+this->sp->x_module_offset, to+this->sp->x_module_offset, i+this->sp->x_overlap}, {1, -1, overlap_var}, constraint_name, -w);
            this->sp->constraint_i++;
        }
        else{
            double area = this->sp->modules_area[from];
            vector<double> w(5);
            for(int j = 0; j<5; ++j){
                w[j] = this->sp->soft_area_to_w_h_m_5[from][j].get_x();
            }
            this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 8,
                                  {from+this->sp->x_module_offset, to+this->sp->x_module_offset, i+this->sp->x_overlap,
                                           this->sp->shape_types[0] + from,
                                           this->sp->shape_types[1] + from,
                                           this->sp->shape_types[2] + from,
                                           this->sp->shape_types[3] + from,
                                           this->sp->shape_types[4] + from},
                                      {1, -1, overlap_var, w[0],w[1] ,w[2],w[3], w[4]}, constraint_name, -1e-5);
            this->sp->constraint_i++;
        }
        if( (sequence_pair_t::seq_is_fix[from]==false) && (sequence_pair_t::seq_is_fix[to]==false)){
            vector<double> w_from(5), w_to(5); 
            vector<double> vars = {1, -1};
            for(int j = 0; j<5; ++j){
                w_from[j] = this->sp->soft_area_to_w_h_m_5[from][j].get_x();
                w_to[j] = this->sp->soft_area_to_w_h_m_5[to][j].get_x();
            }
            for(int j = 0; j<5; ++j){
                vars.push_back(w_from[j]);
            }
            for(int j = 0; j<5; ++j){
                vars.push_back(-w_to[j]);
            }
            this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, vars.size(),
                                  {from+this->sp->x_module_offset, to+this->sp->x_module_offset,
                                           this->sp->shape_types[0] + from,
                                           this->sp->shape_types[1] + from,
                                           this->sp->shape_types[2] + from,
                                           this->sp->shape_types[3] + from,
                                           this->sp->shape_types[4] + from,
                                           this->sp->shape_types[0] + to,
                                           this->sp->shape_types[1] + to,
                                           this->sp->shape_types[2] + to,
                                           this->sp->shape_types[3] + to,
                                           this->sp->shape_types[4] + to
                                    },
                                      vars, constraint_name, -1);
            this->sp->constraint_i++;

            this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2,
                {from+this->sp->x_module_offset, to+this->sp->x_module_offset},
                {1, -1},
                constraint_name, -1);
            this->sp->constraint_i++;
        } 
    }
}
void sp_ilp_settings_find_position_t::set_constraints_modules_allow_overlap_v(){
    for(int i = 0; i<this->sp->constraint_graph_v.size(); ++i){
        int from = this->sp->constraint_graph_v[i].from, to = this->sp->constraint_graph_v[i].to, w = this->sp->constraint_graph_v[i].w;
        string constraint_name = "v_c"+ std::to_string(i);
        double overlap_var = 0.0;
        double from_area = this->sp->modules_area[from], to_area = this->sp->modules_area[to];
        double ratio = from_area/to_area;
        if( (ratio>=area_overlap_contraint || ratio  <= 1/area_overlap_contraint) && this->sp->is_transitive_v[from][to]==0){
            overlap_var = -1;
        }
        if(sequence_pair_t::seq_is_fix[from] && this->sp->modules_area[from]>=this->sp->modules_area[to]){
            overlap_var = 0;
        }
        if(sequence_pair_t::seq_is_fix[to] && this->sp->modules_area[to]>=this->sp->modules_area[from]){
            overlap_var = 0;
        }
        if(this->sp->seq_is_fix[from]){
            this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 3, {from+this->sp->y_module_offset, to+this->sp->y_module_offset , i+this->sp->y_overlap}, {1, -1,overlap_var}, constraint_name, -w);
            this->sp->constraint_i++;
        }
        else{
            double area = this->sp->modules_area[from];
            vector<double> h(5);
            for(int j = 0; j<5; ++j){
                h[j] = this->sp->soft_area_to_w_h_m_5[from][j].get_y();    
            }
            this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 8,
                              {from+this->sp->y_module_offset, to+this->sp->y_module_offset, i+this->sp->y_overlap,
                                       this->sp->shape_types[0] + from,
                                       this->sp->shape_types[1] + from,
                                       this->sp->shape_types[2] + from,
                                       this->sp->shape_types[3] + from,
                                       this->sp->shape_types[4] + from},
                               {1, -1,overlap_var,h[0],h[1],h[2],h[3],h[4]}, constraint_name, -1e-5);
            this->sp->constraint_i++;
            if( (sequence_pair_t::seq_is_fix[from]==false) && (sequence_pair_t::seq_is_fix[to]==false)){
                vector<double> w_from(5), w_to(5); 
                vector<double> vars = {1, -1};
                for(int j = 0; j<5; ++j){
                    w_from[j] = this->sp->soft_area_to_w_h_m_5[from][j].get_y();
                    w_to[j] = this->sp->soft_area_to_w_h_m_5[to][j].get_y();
                }
                for(int j = 0; j<5; ++j){
                    vars.push_back(w_from[j]);
                }
                for(int j = 0; j<5; ++j){
                    vars.push_back(-w_to[j]);
                }
                this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, vars.size(),
                                    {from+this->sp->y_module_offset, to+this->sp->y_module_offset,
                                            this->sp->shape_types[0] + from,
                                            this->sp->shape_types[1] + from,
                                            this->sp->shape_types[2] + from,
                                            this->sp->shape_types[3] + from,
                                            this->sp->shape_types[4] + from,
                                            this->sp->shape_types[0] + to,
                                            this->sp->shape_types[1] + to,
                                            this->sp->shape_types[2] + to,
                                            this->sp->shape_types[3] + to,
                                            this->sp->shape_types[4] + to
                                        },
                                        vars, constraint_name, -1);
                this->sp->constraint_i++;

                this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2,
                    {from+this->sp->y_module_offset, to+this->sp->y_module_offset},
                    {1, -1},
                    constraint_name, -1);
                this->sp->constraint_i++;
            } 
        }
        
    }
}
void sp_ilp_settings_find_position_t::set_constraints_modules_overlap_v()
{
    for(int i = 0; i<this->sp->constraint_graph_v.size(); ++i){
        int from = this->sp->constraint_graph_v[i].from, to = this->sp->constraint_graph_v[i].to, w = this->sp->constraint_graph_v[i].w;
        string constraint_name = "v_c"+ std::to_string(i);

        if(this->sp->seq_is_fix[from]){
            this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2, {from+this->sp->y_module_offset, to+this->sp->y_module_offset}, {1, -1}, constraint_name, -w);
        }
        else{
            double area = this->sp->modules_area[from];
            vector<double> h(5);
            for(int j = 0; j<5; ++j){
                h[j] = this->sp->soft_area_to_w_h_m_5[from][j].get_y();    
            }
            this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 7,
                              {from+this->sp->y_module_offset, to+this->sp->y_module_offset,
                                       this->sp->shape_types[0] + from,
                                       this->sp->shape_types[1] + from,
                                       this->sp->shape_types[2] + from,
                                       this->sp->shape_types[3] + from,
                                       this->sp->shape_types[4] + from},
                               {1, -1,h[0],h[1],h[2],h[3], h[4]}, constraint_name, -1e-5);
        }
        this->sp->constraint_i++;
    }
}
void sp_ilp_settings_find_position_t::set_constraints_modules_fixed(){
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        if(sequence_pair_t::seq_is_fix[i]){
            string x_constraint_name = "fix_x_c"+ std::to_string(i);
            string y_constraint_name = "fix_y_c"+ std::to_string(i);
            vec2d_t ll_pos = chip_t::get_modules()[i]->get_left_lower();
            this->sp->ILP_solver.set_constraint_fx(this->sp->constraint_i, 1, {i+this->sp->x_module_offset}, {1}, x_constraint_name, static_cast<int>(ll_pos.get_x()));
            this->sp->constraint_i++;
            this->sp->ILP_solver.set_constraint_fx(this->sp->constraint_i, 1, {i+this->sp->y_module_offset}, {1}, y_constraint_name, static_cast<int>(ll_pos.get_y()));
            this->sp->constraint_i++;
        }
    }
}
void sp_ilp_settings_find_position_t::set_constraints_net_direction(){
    //2 * this->sp->connections
    for(int i = 0; i<this->sp->connections.size(); ++i){
        string x_constraint_name = "x_r_e"+ std::to_string(i);
        string y_constraint_name = "y_r_e"+ std::to_string(i);
        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2, {this->sp->x_edge_offset_l+i, this->sp->x_edge_offset_r+i}, {1, -1}, x_constraint_name, 0.0);
        this->sp->constraint_i++;
        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2, {this->sp->y_edge_offset_l+i, this->sp->y_edge_offset_r+i}, {1, -1}, y_constraint_name, 0.0);
        this->sp->constraint_i++;
    }
}
void sp_ilp_settings_find_position_t::set_constraints_net(){
    //nodes* 4
    for(int i = 0; i<this->sp->connections.size(); ++i){
        for(int j = 0; j<this->sp->connections[i].nodes.size(); ++j){
            int b = this->sp->connections[i].nodes[j];
            double b_x = this->sp->modules_wh[b].get_half_x();
            double b_y = this->sp->modules_wh[b].get_half_y();
            string x_l_constraint_name = "x_e_b_l"+ std::to_string(i)+"_"+std::to_string(j);
            string y_l_constraint_name = "y_e_b_l"+ std::to_string(i)+"_"+std::to_string(j);
            string x_r_constraint_name = "x_e_b_r"+ std::to_string(i)+"_"+std::to_string(j);
            string y_r_constraint_name = "y_e_b_r"+ std::to_string(i)+"_"+std::to_string(j);


            if(sequence_pair_t::seq_is_fix[b]){
                this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2, {this->sp->x_edge_offset_l+i,b+this->sp->x_module_offset}, {1, -1}, x_l_constraint_name, b_x);
                this->sp->constraint_i++;
                
                this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2,{this->sp->y_edge_offset_l+i,b+this->sp->y_module_offset},{1, -1}, y_l_constraint_name, b_y);
                this->sp->constraint_i++;

                this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2,{b+this->sp->x_module_offset,this->sp->x_edge_offset_r+i},{1, -1}, x_r_constraint_name, -b_x);
                this->sp->constraint_i++;
                this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2,{b+this->sp->y_module_offset,this->sp->y_edge_offset_r+i},{1, -1}, y_r_constraint_name, -b_y);
                this->sp->constraint_i++;
            }
            else{
                vector<double> b_w(5), b_h(5);
                for(int k = 0; k<5; ++k){
                    b_w[k] = this->sp->soft_area_to_w_h_m_5[b][k].get_half_x();
                    b_h[k] = this->sp->soft_area_to_w_h_m_5[b][k].get_half_y();
                }
                this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 7,
                                            {this->sp->x_edge_offset_l+i,b+this->sp->x_module_offset,b+this->sp->shape_types[0], b+this->sp->shape_types[1], b+this->sp->shape_types[2], b+this->sp->shape_types[3], b+this->sp->shape_types[4]},
                                            {1, -1,-b_w[0],-b_w[1],-b_w[2],-b_w[3],-b_w[4]}, x_l_constraint_name, 0);
                this->sp->constraint_i++;
                this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 7,
                                            {this->sp->y_edge_offset_l+i,b+this->sp->y_module_offset,b+this->sp->shape_types[0], b+this->sp->shape_types[1], b+this->sp->shape_types[2], b+this->sp->shape_types[3], b+this->sp->shape_types[4]},
                                            {1, -1,-b_h[0],-b_h[1],-b_h[2],-b_h[3],-b_h[4]}, y_l_constraint_name, 0);
                this->sp->constraint_i++;

                this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 7,
                                            {b+this->sp->x_module_offset,this->sp->x_edge_offset_r+i,b+this->sp->shape_types[0], b+this->sp->shape_types[1], b+this->sp->shape_types[2], b+this->sp->shape_types[3], b+this->sp->shape_types[4]},
                                            {1, -1,b_w[0],b_w[1],b_w[2],b_w[3],b_w[4]}, x_r_constraint_name, 0);
                this->sp->constraint_i++;
                this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 7,
                                            {b+this->sp->y_module_offset,this->sp->y_edge_offset_r+i,b+this->sp->shape_types[0], b+this->sp->shape_types[1], b+this->sp->shape_types[2], b+this->sp->shape_types[3], b+this->sp->shape_types[4]},
                                            {1, -1,b_h[0],b_h[1],b_h[2],b_h[3],b_h[4]}, y_r_constraint_name, 0);
                this->sp->constraint_i++;
            }
        }
    }
}
void sp_ilp_settings_find_position_t::set_constraints_ratio_equal_1(){
    for(int i = 0; i<sequence_pair_t::soft_n; ++i){
        string constraint_name1 = "w_h_1"+ std::to_string(i);
        this->sp->ILP_solver.set_constraint_fx(this->sp->constraint_i, 5,
        {this->sp->shape_types[0] + i, this->sp->shape_types[1] + i,this->sp->shape_types[2] + i,this->sp->shape_types[3]+i, this->sp->shape_types[4]+i}, {1, 1,1,1,1}, constraint_name1, 1.001);
        this->sp->constraint_i++;
    }
}
void sp_ilp_settings_find_position_t::set_constraints_boundaries(){
    for(int i = 0; i<sequence_pair_t::soft_n; ++i){
        string  constraint_name1 = "x_bound"+ std::to_string(i);
        string  constraint_name2 = "y_bound"+ std::to_string(i);
        vector<double> w(5), h(5);
        for(int j = 0; j<5; ++j){
            w[j] = this->sp->soft_area_to_w_h_m_5[i][j].get_x();
            h[j] = this->sp->soft_area_to_w_h_m_5[i][j].get_y();
        }


        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 6,
            {this->sp->x_module_offset+i, this->sp->shape_types[0] + i, this->sp->shape_types[1] + i,  this->sp->shape_types[2] + i,this->sp->shape_types[3] + i,this->sp->shape_types[4] + i},
            {1, w[0], w[1],w[2],w[3],w[4]}, constraint_name1, chip_t::get_width());


        this->sp->constraint_i++;
        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 6,
            {this->sp->y_module_offset+i, this->sp->shape_types[0] + i,this->sp->shape_types[1] + i, this->sp->shape_types[2] + i,this->sp->shape_types[3] + i,this->sp->shape_types[4] + i},
            {1, h[0], h[1],h[2],h[3],h[4]}, constraint_name2, chip_t::get_height());
        this->sp->constraint_i++;
    }
}
void sp_ilp_settings_find_position_t::set_variables_modules(){
    for(int i = 0;i<sequence_pair_t::sequence_n; ++i){ //x
        string var_name = "x"+ std::to_string(i);
        glp_set_col_name(this->sp->ILP_solver.ILP, i+this->sp->x_module_offset, var_name.c_str());
        this->sp->ILP_solver.set_variable_double_range(i+this->sp->x_module_offset, 0.0,chip_t::get_width());
        glp_set_col_name(this->sp->ILP_solver.ILP, i+this->sp->y_module_offset, var_name.c_str());
        this->sp->ILP_solver.set_variable_double_range(i+this->sp->y_module_offset, 0.0,chip_t::get_height());

    }
}
void sp_ilp_settings_find_position_t::set_variables_allow_overlap(){
    for(int i = 0; i<this->sp->constraint_graph_h.size(); ++i){
        string var_name = "x_overlap_"+ std::to_string(i);
        glp_set_col_name(this->sp->ILP_solver.ILP, this->sp->x_overlap+i, var_name.c_str());
        this->sp->ILP_solver.set_variable_double_range(this->sp->x_overlap+i, 0.0, chip_t::get_width());
    }    
    for(int i = 0; i<this->sp->constraint_graph_v.size(); ++i){
        string var_name = "y_overlap_"+ std::to_string(i);
        glp_set_col_name(this->sp->ILP_solver.ILP, this->sp->y_overlap+i, var_name.c_str());
        this->sp->ILP_solver.set_variable_double_range(this->sp->y_overlap+i, 0.0, chip_t::get_height());
    }    
}
void sp_ilp_settings_find_position_t::set_constraints_modules_overlap_content(){
    for(int i = 0;i<sequence_pair_t::sequence_n; ++i){ //x

        vector<int> ph, pv;
        for(int j = 0; j<this->sp->constraint_graph_h.size(); ++j){
            int from = this->sp->constraint_graph_h[j].from;
            int to = this->sp->constraint_graph_h[j].to;
            int from_area = this->sp->modules_area[from];
            int to_area = this->sp->modules_area[to];
            if((from==i && from_area>to_area)||(to==i && to_area>from_area)){
                ph.push_back(this->sp->x_overlap+j);
            }
        }
        for(int j = 0; j<this->sp->constraint_graph_v.size(); ++j){
            int from = this->sp->constraint_graph_v[j].from;
            int to = this->sp->constraint_graph_v[j].to;
            int from_area = this->sp->modules_area[from];
            int to_area = this->sp->modules_area[to];
            if((from==i && from_area>to_area)||(to==i && to_area>from_area)){
                pv.push_back(this->sp->y_overlap+j);
            }
        }
        string constraint_name1 = "x_overlap"+ std::to_string(i);
        vector<double> coef_h = vector<double>(ph.size(), 1);
        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, ph.size(), ph, coef_h, 
        constraint_name1, sqrt(this->sp->modules_area[i])*0.5);
        this->sp->constraint_i++;

        string constraint_name2 = "y_overlap"+ std::to_string(i);
        vector<double> coef_v = vector<double>(pv.size(), 1);
        // cout<<i<<": ";
        // for(auto& e:pv){
        //     auto x = this->sp->constraint_graph_v[e-this->sp->y_overlap];
        //     cout<<"{"<<x.from<<", "<<x.to<<" } ";
        // }
        // cout<<endl;
        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, pv.size(), pv, coef_v, 
        constraint_name2, sqrt(this->sp->modules_area[i])*0.5);
        this->sp->constraint_i++;
    }
}
void sp_ilp_settings_find_position_t::set_variables_connections(){
    for(int i = 0; i<this->sp->connections.size(); ++i){
        string var_name1 = "x_e_l"+ std::to_string(i);
        glp_set_col_name(this->sp->ILP_solver.ILP, this->sp->x_edge_offset_l+i, var_name1.c_str());
        this->sp->ILP_solver.set_variable_double_range(this->sp->x_edge_offset_l+i, 0.0, chip_t::get_width());

        string var_name2 = "x_e_r"+ std::to_string(i);
        glp_set_col_name(this->sp->ILP_solver.ILP, this->sp->x_edge_offset_r+i, var_name2.c_str());
        this->sp->ILP_solver.set_variable_double_range(this->sp->x_edge_offset_r+i, 0.0, chip_t::get_width());

        string var_name3 = "y_e_l"+ std::to_string(i);
        glp_set_col_name(this->sp->ILP_solver.ILP, this->sp->y_edge_offset_l+i, var_name3.c_str());
        this->sp->ILP_solver.set_variable_double_range(this->sp->y_edge_offset_l+i, 0.0, chip_t::get_height());

        string var_name4 = "y_e_r"+ std::to_string(i);
        glp_set_col_name(this->sp->ILP_solver.ILP, this->sp->y_edge_offset_r+i, var_name4.c_str());
        this->sp->ILP_solver.set_variable_double_range(this->sp->y_edge_offset_r+i, 0.0, chip_t::get_height());
    }
}
void sp_ilp_settings_find_position_t::set_variables_shapes(){
    for(int i = 0; i<sequence_pair_t::soft_n; ++i){
        for(int j = 0; j<5; ++j){
            string var_name = "shape"+std::to_string(j)+ std::to_string(i);
            glp_set_col_name(this->sp->ILP_solver.ILP, this->sp->shape_types[j] + i, var_name.c_str());
            this->sp->ILP_solver.set_variable_double_range(this->sp->shape_types[j] + i, 0, 1);    
        }
    }
}
void sp_ilp_settings_find_position_t::set_coef(vector<double>& coef){
    for(int i = 0; i<this->sp->connections.size(); ++i){
        coef[i+this->sp->x_edge_offset_r] = this->sp->connections[i].w;
        coef[i+this->sp->x_edge_offset_l] = -this->sp->connections[i].w;
        coef[i+this->sp->y_edge_offset_r] = this->sp->connections[i].w;
        coef[i+this->sp->y_edge_offset_l] = -this->sp->connections[i].w;
    }
}

void sp_ilp_settings_find_position_t::set_coef_allow_overlap(std::vector<double> & coef){
    for(int i = 0; i<this->sp->connections.size(); ++i){
        coef[i+this->sp->x_edge_offset_r] = this->sp->connections[i].w;
        coef[i+this->sp->x_edge_offset_l] = -this->sp->connections[i].w;
        coef[i+this->sp->y_edge_offset_r] = this->sp->connections[i].w;
        coef[i+this->sp->y_edge_offset_l] = -this->sp->connections[i].w;

        
    }
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        coef[i+this->sp->x_overlap] = 0;
        coef[i+this->sp->y_overlap] = 0;
        //give overlap coefficient a huge penalty
    }
}

void sp_ilp_settings_find_position_t::set_constraints_opt_nets(){
    for(int i = 0; i<this->sp->connections.size(); ++i){
        int u = this->sp->connections[i].nodes[0], v = this->sp->connections[i].nodes[1];
        string x_l_constraint_name = "x_e_b_l"+ std::to_string(i)+"_"+std::to_string(i);
        string y_l_constraint_name = "y_e_b_l"+ std::to_string(i)+"_"+std::to_string(i);
        string x_r_constraint_name = "x_e_b_r"+ std::to_string(i)+"_"+std::to_string(i);
        string y_r_constraint_name = "y_e_b_r"+ std::to_string(i)+"_"+std::to_string(i);

        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2,{this->sp->x_module_offset+u, this->sp->x_edge_offset_r+i},{1, -1}, x_r_constraint_name, 0);
        this->sp->constraint_i++;
        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2,{this->sp->x_module_offset+v, this->sp->x_edge_offset_r+i},{1, -1}, x_r_constraint_name, 0);
        this->sp->constraint_i++;

        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2,{this->sp->x_edge_offset_l+i, this->sp->x_module_offset+u},{1, -1}, x_l_constraint_name, 0);
        this->sp->constraint_i++;
        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2,{this->sp->x_edge_offset_l+i, this->sp->x_module_offset+v},{1, -1}, x_l_constraint_name, 0);
        this->sp->constraint_i++;


        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2,{this->sp->y_module_offset+u, this->sp->y_edge_offset_r+i},{1, -1}, y_r_constraint_name, 0);
        this->sp->constraint_i++;
        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2,{this->sp->y_module_offset+v, this->sp->y_edge_offset_r+i},{1, -1}, y_r_constraint_name, 0);
        this->sp->constraint_i++;

        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2,{this->sp->y_edge_offset_l+i, this->sp->y_module_offset+u},{1, -1}, y_l_constraint_name, 0);
        this->sp->constraint_i++;
        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2,{this->sp->y_edge_offset_l+i, this->sp->y_module_offset+v},{1, -1}, y_l_constraint_name, 0);
        this->sp->constraint_i++;


    }
}

void sp_ilp_settings_find_position_t::set_constraints_opt_modules(){
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        string x_c_name = "x_c"+ std::to_string(i);
        string y_c_name = "y_c"+ std::to_string(i);
        {
            double center_i_x = this->sp->modules_positions[i].get_x()+this->sp->modules_wh[i].get_half_x();
            vector<double> coef;
            vector<int> ids;
            for(auto& e:this->sp->near_x_map[i]){
                vector<int> edge = this->sp->near_x[this->sp->near_x_id[i][e]];
                int h = edge[2];
                if(edge[0]==i){ // i-> j
                    coef.push_back(h/2);
                }
                else if(edge[1]==i){ // j->i
                    coef.push_back(-h/2);
                }
                ids.push_back(this->sp->near_x_id[i][e]+this->sp->near_x_offset);
            }
            coef.push_back(-1);
            ids.push_back(this->sp->x_module_offset+i);
            this->sp->ILP_solver.set_constraint_fx(this->sp->constraint_i, ids.size(),ids,coef, x_c_name, -center_i_x);
            this->sp->constraint_i++;
        }
        {
            double center_i_y = this->sp->modules_positions[i].get_y()+this->sp->modules_wh[i].get_half_y();
            vector<double> coef;
            vector<int> ids;
            for(auto& e:this->sp->near_y_map[i]){
                vector<int> edge = this->sp->near_y[this->sp->near_y_id[i][e]];
                int h = edge[2];
                if(edge[0]==i){ // i-> j
                    coef.push_back(h/2);
                }
                else if(edge[1]==i){ // j->i
                    coef.push_back(-h/2);
                }
                ids.push_back(this->sp->near_y_id[i][e]+this->sp->near_y_offset);
            }
            coef.push_back(-1);
            ids.push_back(this->sp->y_module_offset+i);
            this->sp->ILP_solver.set_constraint_fx(this->sp->constraint_i, ids.size(),ids,coef, y_c_name, -center_i_y);
            this->sp->constraint_i++;
        }
    }
}

void sp_ilp_settings_find_position_t::set_constraints_only_one_hand(){
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        if(sequence_pair_t::seq_is_fix[i]){continue;}
        vector<int> ids;
        for(auto& e:this->sp->near_x_map[i]){
            ids.push_back(this->sp->near_x_id[i][e] + this->sp->near_x_offset);
        }
        for(auto& e:this->sp->near_y_map[i]){
            ids.push_back(this->sp->near_y_id[i][e] + this->sp->near_y_offset);
        }
        vector<double> coef(ids.size(), 1);
        string x_c_name = "x_c" + std::to_string(i);
        // cout<< "i: "<<i<<" ";
        // for(auto& e:ids){cout<<e<<" ";}cout<<endl;
        if(ids.size()){
            this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, ids.size(),ids,coef, x_c_name, 1);
            this->sp->constraint_i++;
        }
    }
}

void sp_ilp_settings_find_position_t::set_variables_hands(){
    for(int i = 0; i<this->sp->near_x.size(); ++i){
        string var_name = "x_hand" + std::to_string(i);
        glp_set_col_name(this->sp->ILP_solver.ILP, this->sp->near_x_offset+i, var_name.c_str());
        this->sp->ILP_solver.set_variable_double_range_int(this->sp->near_x_offset + i, 0, 1);
    }
    for(int i = 0; i<this->sp->near_y.size(); ++i){
        string var_name = "y_hand" + std::to_string(i);
        glp_set_col_name(this->sp->ILP_solver.ILP, this->sp->near_y_offset + i, var_name.c_str());
        this->sp->ILP_solver.set_variable_double_range_int(this->sp->near_y_offset + i, 0, 1);
    }
}



sp_ilp_settings_find_position_with_area_t::sp_ilp_settings_find_position_with_area_t(sequence_pair_t * sp){
    this->sp = sp;
}

void sp_ilp_settings_find_position_with_area_t::set_constraints_modules_overlap_h(){
    for(int i = 0; i<this->sp->constraint_graph_h.size(); ++i){
        int from = this->sp->constraint_graph_h[i].from, to = this->sp->constraint_graph_h[i].to, w = this->sp->constraint_graph_h[i].w;
        string constraint_name = "h_c"+ std::to_string(i);
        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2, {from+1, to+1}, {1, -1}, constraint_name, -w);
        this->sp->constraint_i++;
    }
}

void sp_ilp_settings_find_position_with_area_t::set_constraints_modules_overlap_v(){
    for(int i = 0; i<this->sp->constraint_graph_v.size(); ++i){
        int from = this->sp->constraint_graph_v[i].from, to = this->sp->constraint_graph_v[i].to, w = this->sp->constraint_graph_v[i].w;
        string constraint_name = "v_c"+ std::to_string(i);
        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2, {from+1+sequence_pair_t::sequence_n, to+1+sequence_pair_t::sequence_n}, {1, -1}, constraint_name, -w);
        this->sp->constraint_i++;
    }
}

void sp_ilp_settings_find_position_with_area_t::set_constraints_modules_fixed(){
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        if(sequence_pair_t::seq_is_fix[i]){
            string x_constraint_name = "fix_x_c"+ std::to_string(i);
            string y_constraint_name = "fix_y_c"+ std::to_string(i);
            vec2d_t ll_pos = sequence_pair_t::seq_fixed_map[i]->get_left_lower();
            this->sp->ILP_solver.set_constraint_fx(this->sp->constraint_i, 1, {i+1}, {1}, x_constraint_name, static_cast<int>(ll_pos.get_x()));
            this->sp->constraint_i++;
            this->sp->ILP_solver.set_constraint_fx(this->sp->constraint_i, 1, {i+1+sequence_pair_t::sequence_n}, {1}, y_constraint_name, static_cast<int>(ll_pos.get_y()));
            this->sp->constraint_i++;
        }
    }
}

void sp_ilp_settings_find_position_with_area_t::set_constraints_net_direction(){
    for(int i = 0; i<this->sp->connections.size(); ++i){
        string x_constraint_name = "x_r_e"+ std::to_string(i);
        string y_constraint_name = "y_r_e"+ std::to_string(i);
        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2, {this->sp->x_edge_offset_l+i, this->sp->x_edge_offset_r+i}, {1, -1}, x_constraint_name, 0.0);
        this->sp->constraint_i++;
        this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2, {this->sp->y_edge_offset_l+i, this->sp->y_edge_offset_r+i}, {1, -1}, y_constraint_name, 0.0);
        this->sp->constraint_i++;
    }
}

void sp_ilp_settings_find_position_with_area_t::set_constraints_net(){
    for(int i = 0; i<this->sp->connections.size(); ++i){
        for(int j = 0; j<this->sp->connections[i].nodes.size(); ++j){
            int b = this->sp->connections[i].nodes[j];
            double b_x = this->sp->modules_wh[b].get_half_x();
            double b_y = this->sp->modules_wh[b].get_half_y();    
            string x_l_constraint_name = "x_e_b_l"+ std::to_string(i)+"_"+std::to_string(j);
            string y_l_constraint_name = "y_e_b_l"+ std::to_string(i)+"_"+std::to_string(j);
            string x_r_constraint_name = "x_e_b_r"+ std::to_string(i)+"_"+std::to_string(j);
            string y_r_constraint_name = "y_e_b_r"+ std::to_string(i)+"_"+std::to_string(j);
            this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2, {this->sp->x_edge_offset_l+i,b+this->sp->x_module_offset}, {1, -1}, x_l_constraint_name, b_x);
            this->sp->constraint_i++;
            this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2, {this->sp->y_edge_offset_l+i,b+this->sp->y_module_offset}, {1, -1}, y_l_constraint_name, b_y);
            this->sp->constraint_i++;
            this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2, {b+this->sp->x_module_offset,this->sp->x_edge_offset_r+i}, {1, -1}, x_r_constraint_name, -b_x);
            this->sp->constraint_i++;
            this->sp->ILP_solver.set_constraint_upb(this->sp->constraint_i, 2, {b+this->sp->y_module_offset,this->sp->y_edge_offset_r+i}, {1, -1}, y_r_constraint_name, -b_y);
            this->sp->constraint_i++;
        }
    }
}

void sp_ilp_settings_find_position_with_area_t::set_variables_modules(){
    for(int i = 1;i<=this->sp->sequence_n; ++i){
        string var_name = "x"+ std::to_string(i);
        glp_set_col_name(this->sp->ILP_solver.ILP, i, var_name.c_str());
        //ILP_solver.set_variable_double_range(i, 0.0,chip_t::get_width()-this->modules_wh[i-1].get_x());
        this->sp->ILP_solver.set_variable_double_range(i, 0.0,chip_t::get_width()-this->sp->modules_wh[i-1].get_x());
    }
    for(int i = sequence_pair_t::sequence_n+1;i<=2*sequence_pair_t::sequence_n; ++i){
        string var_name = "x"+ std::to_string(i);
        glp_set_col_name(this->sp->ILP_solver.ILP, i, var_name.c_str());
        //ILP_solver.set_variable_double_range(i, 0.0, chip_t::get_height()-this->modules_wh[ i-1-sequence_n].get_y());
        this->sp->ILP_solver.set_variable_double_range(i, 0.0, chip_t::get_height()-this->sp->modules_wh[ i-1-sequence_pair_t::sequence_n].get_y());
    }
}

void sp_ilp_settings_find_position_with_area_t::set_variables_connections(){
    for(int i = 0; i<this->sp->connections.size(); ++i){
        string var_name1 = "x_e_l"+ std::to_string(i);
        glp_set_col_name(this->sp->ILP_solver.ILP, this->sp->x_edge_offset_l+i, var_name1.c_str());
        this->sp->ILP_solver.set_variable_double_range(this->sp->x_edge_offset_l+i, 0.0, chip_t::get_width());

        string var_name2 = "x_e_r"+ std::to_string(i);
        glp_set_col_name(this->sp->ILP_solver.ILP, this->sp->x_edge_offset_r+i, var_name2.c_str());
        this->sp->ILP_solver.set_variable_double_range(this->sp->x_edge_offset_r+i, 0.0, chip_t::get_width());

        string var_name3 = "y_e_l"+ std::to_string(i);
        glp_set_col_name(this->sp->ILP_solver.ILP, this->sp->y_edge_offset_l+i, var_name3.c_str());
        this->sp->ILP_solver.set_variable_double_range(this->sp->y_edge_offset_l+i, 0.0, chip_t::get_height());

        string var_name4 = "y_e_r"+ std::to_string(i);
        glp_set_col_name(this->sp->ILP_solver.ILP, this->sp->y_edge_offset_r+i, var_name4.c_str());
        this->sp->ILP_solver.set_variable_double_range(this->sp->y_edge_offset_r+i, 0.0, chip_t::get_height());
    }
}

void sp_ilp_settings_find_position_with_area_t::set_coef(std::vector<double> & coef){
    for(int i = 0; i<this->sp->connections.size(); ++i){
        coef[i+this->sp->x_edge_offset_r] = this->sp->connections[i].w;
        coef[i+this->sp->x_edge_offset_l] = -this->sp->connections[i].w;
        coef[i+this->sp->y_edge_offset_r] = this->sp->connections[i].w;
        coef[i+this->sp->y_edge_offset_l] = -this->sp->connections[i].w;
    }
}
