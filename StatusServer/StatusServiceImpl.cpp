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
        auto chatserver_name = getNextChatServer();
        reply->set_chatserver(chatserver_name);
        reply->set_host(servers_[chatserver_name].host);
        reply->set_port(servers_[chatserver_name].port);

        std::unordered_map<std::string, std::string> fields;
        std::string token = generate_unique_string();
        fields["token"] = token;

        RedisMgr::GetInstance()->hsetUserInfo(std::to_string(request->uid()), fields);

        reply->set_token(token);
        reply->set_error(ErrorCodes::SUCCESS);
        
        return Status::OK;
    }catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return Status::CANCELLED;
    }
}

std::string StatusServiceImpl::getNextChatServer() 
{
    std::lock_guard<std::mutex> guard(m_mutex);
    int min_server_count = INT_MAX;
    std::string min_connect_server_name;

    for(auto& server : servers_)
    {
        std::string server_name = server.first;
        int connect_count = RedisMgr::GetInstance()->get_connection_count(server_name);
        if(connect_count < min_server_count)
        {
            min_server_count = connect_count;
            min_connect_server_name = server_name;
        }
    }

    return min_connect_server_name;
}

Status StatusServiceImpl::Login(ServerContext *context, LoginReq const *request, LoginRsp *reply) 
{
    try{
        const unsigned int& uid = request->uid();
        const std::string& chatserver = request->chatserver();

        // 在redis中保存登陆状态
        std::unordered_map<std::string, std::string> fields;
        fields["status"] = "true";
        fields["chatserver"] = chatserver;
        RedisMgr::GetInstance()->hsetUserInfo(std::to_string(uid), fields);
        RedisMgr::GetInstance()->update_connection_count(chatserver, 1);
        reply->set_error(ErrorCodes::SUCCESS);
        
        return Status::OK;
    }catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return Status::CANCELLED;
    }
}

StatusServiceImpl::StatusServiceImpl()
{
    auto& cfg = ConfigMgr::getInstance();
    ChatServer server;
    std::vector<std::string> server_names;
    auto server_list = cfg["ChatServers"]["Name"];
    std::stringstream ss(server_list);
    std::string word;

    while (std::getline(ss, word, ',')) {
        if(!cfg.isContainSection(word)){
            std::cout << "错误," << word << "无配置文件" << std::endl;
            exit(0);
        }
        server_names.push_back(word);
    }

    for(auto& server_name : server_names)
    {
        server.host = cfg[server_name]["Host"];
        server.port = cfg[server_name]["Port"];
        servers_[server_name] = server;        
    }
}