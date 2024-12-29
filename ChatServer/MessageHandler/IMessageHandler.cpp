#include "IMessageHandler.h"
#include "../ChatServer/ChatServer.h"

bool checkToken(int uid, const std::string& token)
{
    std::string redis_token;
    if(RedisMgr::GetInstance()->hgetUserToken(std::to_string(uid), redis_token) && !redis_token.empty())
    { 
        return redis_token == token;
    }
    return false;
}  

IMessageHandler::IMessageHandler(ChatServer* server) : m_server(server){}

void IMessageHandler::sendMsg(const muduo::net::TcpConnectionPtr& conn, ReqId msgId, const std::string& msg)
{
    m_server->sendMsg(conn, msgId, msg);
}