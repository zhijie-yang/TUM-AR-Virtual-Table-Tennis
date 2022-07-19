#include <grpc++/grpc++.h>

#include "libnetwork/include/client.h"
#include "libframework/include/infos.h"
#include "libframework/include/responses.h"
#include "libframework/include/requests.h"
#include "libframework/include/geometry.h"
#include "libframework/include/ball_status.h"
#include "libframework/include/racket_status.h"
#include "libnetwork/proto_src/network.pb.h"
#include "libnetwork/proto_src/network.grpc.pb.h"


ClientConnectionResponse VirtualTennisNetworkClient::connectServer() {
    grpc::ClientContext context;
    // serialize
    libnetwork::ClientConnectionRequest request;
    request.set_player_name(this->player_name);    
    // handling the request and reponse
    libnetwork::ClientConnectionResponse response;
    grpc::Status status = stub_->ConnectServer(&context, request, &response);
    // deserialize
    // auto _response = new GeneralResponse();
    ClientConnectionResponse _response;
    if (status.ok()) {
        ClientConnectionResponse::fromProto(response, _response);
        this->player_id = _response.get_player_info().get_player_id();
        this->connected = true;
    } else {
        std::cout << status.error_message() << std::endl;
    }
    return _response;
}

GeneralResponse VirtualTennisNetworkClient::disconnectServer() {
    grpc::ClientContext context;
    libnetwork::ClientDisconnectRequest request;
    libnetwork::GeneralResponse response;
    grpc::Status status = stub_->DisconnectServer(&context, request, &response);
    GeneralResponse _response;
    if (status.ok()) {
        GeneralResponse::fromProto(response, _response);
        std::cout << response.detail() << std::endl;
    } else {
        std::cout << status.error_message() << std::endl;
    }
    return _response;
}

GeneralResponse VirtualTennisNetworkClient::sendBallStatus(BallStatus& _request) {
    grpc::ClientContext context;
    libnetwork::BallStatus request;
    libnetwork::GeneralResponse response;
    _request.toProto(request);
    grpc::Status status = stub_->SendBallStatus(&context, request, &response);
    GeneralResponse _response;
    if (status.ok()) {
        GeneralResponse::fromProto(response, _response);
    } else {
        std::cout << status.error_message() << std::endl;
    }
    return _response;
}

BallStatus VirtualTennisNetworkClient::getBallStatus() {
    grpc::ClientContext context;
    libnetwork::BallStatusRequest request;
    libnetwork::BallStatus response;
    grpc::Status status = stub_->GetBallStatus(&context, request, &response);
    BallStatus _response;
    if (status.ok()) {
        BallStatus::fromProto(response, _response);
    } else {
        std::cout << status.error_message() << std::endl;
    }
    return _response;
}

GeneralResponse VirtualTennisNetworkClient::sendRacketStatus(RacketStatus& _request) {
    grpc::ClientContext context;
    libnetwork::RacketStatus request;
    libnetwork::GeneralResponse response;
    _request.toProto(request);
    grpc::Status status = stub_->SendRacketStatus(&context, request, &response);
    GeneralResponse _response;
    if (status.ok()) {
        GeneralResponse::fromProto(response, _response);
    } else {
        std::cout << status.error_message() << std::endl;
    }
    return _response;
}

RacketStatus VirtualTennisNetworkClient::getRacketStatus(unsigned const& player_id) {
    grpc::ClientContext context;
    libnetwork::RacketStatusRequest request;
    request.set_player_id(player_id);
    libnetwork::RacketStatus response;
    grpc::Status status = stub_->GetRacketStatus(&context, request, &response);
    RacketStatus _response;
    if (status.ok()) {
        RacketStatus::fromProto(response, _response);
    } else {
        std::cout << status.error_message() << std::endl;
    }
    return _response;
}

GeneralResponse VirtualTennisNetworkClient::sendScoreBoard(ScoreBoard& _request) {
    grpc::ClientContext context;
    libnetwork::ScoreBoard request;
    libnetwork::GeneralResponse response;
    _request.toProto(request);
    grpc::Status status = stub_->SendScoreBoard(&context, request, &response);
    GeneralResponse _response;
    if (status.ok()) {
        GeneralResponse::fromProto(response, _response);
    } else {
        std::cout << status.error_message() << std::endl;
    }
    return _response;
}

