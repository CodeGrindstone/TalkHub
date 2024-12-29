#pragma once

#include "IMessageHandler.h"

class LoginHandler : public IMessageHandler {
public:

    LoginHandler(ChatServer* server) : IMessageHandler(server){}

    void handle(const muduo::net::TcpConnectionPtr& conn, const std::string& data) override 
    {
        // 解析数据
        Json::Value root;
        Json::Value src_root;

        Defer defer([&conn, &root, this]{
            std::string jsonstr = root.toStyledString();
            sendMsg(conn, ID_CHAT_LOGIN_RSP, jsonstr);
        });

        if(!parseJson(root, data, src_root))
            return;

        int uid = src_root["uid"].asInt();
        std::string token = src_root["token"].asString();
        std::string chatserver = src_root["chatserver"].asString();

        // 检验token
        if(!checkToken(uid, token))
        {
            root["code"] = ErrorCodes::ERR_TOKEN_INVALID;
            return;
        }

        /*
            通知状态服务器
                1. ChatServer的登陆数量+1
                2. 为用户设置登录ip server的名字=>hset user:<user_id> host <ChatServer_name>
        */
        StatusGrpcClient::GetInstance()->Login(root, uid, chatserver);

        //获取好友申请列表

        //获取好友列表

        //uid和session绑定管理,
        UserMgr::GetInstance()->SetUserSession(conn, uid);
    }
};