#ifndef LIBFRAMEWORK_RESPONSES
#define LIBFRAMEWORK_RESPONSES

#include "infos.h"
#include "libnetwork/proto_src/network.pb.h"

class GeneralResponse {
public:
    GeneralResponse();
    GeneralResponse(bool const& result, std::string const& detail) {
        this->result = result;
        this->detail = detail;
    }
    ~GeneralResponse();

private:
    bool result;
    std::string detail;

public:
    inline bool get_result() {
        return this->result;
    }

    inline std::string get_detail() {
        return this->detail;
    }

    inline void toProto(libnetwork::GeneralResponse& r) {
        r.set_result(this->result);
        r.set_detail(this->detail);
    }

    inline libnetwork::GeneralResponse* toProtoAllocated() {
        auto r = new libnetwork::GeneralResponse();
        r->set_result(this->result);
        r->set_detail(this->detail);
        return r;
    }

    inline static void fromProto(const libnetwork::GeneralResponse &r, GeneralResponse &out) {
        out.result = r.result();
        out.detail = r.detail();
    }
};


class StartNewRoundResponse {
public:
    StartNewRoundResponse();
    StartNewRoundResponse(PlayerInfo const& player_info) {
        this->player_info = player_info;
    }
    ~StartNewRoundResponse();

private:
    PlayerInfo player_info;

public:
    inline PlayerInfo get_player_info() {
        return this->player_info;
    }

    inline void set_player_info(PlayerInfo const& p) {
        this->player_info = p;
    }

    inline void toProto(libnetwork::StartNewRoundResponse& r) {
        r.set_allocated_player_info(this->player_info.toProtoAllocated());
    }

    inline libnetwork::StartNewRoundResponse* toProtoAllocated() {
        auto r = new libnetwork::StartNewRoundResponse();
        r->set_allocated_player_info(this->player_info.toProtoAllocated());
        return r;
    }

    inline static void fromProto(const libnetwork::StartNewRoundResponse &r, StartNewRoundResponse &out) {
        PlayerInfo::fromProto(r.player_info(), out.player_info);
    }
};

class RoundEndingResponse {
public:
    RoundEndingResponse();
    RoundEndingResponse(bool const& result, ScoreBoard const& score_board) {
        this->result = result;
        this->score_board = score_board;
    }
    ~RoundEndingResponse();

private:
    bool result;
    ScoreBoard score_board;

public:
    inline bool get_result() {
        return this->result;
    }

    inline ScoreBoard get_score_board() {
        return this->score_board;
    }

    inline void toProto(libnetwork::RoundEndingResponse& r) {
        r.set_result(this->result);
        r.set_allocated_score_board(this->score_board.toProtoAllocated());
    }

    inline libnetwork::RoundEndingResponse* toProtoAllocated() {
        auto r = new libnetwork::RoundEndingResponse();
        r->set_result(this->result);
        r->set_allocated_score_board(this->score_board.toProtoAllocated());
        return r;
    }

    inline static void fromProto(const libnetwork::RoundEndingResponse &r, RoundEndingResponse &out) {
        out.result = r.result();
        ScoreBoard::fromProto(r.score_board(), out.score_board);
    }

};

class ClientConnectionResponse {
public:
    ClientConnectionResponse();
    ClientConnectionResponse(bool const& result, std::string const& detail, PlayerInfo const& player_info) {
        this->result = result;
        this->detail = detail;
        this->player_info = player_info;
    }
    ~ClientConnectionResponse();

private:
    bool result;
    std::string detail;
    PlayerInfo player_info;

public:
    inline bool get_result() {
        return this->result;
    }

    inline std::string get_detail() {
        return this->detail;
    }

    inline PlayerInfo get_player_info() {
        return this->player_info;
    }

    inline void toProto(libnetwork::ClientConnectionResponse& r) {
        r.set_result(this->result);
        r.set_detail(this->detail);
        r.set_allocated_player_info(this->player_info.toProtoAllocated());
    }

    inline libnetwork::ClientConnectionResponse* toProtoAllocated() {
        auto r = new libnetwork::ClientConnectionResponse();
        r->set_result(this->result);
        r->set_detail(this->detail);
        r->set_allocated_player_info(this->player_info.toProtoAllocated());
        return r;
    }

    inline static void fromProto(const libnetwork::ClientConnectionResponse &r, ClientConnectionResponse &out) {
        out.result = r.result();
        out.detail = r.detail();
        PlayerInfo::fromProto(r.player_info(), out.player_info);
    }

};

#endif
// !LIBFRAMEWORK_RESPONSES
