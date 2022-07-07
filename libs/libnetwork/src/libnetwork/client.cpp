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


GeneralResponse VirtualTennisNetworkClient::connectServer(ClientConnectionRequest& _request) {
    grpc::ClientContext context;
    // serialize
    libnetwork::ClientConnectionRequest request;
    _request.toProto(request);
    // handling the request and reponse
    libnetwork::GeneralResponse response;
    grpc::Status status = stub_->ConnectServer(&context, request, &response);
    // deserialize
    // auto _response = new GeneralResponse();
    GeneralResponse _response;
    if (status.ok()) {
        GeneralResponse::fromProto(response, _response);
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

RacketStatus VirtualTennisNetworkClient::getRacketStatus() {
    grpc::ClientContext context;
    libnetwork::RacketStatusRequest request;
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

GeneralResponse VirtualTennisNetworkClient::changeTurn(ChangeTurnRequest& _request) {
    grpc::ClientContext context;
    libnetwork::ChangeTurnRequest request;
    libnetwork::GeneralResponse response;
    _request.toProto(request);
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


int main(int argc, char** argv) {
    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint specified by
    // the argument "--target=" which is the only expected argument.
    // We indicate that the channel isn't authenticated (use of
    // InsecureChannelCredentials()).
    std::string target_str;
    std::string arg_str("--ip");
    // std::string arg2_str("--port");
    if (argc > 1) {
        std::string arg_val = argv[1];
        size_t start_pos = arg_val.find(arg_str);
        if (start_pos != std::string::npos) {
            start_pos += arg_str.size();
            if (arg_val[start_pos] == '=') {
                target_str = arg_val.substr(start_pos + 1);
            } else {
                std::cout << "The only correct argument syntax is --ip="
                      << std::endl;
                return 0;
            }
        } else {
            std::cout << "The only acceptable argument is --target=" << std::endl;
            return 0;
        }
    } else {
      target_str = "localhost:50051";
    }
    VirtualTennisNetworkClient tennis_client(
    grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
    auto player_info = PlayerInfo(1, "Zhijie Yang");
    auto connection_req = ClientConnectionRequest(target_str, player_info, 50051);
    GeneralResponse connection_res = tennis_client.connectServer(connection_req);
    return 0;
}