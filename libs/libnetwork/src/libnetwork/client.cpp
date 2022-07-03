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


    ClientConnectionResponse* connectServer(ClientConnectionRequest& _request) {
        grpc::ClientContext context;
        // serialize
        libnetwork::ClientConnectionRequest request;
        _request.toProto(request);
        // handling the request and reponse
        libnetwork::ClientConnectionResponse response;
        grpc::Status status = stub_->ConnectServer(&context, request, &response);
        // deserialize
        auto _response = new ClientConnectionResponse();
        ClientConnectionResponse::fromProto(response, *_response);
        return _response;
    }

private:
    std::unique_ptr<libnetwork::VirtualTennis::Stub> stub_;
};

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
    ClientConnectionResponse* connection_res = tennis_client.connectServer(connection_req);
    std::cout << connection_res->get_result() << std::endl;
    delete connection_res;
    return 0;
}