//
// Created by 31435 on 2024/9/4.
//

#include "LogicSystem.h"
#include "../HttpConn/HttpConn.h"
#include "../VerifyGrpcClient/VerifyGrpcClient.h"
#include "../RedisMgr/RedisMgr.h"
#include "../MysqlMgr/MysqlMgr.h"
// #include "../StatusGrpcClient/StatusGrpcClient.h"

LogicSystem::~LogicSystem()
{
}

bool LogicSystem::HandleGet(std::string url, std::shared_ptr<HttpConn> conn)
{
    if(m_GetHandlers.find(url) == m_GetHandlers.end())
    {
        return false;
    }
    m_GetHandlers[url](conn);
    return true;
}

bool LogicSystem::HandlePost(std::string url, std::shared_ptr<HttpConn> conn)
{
    if(m_PostHandlers.find(url) == m_PostHandlers.end())
    {
        return false;
    }
    m_PostHandlers[url](conn);
    return true;
}

void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
    m_GetHandlers.insert(std::make_pair(url, handler));
}

void LogicSystem::RegPost(std::string url, HttpHandler handler)
{
    m_PostHandlers.insert(std::make_pair(url, handler));
}

LogicSystem::LogicSystem()
{
    RegGet("/get_test", [](std::shared_ptr<HttpConn> connection) {
        beast::ostream(connection->m_response.body()) << "receiv eget_test req";
        int i = 0;
        for(auto& elem : connection->m_get_params)
        {
            i++;
            beast::ostream(connection->m_response.body()) << "param " << i << " " << elem.first << " " << elem.second;
        }
    });

    RegPost("/api/v1/auth/verification/code", [](std::shared_ptr<HttpConn> connection){
        auto body_str = beast::buffers_to_string(connection->m_request.body().data());
        connection->m_response.set(http::field::content_type, "text/json");

        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        bool parse_success = reader.parse(body_str, src_root);
        if(!parse_success)
        {
            root["code"] = ErrorCodes::ERR_INVALID_PARAMS;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return;
        }

        Defer defer([&connection, &root]{
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
        });

        VerifyType type = static_cast<VerifyType>(src_root["type"].asInt());
        std::string email = src_root["email"].asString();
        int errorCode;
        if(!MysqlMgr::GetInstance()->isUserExistByEmail(email, errorCode)) // 执行失败
        {
            // 根据errorCode做出不同动作
            switch(errorCode)
            {
                case -1:{
                    root["code"] = ErrorCodes::ERR_DATABASE;
                    break;
                }
                default:{
                    root["root"] = ErrorCodes::ERR_SERVER_INTERNAL;
                }
            }
            return;
        }

        if(type == VerifyType::REGISTER)
        {
            if(!errorCode)  // 不存在此邮箱
            {
                GetVerifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVerifyCode(email);
                std::cout << "email is " << email << std::endl;
                root["code"] = rsp.error();
                root["email"] = src_root["email"];
            }
            else    // 存在邮箱
            {
                root["code"] = ErrorCodes::ERR_EMAIL_ALREADY_REGISTERED;
            }
            return;
        }
        else if(type == VerifyType::RESET_PASSWD || type == VerifyType::LOGIN)
        {
            if(!errorCode)  // 不存在用户
            {
                root["code"] = ErrorCodes::ERR_USER_NOT_FOUND;
            }
            else
            {
                GetVerifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVerifyCode(email);
                std::cout << "email is " << email << std::endl;
                root["code"] = rsp.error();
                root["email"] = src_root["email"];
            }
            return;
        }
    });

    RegPost("/api/v1/auth/register", [](std::shared_ptr<HttpConn> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->m_request.body().data());
        connection->m_response.set(http::field::content_type, "text/json");

        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;

        Defer defer([&connection, &root]{
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
        });

        bool parse_success = reader.parse(body_str, src_root);
        if(!parse_success)
        {
            root["code"] = ErrorCodes::ERR_INVALID_PARAMS;
            return;
        }

        auto email = src_root["email"].asString();
        auto pwd = src_root["passwd"].asString();
        auto verifyCode = src_root["verifyCode"].asString();

        std::string redis_verifyCode;
        bool b_get_varify = RedisMgr::GetInstance()->Get(CODEPREFIX + src_root["email"].asString(), redis_verifyCode);
        if (!b_get_varify) {    // 验证码失效
            root["code"] = ErrorCodes::ERR_VERIFICATION_EXPIRED;
            return;
        }

        if (verifyCode != redis_verifyCode) {   // 验证码错误
            root["code"] = ErrorCodes::ERR_VERIFICATION_INVALID;
            return;
        }

        if(MysqlMgr::GetInstance()->RegisterUser(root, email, pwd)) // 注册账户
        {
            return;
        }
        else
        {
            root["code"] = ErrorCodes::ERR_SERVER_INTERNAL;
            return;
        }
        
    });

    RegPost("/api/v1/auth/reset_passwd", [](std::shared_ptr<HttpConn> connection){
        auto body_str = boost::beast::buffers_to_string(connection->m_request.body().data());
        connection->m_response.set(http::field::content_type, "text/json");

        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;

        Defer defer([&connection, &root]{
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
        });

        bool parse_success = reader.parse(body_str, src_root);
        if(!parse_success)
        {
            root["code"] = ErrorCodes::ERR_INVALID_PARAMS;
            return;
        }

        auto email = src_root["email"].asString();
        auto pwd = src_root["passwd"].asString();
        auto verifyCode = src_root["verifyCode"].asString();

        std::string redis_verifyCode;
        bool b_get_varify = RedisMgr::GetInstance()->Get(CODEPREFIX + src_root["email"].asString(), redis_verifyCode);
        if (!b_get_varify) {    // 验证码失效
            root["code"] = ErrorCodes::ERR_VERIFICATION_EXPIRED;
            return;
        }

        if (verifyCode != redis_verifyCode) {   // 验证码错误
            root["code"] = ErrorCodes::ERR_VERIFICATION_INVALID;
            return;
        }

        if(MysqlMgr::GetInstance()->ResetPasswdByEmail(root, email, pwd)) // 注册账户
        {
            return;
        }
        else
        {
            root["code"] = ErrorCodes::ERR_SERVER_INTERNAL;
            return;
        }
    });


    // RegPost("/user_login", [](std::shared_ptr<HttpConn> connection) {
    //     auto body_str = boost::beast::buffers_to_string(connection->m_request.body().data());
    //     std::cout << "receive body is " << body_str << std::endl;
    //     connection->m_response.set(http::field::content_type, "text/json");
    //     Json::Value root;
    //     Json::Reader reader;
    //     Json::Value src_root;
    //     bool parse_success = reader.parse(body_str, src_root);
    //     if (!parse_success) {
    //         std::cout << "Failed to parse JSON data!" << std::endl;
    //         root["error"] = ErrorCodes::Error_Json;
    //         std::string jsonstr = root.toStyledString();
    //         beast::ostream(connection->m_response.body()) << jsonstr;
    //         return true;
    //     }
    //     auto email = src_root["email"].asString();
    //     auto pwd = src_root["passwd"].asString();
    //     UserInfo userInfo;
    //     //查询数据库判断用户名和密码是否匹配
    //     bool pwd_valid = MysqlMgr::GetInstance()->CheckPwd(email, pwd, userInfo);
    //     if (!pwd_valid) {
    //         std::cout << " user pwd not match" << std::endl;
    //         root["error"] = ErrorCodes::PasswdInvalid;
    //         std::string jsonstr = root.toStyledString();
    //         beast::ostream(connection->m_response.body()) << jsonstr;
    //         return true;
    //     }
    //     //查询StatusServer找到合适的连接
    //     auto reply = StatusGrpcClient::GetInstance()->GetChatServer(userInfo.uid);
    //     if (reply.error()) {
    //         std::cout << "grpc get chat server failed, error is " << reply.error()<< std::endl;
    //         root["error"] = ErrorCodes::RPCFailed;
    //         std::string jsonstr = root.toStyledString();
    //         beast::ostream(connection->m_response.body()) << jsonstr;
    //         return true;
    //     }
    //     std::cout << "succeed to load userinfo uid is " << userInfo.uid << std::endl;
    //     root["error"] = 0;
    //     root["email"] = email;
    //     root["uid"] = userInfo.uid;
    //     root["token"] = reply.token();
    //     root["host"] = reply.host();
    //     root["port"] = reply.port();
    // #if DEBUG
    //     std::cout << "host: " << reply.host() <<" port: " << reply.port() << std::endl;
    // #endif
    //     std::string jsonstr = root.toStyledString();
    //     beast::ostream(connection->m_response.body()) << jsonstr;
    //     return true;
    //});
}
