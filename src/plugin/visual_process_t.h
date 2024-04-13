
#ifndef INC_2023PD_VISUAL_PROCESS_T_H
#define INC_2023PD_VISUAL_PROCESS_T_H

#include <vector>
#include "utilities/vec2d_t.h"
#include "utilities/line_t.h"

class visual_process_t;

class visual_unit_t {
    friend class visual_process_t;
protected:
    int index;
public:
    virtual ~visual_unit_t() = default;
    virtual std::string to_string() const = 0;
    virtual std::string to_setting() const = 0;
    virtual std::string to_label() const = 0;
};

class visual_unit_polygon_t : public visual_unit_t {
    std::vector<vec2d_t> nodes;
    std::string name;
    bool vertex_label;
public:
    visual_unit_polygon_t(const std::vector<vec2d_t>& nodes);
    visual_unit_polygon_t(const std::vector<vec2d_t>& nodes, bool vertex_label);
    visual_unit_polygon_t(const std::vector<vec2d_t>& nodes, std::string name);
    visual_unit_polygon_t(const std::vector<vec2d_t>& nodes, std::string name, bool vertex_label);
    // ~visual_unit_polygon_t() = default;
    std::string to_string() const;
    std::string to_setting() const;
    std::string to_label() const;
};

class visual_unit_line_t : public visual_unit_t {
    line_t line;
public:
    visual_unit_line_t(const line_t& line);
    // ~visual_unit_line_t() = default;
    std::string to_string() const;
    std::string to_setting() const;
    std::string to_label() const;
};

class visual_process_t {
    std::string title = "";
    std::vector<visual_unit_t*> units;
public:
    void thread_plot();
    ~visual_process_t();
    void push_unit(visual_unit_t* unit);
    void plot();
};

#endif //INC_2023PD_VISUAL_PROCESS_T_H
