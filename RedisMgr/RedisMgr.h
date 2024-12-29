#pragma once

#include "../Singleton.h"
#include "../const.h"
#include "../Config/ConfigMgr.h"
#include "../MysqlMgr/MysqlMgr.h"

#include <hiredis/hiredis.h>
#include <sw/redis++/redis++.h>
#include <memory>

using namespace sw::redis;

class RedisMgr : public Singleton<RedisMgr>
{
    friend class Singleton;
public:
    ~RedisMgr(){
        redis_.reset();
    }
    // 设置键值，成功返回 true，失败返回 false
    bool set(const std::string& key, const std::string& value, int expire_time_ms = 0);
    bool hset(const std::string& key, const std::string& field, const std::string& value);
    // 获取键值，结果通过传出参数 result 返回
    bool get(const std::string& key, std::string& result);
     // 删除键，成功返回 true，失败返回 false
    bool del(const std::string& key);
    // 检查键是否存在，结果通过传出参数 exists 返回
    bool exists(const std::string& key, bool& exists);
    // 使用 HSET 存储多个键值对
    bool hsetUserInfo(const std::string& userId, const std::unordered_map<std::string, std::string>& fields, int expireSeconds = 3600);
    bool hgetAllUserInfo(const std::string& userId, std::unordered_map<std::string, std::string>& fields);
    bool hgetUserToken(const std::string& userId, std::string& token);
    
    // 更新服务器的连接数 (HSET/HINCRBY)
    void update_connection_count(const std::string &server_name, int increment);

    // 获取某台服务器的连接数 (HGET)
    int get_connection_count(const std::string &server_name);

    bool LoginOut(const std::string& userId);
    
    // 给定uid查找用户所在的ChatServer
    bool search_uid_chatserver(int uid, std::string& server_name, int& errorCode);

private:
    RedisMgr();
    std::shared_ptr<Redis> redis_;  // Redis 客户端;
};

