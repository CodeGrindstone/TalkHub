#include "IMessageHandler.h"

class ApplytoAddFriendHandler : public IMessageHandler
{
public:
    ApplytoAddFriendHandler(ChatServer* server) : IMessageHandler(server){}

    void handle(const muduo::net::TcpConnectionPtr& conn, const std::string& data) override 
    {
        // 解析数据
        Json::Value root;
        Json::Value src_root;

        Defer defer([&conn, &root, this]{
            std::string jsonstr = root.toStyledString();
            sendMsg(conn, ID_ADD_FRIEND_RSP, jsonstr);
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

        // 检查对端uid是否为自己
        int peer_uid = src_root["peer_uid"].asInt();
        if(uid == peer_uid)
        {
            root["code"] = ErrorCodes::ERR_REQUEST_SELF;
            return;
        }

        std::string remark_name = src_root["remark_name"].asString();
        std::string request_message = src_root["request_message"].asString();
        if(!MysqlMgr::GetInstance()->Applytoaddfriends(root, uid, peer_uid, remark_name, request_message))
        {
            root["code"] = ErrorCodes::ERR_UNKNOWN;
        }

        // 通知对端peer_uid
        // 查询redis，查找peer_uid对应的ChatServer
        // TODO...这里假设一定能找到ChatServer
        std::string peer_server_name;
        int errorCode;
        if(RedisMgr::GetInstance()->search_uid_chatserver(peer_uid, peer_server_name, errorCode) && errorCode == 0)
        {
            // 成功找到对端所在ChatServer
            if(peer_server_name == server_name)
            {
                // 对端在本服务器
                auto session = UserMgr::GetInstance()->GetSession(peer_uid);
                if(session == nullptr)
                    return;
                Json::Value notify;
                MysqlMgr::GetInstance()->GetUserBaseInfo(notify, uid);
                notify["code"] = ErrorCodes::SUCCESS;
                notify["request_message"] = request_message;

                std::string jsonstr = notify.toStyledString();
                sendMsg(session, ID_NOTIFY_ADD_FRIEND_REQ, jsonstr);

                return;
            }

            // 对端在其他服务器
        }
    }
};