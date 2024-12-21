//
// Created by 31435 on 2024/9/24.
//

#include "StatusServiceImpl.h"
#include "../Config/ConfigMgr.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <unordered_map>

std::string generate_unique_string() {
    // 创建UUID对象
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    // 将UUID转换为字符串
    std::string unique_string = to_string(uuid);
    return unique_string;
}

Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* reply)
{
    try{
        _server_index = (_server_index++) % (_servers.size());
        auto &server = _servers[_server_index];
        reply->set_host(server.host);
        reply->set_port(server.port);

        std::unordered_map<std::string, std::string> fields;
        fields["token"] = generate_unique_string();

        RedisMgr::GetInstance()->hsetUserInfo(std::to_string(request->uid()), fields);

        reply->set_error(ErrorCodes::SUCCESS);
        
        return Status::OK;
    }catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return Status::CANCELLED;
    }
}

// Status StatusServiceImpl::Login(ServerContext *context, LoginReq const *request,
//                                 LoginRsp *reply) 
// {
//     auto uid = request->uid();
//     auto token = request->token();
 
//     std::lock_guard<std::mutex> mutex(m_mutex);
//     auto iter = _uid_tokens.find(uid);
//     if(iter == _uid_tokens.end()){
//         reply->set_error(ErrorCodes::UidInvalid);   
//         return Status::OK;
//     }
//     if(token != iter->second){
//         reply->set_error(ErrorCodes::TokenInvalid);
//         return Status::OK;
//     }

//     reply->set_error(ErrorCodes::Success);
//     reply->set_uid(uid);
//     reply->set_token(token);
//     return Status::OK;
// }

StatusServiceImpl::StatusServiceImpl():_server_index(0)
{
    auto& cfg = ConfigMgr::getInstance();
    ChatServer server;
    server.port = cfg["ChatServer1"]["Port"];
    server.host = cfg["ChatServer1"]["Host"];
    _servers.push_back(server);
    server.port = cfg["ChatServer2"]["Port"];
    server.host = cfg["ChatServer2"]["Host"];
    _servers.push_back(server);
}