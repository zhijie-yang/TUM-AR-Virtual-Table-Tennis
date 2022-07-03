#ifndef LIBFRAMEWORK_BALL
#define LIBFRAMEWORK_BALL

#include "geometry.h"
#include "libnetwork/proto_src/network.pb.h"

enum FlyingStatus {
        UNKNOWN,
        WAITING_START,
        FLYING,
        HIT_WITH_RACKET,
        HIT_WITH_TABLE,
        HIT_WITH_NET,
        OUT_OF_BOUND
};

class BallStatus {
public:
    BallStatus();
    BallStatus(unsigned const& ball_id, Transform const& pose,
               Velocity const& velocity, unsigned const& flying_status) {
        this->ball_id = ball_id;
        this->pose = pose;
        this->velocity = velocity;
        this->flying_status = static_cast<FlyingStatus>(flying_status);
    }
    ~BallStatus();

private:
    unsigned ball_id;
    Transform pose;
    Velocity velocity;
    enum FlyingStatus flying_status;

public:
    inline void toProto(libnetwork::BallStatus& r) {
        r.set_ball_id(this->ball_id);
        r.set_allocated_pose(this->pose.toProtoAllocated());
        r.set_allocated_velocity(this->velocity.toProtoAllocated());
        r.set_flying_status(static_cast<libnetwork::FlyingStatus>(this->flying_status));
    }

    inline libnetwork::BallStatus* toProtoAllocated() {
        auto r = new libnetwork::BallStatus();
        r->set_ball_id(this->ball_id);
        r->set_allocated_pose(this->pose.toProtoAllocated());
        r->set_allocated_velocity(this->velocity.toProtoAllocated());
        r->set_flying_status(static_cast<libnetwork::FlyingStatus>(this->flying_status));
        return r;
    }

    inline static void fromProto(const libnetwork::BallStatus &r, BallStatus &out) {
        out.ball_id = r.ball_id();
        Transform::fromProto(r.pose(), out.pose);
        Velocity::fromProto(r.velocity(), out.velocity);
        out.flying_status = static_cast<FlyingStatus>(r.flying_status());
    }
};

#endif // !LIBFRAMEWORK_BALL