#include "ChatServer.h"
using namespace muduo;
using namespace muduo::net;

void ChatServer::onConnection(const muduo::net::TcpConnectionPtr& conn) {
    if (conn->connected()) {
        LOG_INFO << "New connection from " << conn->peerAddress().toIpPort();
        states_[conn] = ConnectionState(); // 初始化连接状态
        publisher_.addConnection(conn);
    } else {
        LOG_INFO << "Connection disconnected: " << conn->peerAddress().toIpPort();
        states_.erase(conn); // 移除对应的状态
        publisher_.removeConnection(conn);
        // 如果此连接有用户登录，则需要修改其在线状态
        UserMgr::GetInstance()->RmvUserSession(conn);
        
    }
}

void ChatServer::onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp time) 
{
    auto& state = states_[conn]; // 获取该连接的状态

    while (buffer->readableBytes() > 0) {
        if (!state.headerParsed) {
            // 尝试解析消息头
            if (buffer->readableBytes() >= 4) {
                state.messageId = buffer->readInt16();
                state.messageLength = buffer->readInt16();
                state.headerParsed = true;
                LOG_DEBUG << "Parsed header: messageId=" << state.messageId
                            << ", messageLength=" << state.messageLength;
            } else {
                // 数据不足 4 字节，等待更多数据
                break;
            }
        }

        if (state.headerParsed) {
            // 尝试解析消息内容
            if (buffer->readableBytes() >= state.messageLength) {
                std::string message = buffer->retrieveAsString(state.messageLength);
                LOG_INFO << "Received message: " << message;

                // 处理消息（根据 messageId）
                dispatcher_.dispatch(static_cast<ReqId>(state.messageId), conn, message);

                // 重置状态以解析下一条消息
                state.headerParsed = false;
            } else {
                // 数据不足，等待更多数据
                break;
            }
        }
    }
}