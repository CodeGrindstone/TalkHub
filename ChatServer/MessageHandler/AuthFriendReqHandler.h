#include "IMessageHandler.h"

class AuthFriendReqHandler : public IMessageHandler {
public:

    AuthFriendReqHandler(ChatServer* server) : IMessageHandler(server){}

    void handle(const muduo::net::TcpConnectionPtr& conn, const std::string& data) override 
    {
        // 解析数据
        Json::Value root;
        Json::Value src_root;

        Defer defer([&conn, &root, this]{
            std::string jsonstr = root.toStyledString();
            sendMsg(conn, ID_AUTH_FRIEND_RSP, jsonstr);
        });

        if(!parseJson(root, data, src_root))
            return;

        int receiver_uid= src_root["receiver_uid"].asInt();
        std::string token = src_root["token"].asString();
        int requester_uid = src_root["requester_uid"].asInt();

        // 检验token
        if(!checkToken(receiver_uid, token))
        {
            root["code"] = ErrorCodes::ERR_TOKEN_INVALID;
            return;
        }
        
        if(!MysqlMgr::GetInstance()->AuthFriendReq(root, requester_uid, receiver_uid))
        {
            return;
        }

        root["new_friend_uid"] = requester_uid;

        // 查询redis，查找peer_uid对应的ChatServer
        // TODO...这里假设一定能找到ChatServer
        std::string peer_server_name;
        int errorCode;
        if(RedisMgr::GetInstance()->search_uid_chatserver(requester_uid, peer_server_name, errorCode) && errorCode == 0)
        {
            // 成功找到对端所在ChatServer
            if(peer_server_name == server_name)
            {
                // 对端在本服务器
                auto session = UserMgr::GetInstance()->GetSession(requester_uid);
                if(session == nullptr)
                    return;
                Json::Value notify;

                notify["error"] = ErrorCodes::SUCCESS;
                notify["fromuid"] = receiver_uid;
                notify["touid"] = requester_uid;    // 请求方


             
                std::string jsonstr = notify.toStyledString();
                sendMsg(session, ID_NOTIFY_AUTH_FRIEND_REQ, jsonstr);

                return;
            }

            // 对端在其他服务器
        }
    }
};