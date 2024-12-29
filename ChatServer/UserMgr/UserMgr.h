#pragma once


#include "../Singleton.h"
#include "../../RedisMgr/RedisMgr.h"
#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <memory>
#include <mutex>

using namespace muduo::net;

class UserMgr : public Singleton<UserMgr>
{
    friend class Singleton<UserMgr>;
public:
    ~UserMgr();
    int GetUid(TcpConnectionPtr session);
    TcpConnectionPtr GetSession(int uid);
    void SetUserSession(TcpConnectionPtr session, int uid);
    void RmvUserSession(TcpConnectionPtr session);
private:
    UserMgr();
    std::mutex m_mutex;
    std::unordered_map<TcpConnectionPtr, int> session_to_uid_;
};