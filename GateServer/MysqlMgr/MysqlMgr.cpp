#include "MysqlMgr.h"
MysqlMgr::~MysqlMgr() {
}

bool MysqlMgr::RegisterUser(Json::Value& root, std::string const &email, std::string const &pwd)
{
    try
    {
        int userNumber;
        if(_dao.RegisterUser(email, pwd, userNumber)) // 成功插入
        {
            root["code"] = ErrorCodes::SUCCESS;
            root["id"] = userNumber;
            return true;
        }

        switch (userNumber) // 失败
        {
            case -1:{
            // 无法连接mysql
                root["code"] = ErrorCodes::ERR_TOO_MANY_CONNECTIONS;
                break;
            }
            case 1:{
                // sql错误，插入失败
                root["code"] = ErrorCodes::ERR_DATABASE;
                break;
            }
            case 2:{
                // 邮箱已被注册
                root["code"] = ErrorCodes::ERR_ACCOUNT_ALREADY_EXISTS;
                break;
            }
            default:{
                root["code"] = ErrorCodes::ERR_INVALID_PARAMS;
                break;
            }
        }  
        return true;  
    }
    catch(std::exception& e)
    {
        return false;
    }
}

bool MysqlMgr::isUserExistByEmail(std::string const &email, int& errorCode)
{
    return _dao.isUserExistByEmail(email, errorCode);   
}

bool MysqlMgr::ResetPasswdByEmail(Json::Value &root, std::string const &email,
                                  std::string const &pwd) 
{
    try
    {
        int errorCode;
        if(_dao.ResetPasswdByEmail(email, pwd, errorCode))
        {
            switch (errorCode)
            {
                case 0:
                {
                    root["code"] = ErrorCodes::SUCCESS;
                    break;
                } 
                case 1:
                {
                    root["code"] = ErrorCodes::ERR_DATABASE;
                    break;
                }
                case 2:
                {
                    root["code"] = ErrorCodes::ERR_USER_NOT_FOUND;
                    break;
                }
                default:{
                    root["code"] = ErrorCodes::ERR_INVALID_PARAMS;
                    break;
                }
            }
        }
        else
        {
            switch (errorCode)
            {
                case -1:
                {
                    root["code"] = ErrorCodes::ERR_TOO_MANY_CONNECTIONS;
                    break;
                }
                case 1:
                {
                    root["code"] = ErrorCodes::ERR_DATABASE;
                    break;
                }
                default:
                {
                    root["code"] = ErrorCodes::ERR_INVALID_PARAMS;
                    break;
                }
            }
        }
    }catch(std::exception& e)
    {
        return false;
    }
    return true;
}
