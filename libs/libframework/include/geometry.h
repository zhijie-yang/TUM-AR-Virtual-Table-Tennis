#ifndef LIBFRAMEWORK_GEOMETRY
#define LIBFRAMEWORK_GEOMETRY

#include <glm/gtx/transform.hpp>

class Transform: public glm::mat4 {
    glm::mat3 get_rotm() {
        glm::mat3 rotm;
        // TODO @Siyun-Liang column-major or row-major???
        return rotm;
    }

    glm::vec3 get_translation() {
        glm::vec3 t;
        // TODO @Siyun-Liang
        return t;
    }

    // SE(3) inverse
    glm::mat4 inverse() {
        // TODO @Siyun-Liang
    }

    void set_rotm() {
        // TODO @Siyun-Liang
    }

    void set_translation() {
        // TODO @Siyun-Liang
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