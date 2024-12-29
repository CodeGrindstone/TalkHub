//
// Created by 31435 on 2024/9/24.
//

#include "StatusGrpcClient.h"

GetChatServerRsp StatusGrpcClient::GetChatServer(Json::Value& root, int uid)
{
    ClientContext context;
    GetChatServerRsp reply;
    GetChatServerReq request;
    request.set_uid(uid);
    auto stub = pool_->getConnection();
    Status status = stub->GetChatServer(&context, request, &reply);

    Defer defer([&stub, this]() {
        pool_->returnConnection(std::move(stub));
    });

    if (status.ok()) {
        root["host"] = reply.host();
        root["port"] = reply.port();
        root["chatserver"] = reply.chatserver();
        root["token"] = reply.token();
        return reply;
    }
    else {
        reply.set_error(ErrorCodes::ERR_DEPENDENCY_FAILURE);
        // 获取错误码
        grpc::StatusCode errorCode = status.error_code();
        // 获取错误消息
        std::string errorMessage = status.error_message();
        // 获取错误详情
        std::string errorDetails = status.error_details();

        std::cout << "RPC failed with error code: " << errorCode << std::endl;
        std::cout << "Error message: " << errorMessage << std::endl;

        // 如果有错误详情，输出详细信息
        if (!errorDetails.empty()) {
            std::cout << "Error details: " << errorDetails << std::endl;
        } 
        return reply;
    }
}

LoginRsp StatusGrpcClient::Login(Json::Value& root, int uid, const std::string& chatserver)
{
    ClientContext context;
    LoginRsp reply;
    LoginReq request;
    request.set_uid(uid);
    request.set_chatserver(chatserver);

    auto stub = pool_->getConnection();
    Defer defer([&stub, this]() {
        pool_->returnConnection(std::move(stub));
    });

    Status status = stub->Login(&context, request, &reply);

    if (status.ok())
    {
        root["code"] = ErrorCodes::SUCCESS;
        return reply;
    }
    else {
        reply.set_error(ErrorCodes::ERR_DEPENDENCY_FAILURE);
        // 获取错误码
        grpc::StatusCode errorCode = status.error_code();
        // 获取错误消息
        std::string errorMessage = status.error_message();
        // 获取错误详情
        std::string errorDetails = status.error_details();

        std::cout << "RPC failed with error code: " << errorCode << std::endl;
        std::cout << "Error message: " << errorMessage << std::endl;

        // 如果有错误详情，输出详细信息
        if (!errorDetails.empty()) {
            std::cout << "Error details: " << errorDetails << std::endl;
        } 
        return reply;
    }
}

StatusGrpcClient::StatusGrpcClient() {
    auto& gCfgMgr = ConfigMgr::getInstance();
    std::string host = gCfgMgr["StatusServer"]["Host"];
    std::string port = gCfgMgr["StatusServer"]["Port"];
    pool_.reset(new StatusConPool(5, host, port));
}
 