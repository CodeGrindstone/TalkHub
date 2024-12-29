#pragma once

#include "../MessagePublisher/MessagePublisher.h"
#include "../MessageDispatcher/MessageDispatcher.h"
#include "../UserMgr/UserMgr.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/Buffer.h>
#include <muduo/base/Logging.h>
#include <string>
#include <memory>

class ChatServer {
public:
    ChatServer(muduo::net::EventLoop* loop, const muduo::net::InetAddress& listenAddr)
        : server_(loop, listenAddr, "ChatServer") {
        server_.setConnectionCallback(
            std::bind(&ChatServer::onConnection, this, std::placeholders::_1));
        server_.setMessageCallback(
            std::bind(&ChatServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    void start() {
        // 注册消息处理器
        dispatcher_.registerHandler(ReqId::ID_CHAT_LOGIN, std::make_shared<LoginHandler>(this));
        dispatcher_.registerHandler(ReqId::ID_SEARCH_USER_REQ, std::make_shared<SearchUserHandler>(this));
        dispatcher_.registerHandler(ReqId::ID_ADD_FRIEND_REQ, std::make_shared<ApplytoAddFriendHandler>(this));
        dispatcher_.registerHandler(ReqId::ID_AUTH_FRIEND_REQ, std::make_shared<AuthFriendReqHandler>(this));
        dispatcher_.registerHandler(ReqId::ID_TEXT_CHAT_MSG_REQ, std::make_shared<ChatTextMsgHandler>(this));
        // dispatcher_.registerHandler(2, std::make_shared<ChatHandler>());

        server_.start();
    }

    void stop()
    {
        LOG_INFO << "Stopping server...";

        // 关闭所有现有连接
        publisher_.removeAllConnections();

        LOG_INFO << "Server stopped.";
    }

    void sendMsg(const muduo::net::TcpConnectionPtr& conn, ReqId msgId, const std::string& msg)
    {
        publisher_.sendMsg(conn, msgId, msg);
    }
    
private:

    struct ConnectionState {
        uint16_t messageId = 0;      // 当前消息的 ID
        uint16_t messageLength = 0;  // 当前消息的长度
        bool headerParsed = false;   // 是否已解析出消息头
    };

    void onConnection(const muduo::net::TcpConnectionPtr& conn);

    void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp time);
    
    muduo::net::TcpServer server_;
    MessagePublisher publisher_; // 消息推送层
    MessageDispatcher dispatcher_; // 消息分发器
    std::unordered_map<muduo::net::TcpConnectionPtr, ConnectionState> states_; // 每个连接的状态
};

