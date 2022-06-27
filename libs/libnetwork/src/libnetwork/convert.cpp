#include "convert.h"

libnetwork::Transform Transform::toProto() {
    libnetwork::Transform t;
    const float *m = (const float*) this->get_value_ptr();
    for (int i = 0; i < 16; ++i)
        t.add_transform(m[i]);
    return t;
}



libnetwork::BallStatus BallStatus::toProto() {
    libnetwork::BallStatus msg;
    msg.set_ball_id(this->ball_id);
    Transform t = this->pose;
    msg.set_allocated_pose(pose.toProto());
    msg.set_allocated_velocity(this->velocity);
}
