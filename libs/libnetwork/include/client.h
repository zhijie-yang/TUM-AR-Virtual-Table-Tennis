#ifndef LIBNETWORK_CLIENT_H
#define LIBNETWORK_CLIENT_H

#include <grpc++/grpc++.h>
#include <sstream>

#include "libframework/include/infos.h"
#include "libframework/include/responses.h"
#include "libframework/include/requests.h"
#include "libframework/include/geometry.h"
#include "libframework/include/ball_status.h"
#include "libframework/include/racket_status.h"
#include "libnetwork/proto_src/network.pb.h"
#include "libnetwork/proto_src/network.grpc.pb.h"

class VirtualTennisNetworkClient {
public:
    VirtualTennisNetworkClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(libnetwork::VirtualTennis::NewStub(channel)) {}
    VirtualTennisNetworkClient(std::string const& ip_address, unsigned const& port,
                    std::string const& player_name) {
        std::stringstream fmt_port;
        fmt_port << port;
        std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(ip_address + std::string(":") + fmt_port.str(),
                                                                    grpc::InsecureChannelCredentials());
        this->stub_ = libnetwork::VirtualTennis::NewStub(channel);
        this->server_ip = ip_address;
        this->server_port = port;
        this->player_name = player_name;
        this->connected = false;
    }

    ClientConnectionResponse connectServer();
    GeneralResponse disconnectServer();
    GeneralResponse sendBallStatus(BallStatus& _request);
    BallStatus getBallStatus();
    GeneralResponse sendRacketStatus(RacketStatus& _request);
    RacketStatus getRacketStatus(unsigned const& player_id);
    GeneralResponse sendScoreBoard(ScoreBoard& _request);
    ScoreBoard getScoreBoard();
    GeneralResponse changeTurn();
    unsigned getCurrentTurn();
    StartNewRoundResponse startNewRound(StartNewRoundRequest& _request);
    RoundEndingResponse endRound(RoundEndingRequest& _request);
    std::string getOpponentName();
    bool onVisionTickEnd(RacketStatus& r);
    bool onTennisTickEnd(BallStatus& ball_status, ScoreBoard& score_board, bool& change_turn);
    void RenderingTickBegin(unsigned const& player_id,
                RacketStatus& racket_status, BallStatus& ball_status,
                ScoreBoard& score_board, unsigned& turn_owner);

    bool is_connected() {return this->connected;}

private:
    std::unique_ptr<libnetwork::VirtualTennis::Stub> stub_;
    std::string player_name;
    unsigned player_id;
    std::string server_ip;
    unsigned server_port;
    bool connected;
};

#endif