#pragma once

#include <muduo/net/TcpConnection.h>
#include <set>
#include <mutex>
#include "../../const.h"

class MessagePublisher {
public:
    // 添加连接
    void addConnection(const muduo::net::TcpConnectionPtr& conn) {
        std::lock_guard<std::mutex> lock(mutex_);
        connections_.insert(conn);
    }

    // 移除连接
    void removeConnection(const muduo::net::TcpConnectionPtr& conn) {
        std::lock_guard<std::mutex> lock(mutex_);
        connections_.erase(conn);
    }

    void removeAllConnections(){
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& entry : connections_) {
                entry->shutdown();  // 关闭连接
            }
        connections_.clear();
    }

    // 根据消息id发送消息
    void sendMsg(const muduo::net::TcpConnectionPtr conn, ReqId msgId, const std::string& msg)
    {
        std::set<muduo::net::TcpConnectionPtr>::iterator it;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            it = connections_.find(conn);
        }
        
        if(it == connections_.end())
            return;

        // 构造消息数据包，消息格式为: [消息ID(2字节网络字节序)][消息长度(2字节网络字节序)][消息内容]
        uint16_t netMsgId = htons(static_cast<uint16_t>(msgId));
        uint16_t netMsgLen = htons(static_cast<uint16_t>(msg.size()));

        std::vector<char> buffer;
        buffer.resize(4 + msg.size());

        // 填充消息ID
        memcpy(buffer.data(), &netMsgId, 2);

        // 填充消息长度
        memcpy(buffer.data() + 2, &netMsgLen, 2);

        // 填充消息内容
        memcpy(buffer.data() + 4, msg.data(), msg.size());

        // 发送消息
        conn->send(buffer.data(), buffer.size());
    }
private:
    std::set<muduo::net::TcpConnectionPtr> connections_;
    std::mutex mutex_;
};

