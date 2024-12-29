#pragma once

#include "../../protoc/message.pb.h"
#include "../../protoc/message.grpc.pb.h"


using grpc::Channel;
using grpc::Status;
using grpc::ServerContext;

using message::ChatService;

using message::AddFriendReq;
using message::AddFriendRsp;

using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::TextChatMsgReq;
using message::TextChatMsgRsp;

class ChatServiceImpl final : public ChatService::Service
{
public:
    ChatServiceImpl();
    Status NotifyAddFriend(::grpc::ServerContext* context, const AddFriendReq* request, AddFriendRsp* reply) override;
    Status NotifyAuthFriend(::grpc::ServerContext* context, const AuthFriendReq* request, AuthFriendRsp* response) override;
    Status NotifyTextChatMsg(::grpc::ServerContext* context, const TextChatMsgReq* request, TextChatMsgRsp* response) override;
    // bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
private:

};