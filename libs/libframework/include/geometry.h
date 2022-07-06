#ifndef LIBFRAMEWORK_GEOMETRY
#define LIBFRAMEWORK_GEOMETRY

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "libnetwork/proto_src/network.pb.h"

class Transform {
public:
    Transform(){}
    Transform(glm::mat4 const& m) {this->transform = m;}
    ~Transform(){}

private:
    glm::mat4 transform;

public:
    inline glm::mat4 get_transform() {
        return this->transform;
    }

    inline glm::mat3 get_rotm() {
        return glm::mat3(glm::vec3(this->transform[0]),
                                   glm::vec3(this->transform[1]),
                                   glm::vec3(this->transform[2]));
    }

    inline glm::vec3 get_translation() {
        return glm::vec3(this->transform[3]);
    }

    // SE(3) inverse
    inline glm::mat4 inverse() {
        glm::vec3 translation = this->get_translation();
        glm::mat3 rotm = this->get_rotm();
        glm::mat3 rotmInverse = glm::transpose(rotm);
        glm::vec3 transInverse = - rotmInverse * translation;
        return glm::mat4(glm::vec4(rotmInverse[0], 0),
                         glm::vec4(rotmInverse[1], 0),
                         glm::vec4(rotmInverse[2], 0),
                         glm::vec4(transInverse, 1));
    }

    inline void set_rotm(glm::mat3 rotm) {
        this->transform[0] = glm::vec4(rotm[0], 0);
        this->transform[1] = glm::vec4(rotm[1], 0);
        this->transform[2] = glm::vec4(rotm[2], 0);
    }

    inline void set_translation(glm::vec3 translation) {
        this->transform[3] = glm::vec4(translation, 1);
    }

    inline const glm::f32* get_value_ptr() {
        return glm::value_ptr(this->transform);
    }    

    inline void toProto(libnetwork::Transform& t) {
        const float *m = (const float*) this->get_value_ptr();
        for (int i = 0; i < 16; ++i)
            t.add_transform(m[i]);
    }

    inline libnetwork::Transform* toProtoAllocated() {
        auto t = new libnetwork::Transform();
        const float *m = (const float*) this->get_value_ptr();
        for (int i = 0; i < 16; ++i)
            t->add_transform(m[i]);
        return t;
    }

    inline static void fromProto(const libnetwork::Transform &t, Transform &out) {
        float tmat[16];
        for (int i = 0; i < 16; ++i)
            tmat[i] = t.transform(i);
        out.transform = glm::make_mat4(tmat);
    }

};

class Velocity {
public:
    Velocity(){}
    Velocity (glm::vec3 const& linear, glm::vec3 const& angular) {
        this->linear = linear;
        this->angular = angular;
    }
    ~Velocity(){}

private:
    glm::vec3 linear;
    glm::vec3 angular;

public:
    inline glm::vec3 get_linear() {
        return this->linear;
    }

    inline glm::vec3 get_angular() {
        return this->angular;
    }

    inline void set_linear(glm::vec3 const& linear) {
        this->linear = linear;
    }

    inline void set_angular(glm::vec3 const& angular) {
        this->angular = angular;
    }

    inline void toProto(libnetwork::Velocity& v) {
        v.set_v_x(this->linear[0]);
        v.set_v_y(this->linear[1]);
        v.set_v_z(this->linear[2]);
        v.set_w_x(this->angular[0]);
        v.set_w_y(this->angular[1]);
        v.set_w_z(this->angular[2]);
    }

    inline libnetwork::Velocity* toProtoAllocated() {
        auto v = new libnetwork::Velocity();
        v->set_v_x(this->linear[0]);
        v->set_v_y(this->linear[1]);
        v->set_v_z(this->linear[2]);
        v->set_w_x(this->angular[0]);
        v->set_w_y(this->angular[1]);
        v->set_w_z(this->angular[2]);
        return v;
    }

    inline static void fromProto(const libnetwork::Velocity &v, Velocity &out) {
        float v_arr[3] = {v.v_x(), v.v_y(), v.v_z()};
        float w_arr[3] = {v.w_x(), v.w_y(), v.w_z()};
        out.linear = glm::make_vec3(v_arr);
        out.angular = glm::make_vec3(w_arr);
    }
};

#endif // !LIBFRAMEWORK_GEOMETRY