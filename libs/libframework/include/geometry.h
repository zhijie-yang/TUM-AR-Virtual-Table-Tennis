#ifndef LIBFRAMEWORK_GEOMETRY
#define LIBFRAMEWORK_GEOMETRY

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "libnetwork/proto_src/network.pb.h"

class Transform{
public:
    Transform(glm::mat4 const& m) {data:m;}

private:
    glm::mat4 data; 

public:
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

    const glm::f32* get_value_ptr() {
        return glm::value_ptr(data);
    }    

    libnetwork::Transform toProto() {
        libnetwork::Transform t;
        const float *m = (const float*) this->get_value_ptr();
        for (int i = 0; i < 16; ++i)
            t.add_transform(m[i]);
        return t;
    }

    void fromProto(const libnetwork::Transform &t, Transform &out) {
        
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

    libnetwork::Velocity toProto() {
        libnetwork::Velocity v;
        v.set_v_x(this->linear[0]);
        v.set_v_y(this->linear[1]);
        v.set_v_z(this->linear[2]);
        v.set_w_x(this->angular[0]);
        v.set_w_y(this->angular[1]);
        v.set_w_z(this->angular[2]);
        return v;
    }
};

#endif // !LIBFRAMEWORK_GEOMETRY