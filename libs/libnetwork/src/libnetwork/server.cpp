#include <grpc++/grpc++.h>
#include <grpc++/ext/proto_server_reflection_plugin.h>
#include <grpc++/health_check_service_interface.h>
#include "network.pb.h"
#include "network.grpc.pb.h"
#include "libframework/include/ball_status.h"
#include "libframework/include/racket_status.h"

#include <sstream>


using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;

class TennisServerManager {
public:
    ~TennisServerManager();

private:
    static TennisServerManager* instance;
    TennisServerManager(){}

    std::pair<bool, bool> client_inited = {false, false};
    std::pair<std::string, std::string> client_ip;
    // std::pair<unsigned, unsigned> client_port;
    std::pair<unsigned, unsigned> client_id;
    std::pair<std::string, std::string> client_name;

    // game logics
    bool turn_owner = false; // 0/false for player A, 1/true for player B
    BallStatus ball_status;
    RacketStatus racket_status;

public:
    inline static TennisServerManager* getInstance();
    inline bool reg_player(std::string client_ip, unsigned client_port,
              unsigned client_player_id, std::string client_player_name);
    // getters
    inline std::pair<std::string, std::string> getClientIP() {return this->client_ip;}
    // inline std::pair<unsigned, unsigned> getClientPort() {return this->client_port;}
    inline std::pair<unsigned, unsigned> getClientID() {return this->client_id;}
    inline std::pair<std::string, std::string> getClientName() {return this->client_name;}
    inline bool getTurnOwner() {return this->turn_owner;}
    inline void changeTurn() {this->turn_owner = !this->turn_owner;}
    inline void dereg(bool pos) {if (!pos) this->client_inited.first = false; else this->client_inited.second = false;}
    inline void dereg() {this->client_inited = {false, false};}
    inline BallStatus get_ball_status() {return this->ball_status;}
    inline RacketStatus get_racket_status() {return this->racket_status;}

    // setters
    inline void set_ball_status(BallStatus const& ball_status) {
        this->ball_status = std::move(ball_status);
    }
    inline void set_racket_status(RacketStatus const &racket_status) {
        this->racket_status = std::move(racket_status);
    }
};

/* Null, because instance will be initialized on demand. */
TennisServerManager* TennisServerManager::instance = 0;

TennisServerManager* TennisServerManager::getInstance()
{
    if (instance == 0)
    {
        instance = new TennisServerManager();
    }

    return instance;
}

bool TennisServerManager::reg_player(std::string client_ip, unsigned client_port,
              unsigned client_player_id, std::string client_player_name) {
                if (!client_inited.first && !client_inited.second) {
                    this->client_ip.first = client_ip;
                    // this->client_port.first = client_port;
                    this->client_id.first = client_player_id;
                    this->client_name.first = client_player_name;
                    client_inited.first = true;
                } else if (client_inited.first && !client_inited.second) {
                    this->client_ip.second = client_ip;
                    // this->client_port.second = client_port;
                    this->client_id.second = client_player_id;
                    this->client_name.second = client_player_name;
                    client_inited.second = true;
                } else {
                    std::cerr << "Can not register more than two clients!" << std::endl;
                    return false;
                }
                return true;
              }

class ConnectionServiceImpl final : public libnetwork::VirtualTennis::Service {
    grpc::Status ConnectServer(ServerContext* context, const libnetwork::ClientConnectionRequest* request,
                libnetwork::GeneralResponse* response) override {
                    std::string peer_info = context->peer();
                    std::cout << "User connection with peer info: " << peer_info << std::endl;
                    auto manager = TennisServerManager::getInstance();
                    // TODO @zhijie-yang: get client ip address and port from context
                    auto success = manager->reg_player("0.0.0.0", 50051, request->player_info().player_id(), request->player_info().player_name());
                    std::cout << "Player Name: " << request->player_info().player_name() << std::endl;
                    response->set_result(success);
                    if (success) {
                        std::stringstream fmt_str;
                        fmt_str<< "Player " << request->player_info().player_name() << "regiester with player ID "
                                            << request->player_info().player_id();
                        response->set_detail(fmt_str.str());
                        return grpc::Status::OK;
                    } else {
                        std::stringstream fmt_str;
                        fmt_str<< "Failed to register";
                        response->set_detail(fmt_str.str());
                        return grpc::Status(grpc::StatusCode::CANCELLED, "Can not register more than two clients!");
                    }
                }
                  

    grpc::Status ChangeTurn(ServerContext* context, const libnetwork::ChangeTurnRequest* request,
                libnetwork::GeneralResponse* response) override {
                    auto manager = TennisServerManager::getInstance();
                    manager->changeTurn();
                    response->set_result(true);
                    std::stringstream fmt_str;
                    fmt_str << manager->getTurnOwner();
                    response->set_detail("Changed turn owner to " + fmt_str.str());
                    return grpc::Status::OK;
                }

    // TODO @zhijie-yang
    grpc::Status StartNewRound(ServerContext* context, const libnetwork::StartNewRoundRequest* request,
                libnetwork::StartNewRoundResponse* response) override {
                    auto manager = TennisServerManager::getInstance();
                    return grpc::Status::OK;
                }

    // TODO @zhijie-yang
    grpc::Status EndRound(ServerContext* context, const libnetwork::RoundEndingRequest* request,
                libnetwork::RoundEndingResponse* response) override {
                    auto manager = TennisServerManager::getInstance();
                    manager->dereg();
                    return grpc::Status::OK;
                }

    grpc::Status GetBallStatus(ServerContext* context, const libnetwork::BallStatusRequest* request,
                libnetwork::BallStatus* response) override {
                    auto manager = TennisServerManager::getInstance();
                    libnetwork::BallStatus _response;
                    manager->get_ball_status().toProto(_response);
                    *response = _response;
                    return grpc::Status::OK;
                }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    ConnectionServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();

    return 0;
}