#ifndef LIBFRAMEWORK_INFOS
#define LIBFRAMEWORK_INFOS

#include <string>

struct PlayerInfo {
    unsigned player_id;
    std::string player_name;
};

struct ScoreBoard {
    unsigned player_1_score;
    unsigned player_2_score;
};

struct ServerBroadcast {
    std::string ip_address;
    std::string host_name;
};

#endif
// !LIBFRAMEWORK_INFOS