#include "IMessageHandler.h"

class ChatTextMsgHandler : public IMessageHandler {
public:

    ChatTextMsgHandler(ChatServer* server) : IMessageHandler(server){}

    void handle(const muduo::net::TcpConnectionPtr& conn, const std::string& data) override 
    {
        // 解析数据
        Json::Value root;
        Json::Value src_root;

        Defer defer([&conn, &root, this]{
            std::string jsonstr = root.toStyledString();
            sendMsg(conn, ID_TEXT_CHAT_MSG_RSP, jsonstr);
        });

        if(!parseJson(root, data, src_root))
            return;

        int from_uid = src_root["from_uid"].asInt();
        std::string token = src_root["token"].asString();
        // 检验token
        if(!checkToken(from_uid, token))
        {
            root["code"] = ErrorCodes::ERR_TOKEN_INVALID;
            return;
        }
        
        int to_uid = src_root["to_uid"].asInt();
        std::string type = src_root["type"].asString();
        std::string message = src_root["content"].asString();

        if(strcmp(type.c_str(), "type"))
        {
            if(!MysqlMgr::GetInstance()->SavePrivateMes(root, from_uid, to_uid, message))
            {
                return;
            }
        }

        // 查询redis，查找peer_uid对应的ChatServer
        // TODO...这里假设一定能找到ChatServer
        std::string peer_server_name;
        int errorCode;
        if(RedisMgr::GetInstance()->search_uid_chatserver(to_uid, peer_server_name, errorCode) && errorCode == 0)
        {
            // 成功找到对端所在ChatServer
            if(peer_server_name == server_name)
            {
                // 对端在本服务器
                auto session = UserMgr::GetInstance()->GetSession(to_uid);
                if(session == nullptr)
                    return;
                Json::Value notify;

                notify["error"] = ErrorCodes::SUCCESS;
                notify["fromuid"] = from_uid;
                notify["touid"] = to_uid;    // 请求方
                notify["type"] = type;
                notify["content"] = message;
          
                std::string jsonstr = notify.toStyledString();
                sendMsg(session, ID_NOTIFY_TEXT_CHAT_MSG_REQ, jsonstr);

                return;
            }

            // 对端在其他服务器
        }
    }
};