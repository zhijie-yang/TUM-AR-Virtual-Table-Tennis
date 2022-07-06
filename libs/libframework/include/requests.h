#ifndef LIBFRAMEWORK_REQUESTS
#define LIBFRAMEWORK_REQUESTS

#include <string>
#include "infos.h"
#include "libnetwork/proto_src/network.pb.h"

class ChangeTurnRequest {
public:
    ChangeTurnRequest(){}
    ChangeTurnRequest(unsigned const& new_id) {
        this->new_id = new_id;
    }
    ~ChangeTurnRequest(){}

private:
    unsigned new_id;

public:
    inline unsigned get_new_id() {
        return this->new_id;
    }

    inline void toProto(libnetwork::ChangeTurnRequest& r) {
        r.set_new_id(this->new_id);
    }

    inline libnetwork::ChangeTurnRequest* toProtoAllocated() {
        auto r = new libnetwork::ChangeTurnRequest();
        r->set_new_id(this->new_id);
        return r;
    }

    inline static void fromProto(const libnetwork::ChangeTurnRequest &r, ChangeTurnRequest &out) {
        out.new_id = r.new_id();
    }
};

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
    ClientConnectionRequest(std::string const& server_ip,
                            PlayerInfo const& player_info, unsigned const& port_number) {
        this->server_ip_address = server_ip;
        this->player_info = player_info;
        this->port_number = port_number;
    }
    ~ClientConnectionRequest(){}

private:
    std::string server_ip_address;
    PlayerInfo player_info;
    unsigned port_number;

public:
    inline std::string get_server_ip() {
        return this->server_ip_address;
    }

    inline PlayerInfo get_player_info() {
        return this->player_info;
    }

    inline unsigned get_port_number() {
        return this->port_number;
    }

    inline void toProto(libnetwork::ClientConnectionRequest& r) {
        r.set_server_ip_address(this->server_ip_address);
        r.set_allocated_player_info(this->player_info.toProtoAllocated());
        r.set_port_number(this->port_number);
    }

    inline libnetwork::ClientConnectionRequest* toProtoAllocated() {
        auto r = new libnetwork::ClientConnectionRequest();
        r->set_server_ip_address(this->server_ip_address);
        r->set_allocated_player_info(this->player_info.toProtoAllocated());
        r->set_port_number(this->port_number);
        return r;
    }

    inline static void fromProto(const libnetwork::ClientConnectionRequest &r, ClientConnectionRequest &out) {
        out.server_ip_address = r.server_ip_address();
        PlayerInfo::fromProto(r.player_info(), out.player_info);
        out.port_number = r.port_number();
    }

};

#endif
// !LIBFRAMEWORK_REQUESTS