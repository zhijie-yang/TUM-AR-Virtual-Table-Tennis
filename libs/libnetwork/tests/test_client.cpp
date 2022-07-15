#include <string>
#include "libnetwork/include/client.h"

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
    std::string player_name1 = "Zhijie Yang";
    std::string player_name2 = "Yifeng Li";
    std::string player_name3 = "Siyun Liang";
    ClientConnectionResponse connection_res = tennis_client.connectServer();
    std::cout << connection_res.get_detail() << std::endl;
    connection_res = tennis_client.connectServer();
    std::cout << connection_res.get_detail() << std::endl;
    connection_res = tennis_client.connectServer();
    std::cout << connection_res.get_detail() << std::endl;
    GeneralResponse disconnection_res = tennis_client.disconnectServer();
    std::cout << connection_res.get_detail() << std::endl;
    disconnection_res = tennis_client.disconnectServer();
    std::cout << connection_res.get_detail() << std::endl;
    disconnection_res = tennis_client.disconnectServer();
    std::cout << connection_res.get_detail() << std::endl;
    return 0;
}