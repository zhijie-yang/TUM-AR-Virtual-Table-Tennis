#ifndef LIBFRAMEWORK_BALL
#define LIBFRAMEWORK_BALL

#include "geometry.h"
#include "libnetwork/proto_src/network.pb.h"

enum Status {
        UNKNOWN,
        WAITING_START,
        FLYING,
        HIT_WITH_RACKET,
        HIT_WITH_TABLE,
        HIT_WITH_NET,
        OUT_OF_BOUND
};

class BallStatus {
    BallStatus();
    ~BallStatus();

private:
    unsigned int ball_id;
    Transform pose;
    Velocity velocity;
    enum Status status;

public:
    virtual libnetwork::BallStatus toProto() = 0;
};

#endif // !LIBFRAMEWORK_BALL