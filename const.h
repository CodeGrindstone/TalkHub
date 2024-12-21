#pragma once


#include <functional>


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

enum ErrorCodes
{
    // 成功状态
    SUCCESS = 0,                // 操作成功

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
    ERR_OPERATION_FAILED,       // 操作失败
    ERR_DUPLICATE_OPERATION,    // 重复操作
    ERR_PERMISSION_DENIED,      // 无权限执行操作
    ERR_ACCOUNT_SUSPENDED,      // 账号已暂停
    ERR_ACCOUNT_EXPIRED,        // 账号已过期
    ERR_INSUFFICIENT_FUNDS,     // 余额不足（如支付系统）
    ERR_INVALID_ORDER_STATUS,   // 订单状态无效
    ERR_DUPLICATE_ORDER,        // 重复订单
    ERR_PAYMENT_FAILED,         // 支付失败
    ERR_ITEM_OUT_OF_STOCK,      // 库存不足
    ERR_SHIPPING_ADDRESS_INVALID, // 收货地址无效
    ERR_ORDER_NOT_FOUND,        // 订单未找到
    ERR_ACTION_NOT_ALLOWED,     // 不允许的操作（如禁用某些功能）
    ERR_RATE_LIMIT_EXCEEDED,    // 超过了操作频率限制（如 API 限制）
    ERR_FEATURE_NOT_SUPPORTED,  // 功能不支持
    ERR_MISSING_REQUIRED_FIELD, // 缺少必要字段（如表单提交时）
    ERR_INVALID_REQUEST,        // 请求无效（比如格式不正确）
    ERR_UNSUPPORTED_MEDIA_TYPE, // 不支持的媒体类型（如上传文件类型错误）

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