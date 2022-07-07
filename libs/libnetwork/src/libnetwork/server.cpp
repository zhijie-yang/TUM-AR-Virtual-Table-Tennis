#include <grpc++/grpc++.h>
#include <grpc++/ext/proto_server_reflection_plugin.h>
#include <grpc++/health_check_service_interface.h>
#include "network.pb.h"
#include "network.grpc.pb.h"
#include "libframework/include/ball_status.h"
#include "libframework/include/racket_status.h"
#include "libframework/include/infos.h"

#include <sstream>
#include <utility>


using grpc::Server;
using grpc::ServerBuilder;

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
    ScoreBoard score_board;
    bool game_running = false;

public:
    inline static TennisServerManager* get_instance();

    // getters
    inline std::pair<std::string, std::string> get_client_ip() {return this->client_ip;}
    // inline std::pair<unsigned, unsigned> getClientPort() {return this->client_port;}
    inline std::pair<unsigned, unsigned> get_client_id() {return this->client_id;}
    inline std::pair<std::string, std::string> get_client_name() {return this->client_name;}
    inline unsigned get_turn_owner() {
        return (!this->turn_owner) ? this->client_id.first : this->client_id.second;
        }
    inline BallStatus get_ball_status() {return this->ball_status;}
    inline RacketStatus get_racket_status() {return this->racket_status;}
    inline ScoreBoard get_score_board() {return this->score_board;}
    inline std::pair<bool, bool> get_client_inited() {return this->client_inited;}

    // setters
    inline void set_ball_status(BallStatus const& ball_status) {
        this->ball_status = ball_status;
    }
    inline void set_racket_status(RacketStatus const &racket_status) {
        this->racket_status = racket_status;
    }
    inline void set_score_board(ScoreBoard const &score_board) {
        this->score_board = score_board;
    }

    // game locigs
    bool start_game() {
        if (this->game_running || !this->client_inited.first || !this->client_inited.second) {return false;}
        this->game_running = true;
        return true;
    }
    bool reg_player(std::string client_ip, unsigned client_port,
              unsigned client_player_id, std::string client_player_name);
    // getters
    inline void change_turn() {this->turn_owner = !this->turn_owner;}
    inline void dereg(bool pos) {if (!pos) this->client_inited.first = false; else this->client_inited.second = false;}
    inline void dereg() {this->client_inited = {false, false};}
};

/* Null, because instance will be initialized on demand. */
TennisServerManager* TennisServerManager::instance = 0;

