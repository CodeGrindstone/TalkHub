#pragma once

#include "IMessageHandler.h"


class SearchUserHandler: public IMessageHandler {
public:

    SearchUserHandler(ChatServer* server) : IMessageHandler(server){}

    void handle(const muduo::net::TcpConnectionPtr& conn, const std::string& data) override 
    {
        // 解析数据
        Json::Value root;
        Json::Value src_root;

        Defer defer([&conn, &root, this]{
            std::string jsonstr = root.toStyledString();
            sendMsg(conn, ID_SEARCH_USER_RSP, jsonstr);
        });

        if(!parseJson(root, data, src_root))
            return;

        int uid = src_root["uid"].asInt();
        std::string token = src_root["token"].asString();
        // 检验token
        if(!checkToken(uid, token))
        {
            root["code"] = ErrorCodes::ERR_TOKEN_INVALID;
            return;
        }

        std::string type = src_root["type"].asString();

        if(type == "uid")
        {
            // uid查找
            if (!src_root["searchInfo"].isInt()) 
            {
                root["code"] = ErrorCodes::ERR_INVALID_PARAMS;
                return;
            }
            int search_id = src_root["searchInfo"].asInt();

            if(!MysqlMgr::GetInstance()->SearchUserByUid(root, search_id))
            {
                root["code"] = ErrorCodes::ERR_INVALID_PARAMS;
                return;
            }
            
        }
        else
        {
            // email查找
        }

    }
};