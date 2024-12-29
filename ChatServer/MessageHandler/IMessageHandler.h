#pragma once

#include <muduo/net/TcpConnection.h>
#include <string>
#include <memory>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <string>
#include "../../const.h"
#include "../UserMgr/UserMgr.h"
#include "../../RedisMgr/RedisMgr.h"
#include "../../MysqlMgr/MysqlMgr.h"
#include "../../StatusGrpcClient/StatusGrpcClient.h"

using namespace Json;
class ChatServer;

// 检验token是否合规
bool checkToken(int uid, const std::string& token);


class IMessageHandler 
{
public:
    IMessageHandler(ChatServer* server);

    virtual ~IMessageHandler() = default;

    // 处理消息的接口
    virtual void handle(const muduo::net::TcpConnectionPtr& conn, const std::string& data) = 0;
    
    // 解析数据
    bool parseJson(Json::Value& root, const std::string& body_str, Json::Value& srcRoot){
        Json::Reader reader;
        bool parse_success = reader.parse(body_str, srcRoot);
        if(!parse_success)
        {
            root["code"] = ErrorCodes::ERR_INVALID_PARAMS;
            return false;
        }
        return true;
    }
protected:
    void sendMsg(const muduo::net::TcpConnectionPtr& conn, ReqId msgId, const std::string& msg);
private:
    ChatServer* m_server;
};
