#include "RedisMgr.h"
#include "stdlib.h"
#include <chrono>

RedisMgr::RedisMgr()
{
    auto& gCfgMgr = ConfigMgr::getInstance();
    auto host = gCfgMgr["Redis"]["Host"];
    auto port = gCfgMgr["Redis"]["Port"];                                                                                                                 
    auto passwd = gCfgMgr["Redis"]["Passwd"];
    auto pool_size = gCfgMgr["Redis"]["pool_size"];

    ConnectionOptions opts;
    opts.host = host;
    opts.port = atoi(port.c_str());
    opts.password = passwd;

    ConnectionPoolOptions pool_opts;
    pool_opts.size = atoi(pool_size.c_str());
    pool_opts.wait_timeout = std::chrono::milliseconds(500);  
    redis_ = std::make_shared<Redis>(opts, pool_opts);
}

// 设置键值，成功返回 true，失败返回 false
bool RedisMgr::set(const std::string& key, const std::string& value, int expire_time_ms) {
    try {
        if (expire_time_ms > 0) {
            redis_->set(key, value, std::chrono::milliseconds(expire_time_ms));
        } else {
            redis_->set(key, value);
        }
        return true;
    } catch (const sw::redis::Error &e) {
        std::cerr << "Error in SET: " << e.what() << std::endl;
        return false;
    }
}

// 获取键值，结果通过传出参数 result 返回
bool RedisMgr::get(const std::string& key, std::string& result) {
    try {
        auto val = redis_->get(key);
        if (val) {
            result = *val;  // 如果键存在，赋值给 result
        } else {
            result = "";  // 如果键不存在，返回空字符串
        }
        return true;
    } catch (const sw::redis::Error &e) {
        std::cerr << "Error in GET: " << e.what() << std::endl;
        return false;
    }
}

// 删除键，成功返回 true，失败返回 false
bool RedisMgr::del(const std::string& key) {
    try {
        redis_->del(key);
        return true;
    } catch (const sw::redis::Error &e) {
        std::cerr << "Error in DEL: " << e.what() << std::endl;
        return false;
    }
}

// 检查键是否存在，结果通过传出参数 exists 返回
bool RedisMgr::exists(const std::string& key, bool& exists) {
    try {
        exists = redis_->exists(key);
        return true;
    } catch (const sw::redis::Error &e) {
        std::cerr << "Error in EXISTS: " << e.what() << std::endl;
        return false;
    }
}

// 使用 HSET 存储多个键值对
bool RedisMgr::hsetUserInfo(const std::string& userId, 
                        const std::unordered_map<std::string, std::string>& fields, int expireSeconds) 
{
    try {
        std::string key = "user:" + userId;

        // 设置多个字段
        redis_->hmset(key, fields.begin(), fields.end());

        // 设置过期时间
        redis_->expire(key, expireSeconds);

        return true;
    } catch (const sw::redis::Error &e) {
        std::cerr << "Error in setUserInfo: " << e.what() << std::endl;
        return false;
    }
}


// 获取用户信息（多个键值对）
bool RedisMgr::hgetAllUserInfo(const std::string& userId, std::unordered_map<std::string, std::string>& fields) 
{
    try {
        std::string key = "user:" + userId;

        // 获取所有字段
        fields.clear();  // 确保 fields 是空的
        // 使用 hgetall 获取所有字段并填充到 fields 中
        redis_->hgetall(key, std::inserter(fields, fields.begin()));

        // 检查是否获取到任何字段
        if (fields.empty()) {
            std::cerr << "Key " << key << " exists but has no fields." << std::endl;
            return false;
        }

        return true;
    } catch (const sw::redis::Error &e) {
        std::cerr << "Error in getUserInfo: " << e.what() << std::endl;
        return false;
    }
}