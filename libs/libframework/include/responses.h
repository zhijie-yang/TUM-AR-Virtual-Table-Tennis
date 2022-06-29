#ifndef LIBFRAMEWORK_RESPONSES
#define LIBFRAMEWORK_RESPONSES

#include "infos.h"
#include "libnetwork/proto_src/network.pb.h"

class GeneralResponse {
public:
    GeneralResponse(bool const& result, std::string const& detail) {
        this->result = result;
        this->detail = detail;
    }

private:
    bool result;
    std::string detail;

public:
    bool get_result() {
        return this->result;
    }

    std::string get_detail() {
        return this->detail;
    }

    libnetwork::GeneralResponse toProto() {
        libnetwork::GeneralResponse r;
        r.result = this->result;
        r.detail = this->detail;
    }

    void fromProto(const libnetwork::GeneralResponse &r, GeneralResponse &out) {
        out.result = r.result();
        out.detail = r.detail();
    }
};


class StartNewRoundResponse {
    PlayerInfo player_info;
};

class RoundEndingResponse {
    bool result;
    ScoreBoard score_board;
};

class ClientConnectionResponse {
    bool result;
    std::string detail;
    PlayerInfo player_info;
};

#endif
// !LIBFRAMEWORK_RESPONSES
