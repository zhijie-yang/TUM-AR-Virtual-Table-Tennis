#ifndef LIBFRAMEWORK_REQUESTS
#define LIBFRAMEWORK_REQUESTS

#include <string>
#include "infos.h"
#include "libnetwork/proto_src/network.pb.h"

class StartNewRoundRequest {
public:
    StartNewRoundRequest(){}
    StartNewRoundRequest(PlayerInfo const& player_info) {
        this->player_info = player_info;
    }
    ~StartNewRoundRequest(){}

private:
    PlayerInfo player_info;

public:
    inline PlayerInfo get_player_info() {
        return this->player_info;
    }

    inline void toProto(libnetwork::StartNewRoundRequest& r) {
        r.set_allocated_player_info(this->player_info.toProtoAllocated());
    }

    inline libnetwork::StartNewRoundRequest* toProtoAllocated() {
        auto r = new libnetwork::StartNewRoundRequest();
        r->set_allocated_player_info(this->player_info.toProtoAllocated());
        return r;
    }

    inline static void fromProto(const libnetwork::StartNewRoundRequest &r, StartNewRoundRequest &out) {
        PlayerInfo::fromProto(r.player_info(), out.player_info);
    }
};

class RoundEndingRequest {
public:
    RoundEndingRequest(){}
    RoundEndingRequest(unsigned const& winner_id) {
        this->winner_id = winner_id;
    }
    ~RoundEndingRequest(){}

private:
    unsigned winner_id;


public:
    unsigned get_winner_id() {
        return this->winner_id;
    }

    inline void toProto(libnetwork::RoundEndingRequest& r) {
        r.set_winner_id(this->winner_id);
    }

    inline libnetwork::RoundEndingRequest* toProtoAllocated() {
        auto r = new libnetwork::RoundEndingRequest();
        r->set_winner_id(this->winner_id);
        return r;
    }

    inline static void fromProto(const libnetwork::RoundEndingRequest &r, RoundEndingRequest &out) {
        out.winner_id = r.winner_id();
    }

};

class ClientConnectionRequest {
public:
    ClientConnectionRequest(){}
    ClientConnectionRequest(std::string const& player_name) {
        this->player_name = player_name;
    }
    ~ClientConnectionRequest(){}

private:
    std::string player_name;

public:
    inline std::string get_player_name() {
        return this->player_name;
    }

    inline void toProto(libnetwork::ClientConnectionRequest& r) {
        r.set_player_name(this->player_name);
    }

    inline libnetwork::ClientConnectionRequest* toProtoAllocated() {
        auto r = new libnetwork::ClientConnectionRequest();
        r->set_player_name(this->player_name);
        return r;
    }

    inline static void fromProto(const libnetwork::ClientConnectionRequest &r, ClientConnectionRequest &out) {
        out.player_name = r.player_name();
    }

};

#endif
// !LIBFRAMEWORK_REQUESTS