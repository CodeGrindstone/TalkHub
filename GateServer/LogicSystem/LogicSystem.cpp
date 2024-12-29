//
// Created by 31435 on 2024/9/4.
//

#include "LogicSystem.h"
#include "../HttpConn/HttpConn.h"
#include "../VerifyGrpcClient/VerifyGrpcClient.h"
#include "../../MysqlMgr/MysqlMgr.h"
#include "../../StatusGrpcClient/StatusGrpcClient.h"

bool parseJson(Json::Value & root, std::string & bodyStr, Json::Value & srcRoot)
{
    Json::Reader reader;
    bool parse_success = reader.parse(bodyStr, srcRoot);
    if(!parse_success)
    {
        root["code"] = ErrorCodes::ERR_INVALID_PARAMS;
        return false;
    }
    return true;
}

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
        Json::Value src_root;

        Defer defer([&connection, &root]{
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
        });

        if(!parseJson(root, body_str, src_root))
            return;

        VerifyType type = static_cast<VerifyType>(src_root["type"].asInt());
        std::string email = src_root["email"].asString();
        bool exists = MysqlMgr::GetInstance()->isUserExistByEmail(email);

        if(type == VerifyType::REGISTER)
        {
            if(!exists) // 不存在邮箱
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
            if(!exists)  // 不存在用户
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
        Json::Value src_root;

        Defer defer([&connection, &root]{
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
        });

        if(!parseJson(root, body_str, src_root))
            return;

        auto email = src_root["email"].asString();
        auto pwd = src_root["passwd"].asString();
        auto verifyCode = src_root["verifyCode"].asString();

        std::string redis_verifyCode;
        if(!RedisMgr::GetInstance()->get(CODEPREFIX + src_root["email"].asString(), redis_verifyCode))
        {
            root["code"] = ErrorCodes::ERR_SERVER_INTERNAL;
            return;
        }

        if (verifyCode != redis_verifyCode) {   // 验证码错误
            root["code"] = ErrorCodes::ERR_VERIFICATION_INVALID;
            return;
        }

        if(!MysqlMgr::GetInstance()->RegisterUser(root, email, pwd)) // 注册账户
        {
            root["code"] = ErrorCodes::ERR_SERVER_INTERNAL;
            return;
        }
    });

    RegPost("/api/v1/auth/reset_passwd", [](std::shared_ptr<HttpConn> connection){
        auto body_str = boost::beast::buffers_to_string(connection->m_request.body().data());
        connection->m_response.set(http::field::content_type, "text/json");

        Json::Value root;
        Json::Value src_root;

        Defer defer([&connection, &root]{
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
        });

        if(!parseJson(root, body_str, src_root))
            return;

        auto email = src_root["email"].asString();
        auto pwd = src_root["passwd"].asString();
        auto verifyCode = src_root["verifyCode"].asString();

        std::string redis_verifyCode;
        if(!RedisMgr::GetInstance()->get(CODEPREFIX + src_root["email"].asString(), redis_verifyCode))
        {
            root["code"] = ErrorCodes::ERR_SERVER_INTERNAL;
            return;
        }

        if (verifyCode != redis_verifyCode) {       // 验证码错误
            root["code"] = ErrorCodes::ERR_VERIFICATION_INVALID;
            return;
        }

        if(!MysqlMgr::GetInstance()->ResetPasswdByEmail(root, email, pwd)) // 
        {
            root["code"] = ErrorCodes::ERR_SERVER_INTERNAL;
            return;
        }
    });

    RegPost("/api/v1/auth/login",[](std::shared_ptr<HttpConn> connection){
        auto body_str = boost::beast::buffers_to_string(connection->m_request.body().data());
        connection->m_response.set(http::field::content_type, "text/json");

        Json::Value root;
        Json::Value src_root;

        Defer defer([&connection, &root]{
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
        });

        if(!parseJson(root, body_str, src_root))
            return;

        auto email = src_root["email"].asString();
        auto pwd = src_root["passwd"].asString();

        int uid;
        if(!MysqlMgr::GetInstance()->VerifyUser(root, email, pwd, uid)) // 注册账户
        {
            root["code"] = ErrorCodes::ERR_SERVER_INTERNAL; // 服务器内部错误
            return;
        }
        root["email"] = email;
        root["uid"] = uid;


        //查询StatusServer找到合适的连接
        auto reply = StatusGrpcClient::GetInstance()->GetChatServer(root, uid);
        if (reply.error()) {
            root["code"] = ErrorCodes::ERR_DEPENDENCY_FAILURE;
            return;
        }
    });
}