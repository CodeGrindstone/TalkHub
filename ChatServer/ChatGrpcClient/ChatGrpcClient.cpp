#include "ChatGrpcClient.h"
#include "../../Config/ConfigMgr.h"

ChatGrpcClient::ChatGrpcClient()
{
    auto& cfg = ConfigMgr::getInstance();
    for(auto& server_name : peer_server_name)
        _pools[server_name] = std::make_unique<ChatConPool>(5, cfg[server_name]["Host"], cfg[server_name]["Port"]);
}

AddFriendRsp ChatGrpcClient::NotifyAddFriend(std::string server_ip, const AddFriendReq& req) 
{
    AddFriendRsp rsp;
    return rsp;
}

AuthFriendRsp ChatGrpcClient::NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req) 
{
    AuthFriendRsp rsp;
    return rsp;
}
// bool ChatGrpcClient::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo) {
//     return true;
// }

TextChatMsgRsp ChatGrpcClient::NotifyTextChatMsg(std::string server_ip,  const TextChatMsgReq& req, const Json::Value& rtvalue) 
{
    TextChatMsgRsp rsp;
    return rsp;
}