ScoreBoard VirtualTennisNetworkClient::getScoreBoard() {
    grpc::ClientContext context;
    libnetwork::ScoreBoardRequest request;
    libnetwork::ScoreBoard response;
    grpc::Status status = stub_->GetScoreBoard(&context, request, &response);
    ScoreBoard _response;
    if (status.ok()) {
        ScoreBoard::fromProto(response, _response);
    } else {
        std::cout << status.error_message() << std::endl;
    }
    return _response;
}

GeneralResponse VirtualTennisNetworkClient::changeTurn() {
    grpc::ClientContext context;
    libnetwork::ChangeTurnRequest request;
    libnetwork::GeneralResponse response;
    grpc::Status status = stub_->ChangeTurn(&context, request, &response);
    GeneralResponse _response;
    if (status.ok()) {
        GeneralResponse::fromProto(response, _response);
    } else {
        std::cout << status.error_message() << std::endl;
    }
    return _response;
}

unsigned VirtualTennisNetworkClient::getCurrentTurn() {
    grpc::ClientContext context;
    libnetwork::CurrentTurnRequest request;
    request.set_player_id(this->player_id);
    libnetwork::CurrentTurnResponse response;
    grpc::Status status = stub_->GetCurrentTurn(&context, request, &response);
    unsigned _response;
    if (status.ok()) {
        _response = response.turn_owner();
    } else {
        std::cout << status.error_message() << std::endl;
    }
    return _response;
}

StartNewRoundResponse VirtualTennisNetworkClient::startNewRound(StartNewRoundRequest& _request) {
    grpc::ClientContext context;
    libnetwork::StartNewRoundRequest request;
    _request.toProto(request);
    libnetwork::StartNewRoundResponse response;
    grpc::Status status = stub_->StartNewRound(&context, request, &response);
    StartNewRoundResponse _response;
    if (status.ok()) {
        StartNewRoundResponse::fromProto(response, _response);
    } else {
        std::cout << status.error_message() << std::endl;
    }
    return _response;
}

RoundEndingResponse VirtualTennisNetworkClient::endRound(RoundEndingRequest& _request) {
    grpc::ClientContext context;
    libnetwork::RoundEndingRequest request;
    _request.toProto(request);
    libnetwork::RoundEndingResponse response;
    grpc::Status status = stub_->EndRound(&context, request, &response);
    RoundEndingResponse _response;
    if (status.ok()) {
        RoundEndingResponse::fromProto(response, _response);
    } else {
        std::cout << status.error_message() << std::endl;
    }
    return _response;
}

std::string VirtualTennisNetworkClient::getOpponentName() {
    grpc::ClientContext context;
    // serialize
    libnetwork::PlayerInfo request;
    request.set_player_id(this->player_id);
    // handling the request and reponse
    libnetwork::PlayerInfo response;
    grpc::Status status = stub_->GetOpponentName(&context, request, &response);
    // deserialize
    if (status.ok()) {
        return response.player_name();
    } else {
        std::cout << status.error_message() << std::endl;
        return std::string("_undefined");
    }
}

bool VirtualTennisNetworkClient::onVisionTickEnd(RacketStatus& r) {
    auto status = sendRacketStatus(r);
    return status.get_result();
}

bool VirtualTennisNetworkClient::onTennisTickEnd(BallStatus& ball_status,
                ScoreBoard& score_board, bool& change_turn) {
                    bool status = true;
                    status &= sendBallStatus(ball_status).get_result();
                    status &= sendScoreBoard(score_board).get_result();
//                    if (change_turn) {
//                        status &= changeTurn().get_result();
//                    }
                    return status;
}

void VirtualTennisNetworkClient::RenderingTickBegin(unsigned const& player_id,
                RacketStatus& racket_status, BallStatus& ball_status,
                ScoreBoard& score_board, unsigned& turn_owner) {
                    racket_status = getRacketStatus(player_id);
                    ball_status = getBallStatus();
                    score_board = getScoreBoard();
                    turn_owner = getCurrentTurn();
                }
