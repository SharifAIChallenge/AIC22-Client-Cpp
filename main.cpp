#include "client/client.h"
#include "yaml-cpp/yaml.h"

int main(int argc, char** argv) {
    if(argc != 2) {
        throw std::runtime_error("you should specify yml file path as command line argument!");
    }
    std::string config_file_path = argv[1];
    YAML::Node config = YAML::LoadFile(config_file_path);
    const auto token = config["token"].as<std::string>();
    const auto host = config["grpc"]["server"].as<std::string>();
    const auto port = config["grpc"]["port"].as<std::string>();
    const auto address = host + ":" + port;
    Client::handle_client(token, address);
    return 0;
}
