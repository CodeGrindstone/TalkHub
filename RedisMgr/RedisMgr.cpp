#include "RedisMgr.h"
#include "stdlib.h"
#include <chrono>

// 更新服务器的连接数 (HSET/HINCRBY)
void RedisMgr::update_connection_count(const std::string &server_name, int increment) {
    try {
        redis_->hincrby(LOGIN_COUNT, server_name, increment);
        std::cout << "Updated connection count for server: " << server_name << ", Increment: " << increment << std::endl;
    } catch (const Error &err) {
        std::cerr << "Failed to update connection count: " << err.what() << std::endl;
    }
}

// 获取某台服务器的连接数 (HGET)
int RedisMgr::get_connection_count(const std::string &server_name) {
    try {
        auto result = redis_->hget(LOGIN_COUNT, server_name);
        if (result) {
            return std::stoi(*result); // 如果存在，转换为整数返回
        } else {
            std::cout << "No connection count found for server: " << server_name << std::endl;
            return 0;
        }
    } catch (const Error &err) {
        std::cerr << "Failed to get connection count: " << err.what() << std::endl;
        return -1;
    }
}

bool RedisMgr::LoginOut(std::string const &userId) 
{
    try {
        std::string key = "user:" + userId;
        
        redis_->hset(key, "status", "false");

        auto result = redis_->hget(key, "chatserver");
        if (!result) 
            return false;

        redis_->hincrby(LOGIN_COUNT, *result, -1);

        return true;

    } catch (const Error &err) {
        std::cerr << "Failed to get connection count: " << err.what() << std::endl;
        return -1;
    }
}

bool RedisMgr::search_uid_chatserver(int uid, std::string &server_name, int& errorCode) 
{
    errorCode = 0;      
    try {
        std::string key = "user:" + std::to_string(uid);
        
        auto result = redis_->hget(key, "status");
        if(!result)
        {
            errorCode = 2;              // 用户不在线
            return false;
        }

        result = redis_->hget(key, "chatserver");
        if (!result)
        {
            errorCode = 3;              // 找不到用户所在服务器
            return false;
        }  

        server_name.clear();
        server_name = *result;
        return true;

    } catch (const Error &err) {
        std::cerr << "Failed to get connection count: " << err.what() << std::endl;
        errorCode = 1;          // 繁忙
    }
    return false;
}

RedisMgr::RedisMgr() {
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

bool RedisMgr::hset(std::string const &key, std::string const &field, std::string const &value) 
{
    try {
            redis_->hset(key, field, value);
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

        if(expireSeconds != -1)
            redis_->expire(key, expireSeconds);// 设置过期时间

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

bool RedisMgr::hgetUserToken(std::string const &userId, std::string &token)
{
    try {
        std::string key = "user:" + userId;

        token = "";
        std::string field = "token";
        auto result = redis_->hget(key, field); // result 是 OptionalString 类型
        
        if (result) {
            token = *result; // 解包 OptionalString
            return true;
        } else {
            token = ""; // 没有找到 token
            return false;
        }
        
    } catch (const sw::redis::Error &e) {
        std::cerr << "Error in getUserInfo: " << e.what() << std::endl;
        return false;
    }
}
