#include "./ChatServer/ChatServer.h"
#include "../Config/ConfigMgr.h"
#include "../const.h"
#include "./ChatServiceImpl/ChatServiceImpl.h"
#include "../RedisMgr/RedisMgr.h"
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/base/Logging.h>
#include <stdlib.h>
#include <thread>


std::string server_name;
std::vector<std::string> peer_server_name; 

int main(int argc, char* argv[]) {

    if(argc != 2){
        std::cout << "错误，参数错误." << std::endl;
        return 0;
    }

    char* server_name_ = argv[1];
    auto& config = ConfigMgr::getInstance();
    if(!config.isContainSection(server_name_)){
        std::cout << "错误, 此服务" << server_name_ << "无配置文件" << std::endl;
        return 0;
    }

    auto peer_server_list = config["ChatServers"]["Name"];
    std::stringstream ss(peer_server_list);
    std::string word;
    while (std::getline(ss, word, ',')) {
        if(!config.isContainSection(word)){
            std::cout << "错误, 对端" << word << "无配置文件" << std::endl;
            return 0;
        }
        peer_server_name.push_back(word);
    }

    server_name = std::string(server_name_);
    auto ip = config[server_name]["Host"];
    auto port = config[server_name]["Port"];
    auto RPCPort = config[server_name]["RPCPort"];

    try
    {
        RedisMgr::GetInstance()->hset(LOGIN_COUNT, server_name, "0");
        //定义一个GrpcServer
        std::string server_address(ip + ":" + RPCPort);
        std::cout << server_address;
        ChatServiceImpl service;
        grpc::ServerBuilder builder;
        // 监听端口和添加服务
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        // 构建并启动gRPC服务器
        std::unique_ptr<grpc::Server> grpc_server(builder.BuildAndStart());
        std::cout << "RPC Server listening on " << server_address << std::endl;
        //单独启动一个线程处理grpc服务
        std::thread  grpc_server_thread([&grpc_server]() {
            grpc_server->Wait();
        });

        muduo::net::InetAddress listenAddr(atoi(port.c_str()));
        muduo::net::EventLoop loop;
        ChatServer server(&loop, listenAddr);
        server.start();

        loop.loop();
        // RedisMgr::GetInstance()->HDel(LOGIN_COUNT, server_name);
        // RedisMgr::GetInstance()->Close();
        grpc_server_thread.join();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}