#ifndef LIBFRAMEWORK_INFOS
#define LIBFRAMEWORK_INFOS

#include <string>
#include "libnetwork/proto_src/network.pb.h"

struct PlayerInfo {
public:
    PlayerInfo();
    PlayerInfo(unsigned const& player_id, std::string const& player_name) {
        this->player_id = player_id;
        this->player_name = player_name;
    }
    ~PlayerInfo();

private:
    unsigned player_id;
    std::string player_name;

public:
    inline unsigned get_player_id() {
        return this->player_id;
    }

    inline std::string get_player_name() {
        return this->player_name;
    }

    inline libnetwork::PlayerInfo* toProto() {
        auto r = new libnetwork::PlayerInfo();
        r->set_player_id(this->player_id);
        r->set_player_name(this->player_name);
        return r;
    }

    inline static void fromProto(const libnetwork::PlayerInfo &r, PlayerInfo &out) {
        out.player_id = r.player_id();
        out.player_name = r.player_name();
    }

};

struct ScoreBoard {
public:
    ScoreBoard();
    ScoreBoard(unsigned const& player_1_score, unsigned const& player_2_score) {
        this->player_1_score = player_1_score;
        this->player_2_score = player_2_score;
    }
    ~ScoreBoard();

private:
    unsigned player_1_score;
    unsigned player_2_score;

public:
    inline unsigned get_player_1_score() {
        return this->player_1_score;
    }

    inline unsigned get_player_2_score() {
        return this->player_2_score;
    }

    inline libnetwork::ScoreBoard* toProto() {
        auto r = new libnetwork::ScoreBoard();
        r->set_player_1_score(this->player_1_score);
        r->set_player_2_score(this->player_1_score);
        return r;
    }

    inline static void fromProto(const libnetwork::ScoreBoard &r, ScoreBoard &out) {
        out.player_1_score = r.player_1_score();
        out.player_2_score = r.player_2_score();
    }

};

struct ServerBroadcast {
public:
    ServerBroadcast();
    ServerBroadcast(std::string ip_address, std::string host_name) {
        this->ip_address = ip_address;
        this->host_name = host_name;
    }
    ~ServerBroadcast();

private:
    std::string ip_address;
    std::string host_name;

public:
    std::string get_ip_addr() {
        return this->ip_address;
    }

    std::string get_host_name() {
        return this->host_name;
    }

    libnetwork::ServerBroadcast* toProto() {
        auto r = new libnetwork::ServerBroadcast();
        r->set_ip_address(this->ip_address);
        r->set_host_name(this->host_name);
        return r;
    }

    static void fromProto(const libnetwork::ServerBroadcast &r, ServerBroadcast &out) {
        out.ip_address = r.ip_address();
        out.host_name = r.host_name();
    }

};

#endif
// !LIBFRAMEWORK_INFOS