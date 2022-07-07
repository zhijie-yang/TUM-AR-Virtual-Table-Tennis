#ifndef LIBNETWORK_CLIENT_H
#define LIBNETWORK_CLIENT_H

#include <grpc++/grpc++.h>

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

    GeneralResponse connectServer(ClientConnectionRequest& _request);
    GeneralResponse disconnectServer();
    GeneralResponse sendBallStatus(BallStatus& _request);
    BallStatus getBallStatus();
    GeneralResponse sendRacketStatus(RacketStatus& _request);
    RacketStatus getRacketStatus();
    GeneralResponse sendScoreBoard(ScoreBoard& _request);
    ScoreBoard getScoreBoard();
    GeneralResponse changeTurn(ChangeTurnRequest& _request);
    unsigned getCurrentTurn();
    StartNewRoundResponse startNewRound(StartNewRoundRequest& _request);
    RoundEndingResponse endRound(RoundEndingRequest& _request);

private:
    std::unique_ptr<libnetwork::VirtualTennis::Stub> stub_;
};

#endif