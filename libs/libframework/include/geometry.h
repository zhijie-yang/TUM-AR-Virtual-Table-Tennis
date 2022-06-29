#ifndef LIBFRAMEWORK_GEOMETRY
#define LIBFRAMEWORK_GEOMETRY

#include <glm/gtx/transform.hpp>

class Transform {
private:
    glm::mat4 transform;

public:
    glm::mat4 get_transform() {
        return this->transform;
    }

    glm::mat3 get_rotm() {
        return glm::mat3(glm::vec3(this->transform[0]),
                                   glm::vec3(this->transform[1]),
                                   glm::vec3(this->transform[2]));
    }

    glm::vec3 get_translation() {
        return glm::vec3(this->transform[3]);
    }

    // SE(3) inverse
    glm::mat4 inverse() {
        glm::vec3 translation = this->get_translation();
        glm::mat3 rotm = this->get_rotm();
        glm::mat3 rotmInverse = glm::transpose(rotm);
        glm::vec3 transInverse = - rotmInverse * translation;
        return glm::mat4(glm::vec4(rotmInverse[0], 0),
                         glm::vec4(rotmInverse[1], 0),
                         glm::vec4(rotmInverse[2], 0),
                         glm::vec4(transInverse, 1));
    }

    void set_rotm(glm::mat3 rotm) {
        this->transform[0] = glm::vec4(rotm[0], 0);
        this->transform[1] = glm::vec4(rotm[1], 0);
        this->transform[2] = glm::vec4(rotm[2], 0);
    }

    void set_translation(glm::vec3 translation) {
        this->transform[3] = glm::vec4(translation, 1);
    }

};

class Velocity {
    Velocity (glm::vec3 const& linear, glm::vec3 const& angular) {
        this->linear = linear;
        this->angular = angular;
    }

private:
    glm::vec3 linear;
    glm::vec3 angular;

public:
    glm::vec3 get_linear() {
        return this->linear;
    }

    glm::vec3 get_angular() {
        return this->angular;
    }

    void set_linear(glm::vec3 const& linear) {
        this->linear = linear;
    }

    void set_angular(glm::vec3 const& angular) {
        this->angular = angular;
    }
};

#endif // !LIBFRAMEWORK_GEOMETRY