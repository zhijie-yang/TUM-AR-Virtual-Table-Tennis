#ifndef LIBFRAMEWORK_RESPONSES
#define LIBFRAMEWORK_RESPONSES

#include <infos.h>

struct RacketStatusResponse {
    bool result;
    std::string detail;
};

struct ChangeTurnResponse {
    bool result;
    std::string detail;
};

struct BallStatusResponse {
    bool result;
    std::string detail;
};

struct StartNewRoundResponse {
    PlayerInfo player_info;
};

struct RoundEndingResponse {
    bool result;
    ScoreBoard score_board;
};

struct ScoreBoardResponse {
    bool result;
    std::string detail;
};

struct ClientConnectionResponse {
    bool result;
    std::string detail;
    PlayerInfo player_info;
};

#endif
// !LIBFRAMEWORK_RESPONSES
