//
// Created by Ryou on 2022/7/11.
//

#ifndef MODERN_CMAKE_TEMPLATE_CONSTANT_H
#define MODERN_CMAKE_TEMPLATE_CONSTANT_H

#include <glm/glm.hpp>

class Constant {
public:
    // length(depth) = 2.74, width = 1.525, net height = 0.1525
    constexpr static const glm::vec3 table_scale = glm::vec3(1.525f, 0.1525f, 2.74f);
    constexpr static const glm::vec3 ball_scale = glm::vec3(0.045f, 0.045f, 0.045f);
    constexpr static const glm::vec3 racket_scale = glm::vec3(0.0f, 0.0f, 0.0f);
    constexpr static const  float ball_mass = 0.0025;
};

#endif //MODERN_CMAKE_TEMPLATE_CONSTANT_H
