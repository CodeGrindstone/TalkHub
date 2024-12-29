#pragma once


#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

// Defer类
class Defer {
public:
    // 接受一个lambda表达式或者函数指针
    Defer(std::function<void()> func) : func_(func) {}

    // 析构函数中执行传入的函数
    ~Defer() {
        func_();
    }

private:
    std::function<void()> func_;
};

enum ReqId{
    ID_CHAT_LOGIN = 1005,       //登陆聊天服务器
    ID_CHAT_LOGIN_RSP = 1006,   //登陆聊天服务器回包
    ID_SEARCH_USER_REQ = 1007,  // 搜索好友
    ID_SEARCH_USER_RSP = 1008,  // 搜索好友回包
    ID_ADD_FRIEND_REQ = 1009,   // 添加好友
    ID_ADD_FRIEND_RSP = 1010,   // 添加好友回包
    ID_NOTIFY_ADD_FRIEND_REQ = 1011,// 通知客户端有添加好友请求
    
    ID_AUTH_FRIEND_REQ = 1013,  //认证好友请求
    ID_AUTH_FRIEND_RSP = 1014,  //认证好友回复
    ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //通知用户认证好友申请
    ID_TEXT_CHAT_MSG_REQ  = 1017,  //文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP  = 1018,  //文本聊天信息回复
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, //通知用户文本聊天信息
};

enum ErrorCodes
{
    // 成功状态
    SUCCESS = 0,                // 操作成功
    ERR_UNKNOWN = 1,            // 未知错误
    // 客户端错误（4xx）
    ERR_NETWORK = 4000,         // 网络错误
    ERR_INVALID_PARAMS,         // 参数错误
    ERR_EMAIL_ALREADY_REGISTERED, // 邮箱已被注册
    ERR_PHONE_ALREADY_REGISTERED, // 手机号已被注册
    ERR_VERIFICATION_INVALID,   // 验证码错误
    ERR_VERIFICATION_EXPIRED,   // 验证码已过期
    ERR_UNAUTHORIZED,           // 未授权（未登录）
    ERR_FORBIDDEN,              // 禁止访问（无权限）
    ERR_RESOURCE_NOT_FOUND,     // 资源未找到
    ERR_REQUEST_TOO_FREQUENT,   // 请求过于频繁
    ERR_PASSWORD_WEAK,          // 密码强度不足
    ERR_PASSWORD_MISMATCH,      // 密码不匹配
    ERR_USER_NOT_FOUND,         // 用户不存在
    ERR_ACCOUNT_NOT_ACTIVATED,  // 账号未激活
    ERR_ACCOUNT_ALREADY_EXISTS, // 账号已存在（用于注册时）
    ERR_INVALID_TOKEN,          // 无效的令牌（可能是JWT等）
    ERR_SESSION_TIMEOUT,        // 会话超时
    ERR_CAPTCHA_REQUIRED,       // 需要验证码（例如，防止机器人攻击）

    // 服务器错误（5xx）
    ERR_SERVER_INTERNAL = 5000, // 服务器内部错误
    ERR_DATABASE,               // 数据库错误
    ERR_SERVICE_UNAVAILABLE,    // 服务不可用
    ERR_TIMEOUT,                // 请求超时
    ERR_DATA_FORMAT_INVALID,    // 数据格式无效（如JSON格式错误）
    ERR_DEPENDENCY_FAILURE,     // 依赖服务失败（例如，外部API调用失败）
    ERR_RESOURCE_LIMIT_REACHED, // 资源限制已达（例如，存储空间满，API请求超限）
    ERR_SERVER_MAINTENANCE,     // 服务器正在维护

    // 业务逻辑错误（6xx）
    ERR_LOGIN_FAILED = 6000,    // 登录失败（用户名或密码错误）
    ERR_SESSION_EXPIRED,        // 会话过期
    ERR_ACCOUNT_LOCKED,         // 账号被锁定
    ERR_TOO_MANY_ATTEMPTS,      // 尝试次数过多
    ERR_TOKEN_INVALID,          // Token 无效
    ERR_ALREADY_FRIENDS,        // 用户与目标用户已是好友关系
    ERR_DUPLICATE_REQUEST,	    //存在重复的好友请求
    ERR_REQUEST_SELF,	        //用户尝试向自己发送好友请求
    ERR_FRIEND_NOT_FOUND,	    // 目标用户不存在
    ERR_REQUEST_NOT_FOUND,	    // 好友请求不存在
    ERR_REQUEST_EXPIRED,	    // 好友请求已过期，无法操作
    ERR_FRIEND_BLOCKED,	        //目标用户已将当前用户屏蔽
    ERR_REQUEST_PENDING,	    //存在未处理的好友请求
 

    // 系统资源错误（7xx）
    ERR_RESOURCE_BUSY = 7000,   // 系统资源忙（例如，数据库锁）
    ERR_TOO_MANY_CONNECTIONS,   // 超过最大连接数（例如，数据库连接池已满）
    ERR_DISK_SPACE_EXHAUSTED,   // 磁盘空间不足
    ERR_MEMORY_LIMIT_EXCEEDED,  // 内存超限
    ERR_CPU_OVERLOAD,           // CPU 超负荷
};


enum VerifyType
{
    REGISTER = 0,
    RESET_PASSWD,
    LOGIN
};


#define CODEPREFIX "code_"
#define LOGIN_COUNT "server_connections"

extern std::string server_name;
extern std::vector<std::string> peer_server_name;


enum Gender{
    MALE,
    FEMALE,
    OTHER
};

// 定义映射关系
const std::unordered_map<std::string, Gender> sex_map = {
    {"male", Gender::MALE},
    {"female", Gender::FEMALE},
    {"other", Gender::OTHER}
};