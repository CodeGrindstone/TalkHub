#pragma once

#include "../Singleton.h"
#include "../Config/ConfigMgr.h"

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
    // 获取键值，结果通过传出参数 result 返回
    bool get(const std::string& key, std::string& result);
     // 删除键，成功返回 true，失败返回 false
    bool del(const std::string& key);
    // 检查键是否存在，结果通过传出参数 exists 返回
    bool exists(const std::string& key, bool& exists);
    // 使用 HSET 存储多个键值对
    bool hsetUserInfo(const std::string& userId, const std::unordered_map<std::string, std::string>& fields, int expireSeconds = 3600);
    bool hgetAllUserInfo(const std::string& userId, std::unordered_map<std::string, std::string>& fields);
private:
    RedisMgr();
    std::shared_ptr<Redis> redis_;  // Redis 客户端;
};

