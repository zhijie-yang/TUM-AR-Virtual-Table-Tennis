#ifndef LIBFRAMEWORK_RACKET
#define LIBFRAMEWORK_RACKET

#include "geometry.h"
#include "libnetwork/proto_src/network.pb.h"

class RacketStatus {
public:
    RacketStatus(){}
    RacketStatus(unsigned const& player_id, Transform const& pose, Velocity const& velocity) {
        this->player_id = player_id;
        this->pose = pose;
        this->velocity = velocity;
    }
    ~RacketStatus(){}

private:
    unsigned player_id;
    Transform pose;
    Velocity velocity;

public:
    inline void toProto(libnetwork::RacketStatus& r) {
        r.set_player_id(this->player_id);
        r.set_allocated_pose(this->pose.toProtoAllocated());
        r.set_allocated_velocity(this->velocity.toProtoAllocated());
    }

    inline libnetwork::RacketStatus* toProtoAllocated() {
        auto r = new libnetwork::RacketStatus();
        r->set_player_id(this->player_id);
        r->set_allocated_pose(this->pose.toProtoAllocated());
        r->set_allocated_velocity(this->velocity.toProtoAllocated());
        return r;
    }

    inline static void fromProto(const libnetwork::RacketStatus &r, RacketStatus &out) {
        out.player_id = r.player_id();
        Transform::fromProto(r.pose(), out.pose);
        Velocity::fromProto(r.velocity(), out.velocity);
    }

    inline Transform get_pose() const {
        return this->pose;
    }
};

#endif // !LIBFRAMEWORK_RACKET
