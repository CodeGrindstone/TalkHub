//
// Created by 31435 on 2024/9/24.
//

#ifndef CHATXTSERVER_STATUSSERVICEIMPL_H
#define CHATXTSERVER_STATUSSERVICEIMPL_H


#include <grpcpp/grpcpp.h>
#include <unordered_map>
#include "../protoc/message.grpc.pb.h"
#include "../const.h"
#include "../RedisMgr/RedisMgr.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;
using message::LoginReq;
using message::LoginRsp;

struct ChatServer 
{
    std::string host;
    std::string port;
};

class StatusServiceImpl final : public StatusService::Service
{
public:
    StatusServiceImpl();
    Status GetChatServer(ServerContext* context, const GetChatServerReq* request,
                         GetChatServerRsp* reply) override;

    Status Login(ServerContext* context, const LoginReq* request, LoginRsp* reply) override;


private:
    std::string getNextChatServer();
    std::unordered_map<std::string, ChatServer> servers_;
    std::mutex m_mutex;
};


#endif //CHATXTSERVER_STATUSSERVICEIMPL_H
