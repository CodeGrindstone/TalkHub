#pragma once

#include "../MessageHandler/IMessageHandler.h"
#include "../MessageHandler/LoginHandler.h"
#include "../MessageHandler/SearchUserHandler.h"
#include "../MessageHandler/ApplytoAddFriendHandler.h"
#include "../MessageHandler/AuthFriendReqHandler.h"
#include "../MessageHandler/ChatTextMsgHandler.h"
#include <unordered_map>
#include <functional>
#include <memory>

class MessageDispatcher {
public:
    // 注册处理器
    void registerHandler(ReqId messageId, std::shared_ptr<IMessageHandler> handler) {
        handlers_[messageId] = handler;
    }

    // 分发消息
    void dispatch(ReqId messageId, const muduo::net::TcpConnectionPtr& conn, const std::string& data) {
        auto it = handlers_.find(messageId);
        if (it != handlers_.end()) {
            it->second->handle(conn, data);
            
        } else {
            conn->send("Unknown message ID: " + std::to_string(messageId) + "\n");
        }
    }

private:
    std::unordered_map<ReqId, std::shared_ptr<IMessageHandler>> handlers_;
};

