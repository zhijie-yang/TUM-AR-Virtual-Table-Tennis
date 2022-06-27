#ifndef LIBFRAMEWORK_REQUESTS
#define LIBFRAMEWORK_REQUESTS

#include <string>
#include "infos.h"

struct ChangeTurnRequest {
    unsigned new_id;
};

struct StartNewRoundRequest {
    PlayerInfo player_info;
};

struct RoundEndingRequest {
    unsigned winner_id;
};

struct ClientConnectionRequest {
    std::string server_ip_address;
    PlayerInfo player_info;
    unsigned port_number;
};

#endif
// !LIBFRAMEWORK_REQUESTS