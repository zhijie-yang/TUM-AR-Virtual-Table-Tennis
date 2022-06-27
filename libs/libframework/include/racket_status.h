#ifndef LIBFRAMEWORK_RACKET
#define LIBFRAMEWORK_RACKET

#include "geometry.h"

enum Status {
        UNKNOWN,
        WAITING_START,
        FLYING,
        HIT_WITH_RACKET,
        HIT_WITH_TABLE,
        HIT_WITH_NET,
        OUT_OF_BOUND
};

class RacketStatus {
    RacketStatus();
    ~RacketStatus();
private:
    unsigned int player_id;
    Transform pose;
    Velocity velocity;
};

#endif // !LIBFRAMEWORK_RACKET