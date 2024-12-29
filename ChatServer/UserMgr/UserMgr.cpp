#include "UserMgr.h"


UserMgr:: ~UserMgr() {
    session_to_uid_.clear();
}

int UserMgr::GetUid(TcpConnectionPtr session)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto iter = session_to_uid_.find(session);
    if (iter == session_to_uid_.end()) {
        return -1;
    }
    return iter->second;
}

TcpConnectionPtr UserMgr::GetSession(int uid)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for(auto iter : session_to_uid_)
    {
        if(iter.second == uid)
        {
            return iter.first;
        }
    }
    return nullptr;
}

void UserMgr::SetUserSession(TcpConnectionPtr session, int uid)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    session_to_uid_[session] = uid;
}

void UserMgr::RmvUserSession(const TcpConnectionPtr session)
{
    //因为再次登录可能是其他服务器，所以会造成本服务器删除key，其他服务器注册key的情况
    // 有可能其他服务登录，本服删除key造成找不到key的情况
    //RedisMgr::GetInstance()->Del(USERIPPREFIX + uid_str);

    auto it = session_to_uid_.find(session);
    if(it == session_to_uid_.end())
        return;
    int uid = it->second;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        session_to_uid_.erase(session);
    }
    // 在Redis设置离线状态    
    RedisMgr::GetInstance()->LoginOut(std::to_string(uid));
}


UserMgr::UserMgr()
{
}