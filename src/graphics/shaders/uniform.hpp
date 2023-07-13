#pragma once

#include <string>

#include "lib/opengl.hpp"
#include "graphics/shaders/shader.hpp"

enum class UniformType {
    INT, UINT, FLOAT, DOUBLE, VEC2, VEC3, VEC4, MAT4
};

struct Uniform {
    UniformType type;
    union {
        int v_int;
        GLuint v_uint;
        float v_float;
        double v_double;
        glm::vec2 v_vec2;
        glm::vec3 v_vec3;
        glm::vec4 v_vec4;
        glm::mat4 v_mat4;
    };

    Uniform()
        : type(UniformType::INT), v_int(0) {}
    
    Uniform(int v_int)
        : type(UniformType::INT), v_int(v_int) {}
    Uniform(GLuint v_uint)
        : type(UniformType::UINT), v_uint(v_uint) {}
    Uniform(float v_float)
        : type(UniformType::FLOAT), v_float(v_float) {}
    Uniform(double v_double)
        : type(UniformType::DOUBLE), v_double(v_double) {}
    Uniform(const glm::vec2& v_vec2)
        : type(UniformType::VEC2), v_vec2(v_vec2) {}
    Uniform(const glm::vec3& v_vec3)
        : type(UniformType::VEC3), v_vec3(v_vec3) {}
    Uniform(const glm::vec4& v_vec4)
        : type(UniformType::VEC4), v_vec4(v_vec4) {}
    Uniform(const glm::mat4& v_mat4)
        : type(UniformType::MAT4), v_mat4(v_mat4) {}
    
    void set(Shader& shader, const std::string& name) const {
        switch (type) {
            case UniformType::INT:
                shader.uniform(name, v_int);
                break;
            case UniformType::UINT:
                shader.uniform(name, v_uint);
                break;
            case UniformType::FLOAT:
                shader.uniform(name, v_float);
                break;
            case UniformType::DOUBLE:
                shader.uniform(name, (float)v_double);
                break;
            case UniformType::VEC2:
                shader.uniform(name, v_vec2);
                break;
            case UniformType::VEC3:
                shader.uniform(name, v_vec3);
                break;
            case UniformType::VEC4:
                shader.uniform(name, v_vec4);
                break;
            case UniformType::MAT4:
                shader.uniform(name, v_mat4);
                break;
        }
    }
};