TennisServerManager* TennisServerManager::get_instance()
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
    grpc::Status SendBallStatus(grpc::ServerContext* context, const libnetwork::BallStatus* request,
                libnetwork::GeneralResponse* response) override {
                    auto manager = TennisServerManager::get_instance();
                    BallStatus status;
                    BallStatus::fromProto(*request, status);
                    manager->set_ball_status(status);
                    response->set_result(true);
                    return grpc::Status::OK;
                }

    grpc::Status GetBallStatus(grpc::ServerContext* context, const libnetwork::BallStatusRequest* request,
                libnetwork::BallStatus* response) override {
                    auto manager = TennisServerManager::get_instance();
                    libnetwork::BallStatus _response;
                    manager->get_ball_status().toProto(_response);
                    *response = _response;
                    return grpc::Status::OK;
                }

    grpc::Status SendRacketStatus(grpc::ServerContext* context, const libnetwork::RacketStatus* request,
                libnetwork::GeneralResponse* response) override {
                    auto manager = TennisServerManager::get_instance();
                    RacketStatus status;
                    RacketStatus::fromProto(*request, status);
                    manager->set_racket_status(status);
                    response->set_result(true);
                    return grpc::Status::OK;
                }

    grpc::Status GetRacketStatus(grpc::ServerContext* context, const libnetwork::RacketStatusRequest* request,
                libnetwork::RacketStatus* response) override {
                    auto manager = TennisServerManager::get_instance();
                    libnetwork::RacketStatus _response;
                    manager->get_racket_status().toProto(_response);
                    *response = _response;
                    return grpc::Status::OK;
                }

    grpc::Status SendScoreBoard(grpc::ServerContext* context, const libnetwork::ScoreBoard* request,
                libnetwork::GeneralResponse* response) override {
                    auto manager = TennisServerManager::get_instance();
                    ScoreBoard score_board;
                    ScoreBoard::fromProto(*request, score_board);
                    manager->set_score_board(score_board);
                    response->set_result(true);
                    return grpc::Status::OK;
                }

    grpc::Status GetScoreBoard(grpc::ServerContext* context, const libnetwork::ScoreBoardRequest* request,
                libnetwork::ScoreBoard* response) override {
                    auto manager = TennisServerManager::get_instance();
                    libnetwork::ScoreBoard _response;
                    manager->get_score_board().toProto(_response);
                    *response = _response;
                    return grpc::Status::OK;
                }
    
    grpc::Status ChangeTurn(grpc::ServerContext* context, const libnetwork::ChangeTurnRequest* request,
                libnetwork::GeneralResponse* response) override {
                    auto manager = TennisServerManager::get_instance();
                    manager->change_turn();
                    response->set_result(true);
                    std::stringstream fmt_str;
                    fmt_str << manager->get_turn_owner();
                    response->set_detail("Changed turn owner to " + fmt_str.str());
                    return grpc::Status::OK;
                }

    grpc::Status GetCurrentTurn(grpc::ServerContext* context, const libnetwork::CurrentTurnRequest* request,
                libnetwork::CurrentTurnResponse* response) override {
                    auto manager = TennisServerManager::get_instance();
                    response->set_turn_owner(manager->get_turn_owner());
                    return grpc::Status::OK;
                }

        // TODO @zhijie-yang
    grpc::Status StartNewRound(grpc::ServerContext* context, const libnetwork::StartNewRoundRequest* request,
                libnetwork::StartNewRoundResponse* response) override {
                    auto manager = TennisServerManager::get_instance();
                    return grpc::Status::OK;
                }

    // TODO @zhijie-yang
    grpc::Status EndRound(grpc::ServerContext* context, const libnetwork::RoundEndingRequest* request,
                libnetwork::RoundEndingResponse* response) override {
                    auto manager = TennisServerManager::get_instance();
                    // manager->dereg();
                    return grpc::Status::OK;
                }

    grpc::Status ConnectServer(grpc::ServerContext* context, const libnetwork::ClientConnectionRequest* request,
                libnetwork::GeneralResponse* response) override {
                    std::string peer_info = context->peer();
                    std::cout << "User connection with peer info: " << peer_info << std::endl;
                    auto manager = TennisServerManager::get_instance();
                    // TODO @zhijie-yang: get client ip address and port from context
                    std::string delimiter = ":";
                    std::string _s = peer_info;
                    size_t pos = _s.find(delimiter);
                    std::string protocol = _s.substr(0, pos);
                    _s = _s.substr(pos+1, _s.size());
                    pos = 0;
                    std::string ip_addr = "";
                    while ((pos = _s.find(delimiter)) <= _s.size()-1) {
                        ip_addr += _s.substr(0, pos+1);
                        _s = _s.substr(pos+1, _s.size());
                    }
                    ip_addr = ip_addr.substr(0, ip_addr.size()-1);
                    std::string port_str = _s;
                    // std::string ip_addr = peer_info.substr(0, pos);
                    std::cout << "IP: " << ip_addr << std::endl;
                    std::cout << "Port: " << port_str << std::endl;
                    auto success = manager->reg_player(ip_addr, 0, request->player_info().player_id(), request->player_info().player_name());
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

    grpc::Status DisconnectServer(grpc::ServerContext* context, const libnetwork::ClientDisconnectRequest* request,
                libnetwork::GeneralResponse* response) override {
                    std::string peer_info = context->peer();
                    std::string delimiter = ":";
                    std::string _s = peer_info;
                    size_t pos = _s.find(delimiter);
                    std::string protocol = _s.substr(0, pos);
                    _s = _s.substr(pos+1, _s.size());
                    pos = 0;
                    std::string ip_addr = "";
                    while ((pos = _s.find(delimiter)) <= _s.size()-1) {
                        ip_addr += _s.substr(0, pos+1);
                        _s = _s.substr(pos+1, _s.size());
                    }
                    ip_addr = ip_addr.substr(0, ip_addr.size()-1);
                    std::string port_str = _s;
                    std::cout << "IP: " << ip_addr << std::endl;
                    std::cout << "Port: " << port_str << std::endl;
                    auto manager = TennisServerManager::get_instance();
                    if (manager->get_client_inited().first && ip_addr == manager->get_client_ip().first) {
                        manager->dereg(static_cast<bool>(0));
                    } else if (manager->get_client_inited().second && ip_addr == manager->get_client_ip().second) {
                        manager->dereg(static_cast<bool>(1));
                    } else {
                        std::cerr << "Player not registered." << std::endl;
                        return grpc::Status(grpc::StatusCode::CANCELLED, "Player not registered.");
                    }
                    // TODO @zhijie-yang: get client ip address and port from context
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