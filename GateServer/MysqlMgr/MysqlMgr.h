#pragma once

#include "../../const.h"
#include <json/json.h>
#include <json/value.h>
#include "MysqlDao.h"
#include "../Singleton.h"

class MysqlMgr: public Singleton<MysqlMgr>
{
    friend class Singleton<MysqlMgr>;
public:
    ~MysqlMgr();
    bool RegisterUser(Json::Value& root, const std::string& email, const std::string& pwd);
    bool isUserExistByEmail(const std::string& email, int& errorCode);
    bool ResetPasswdByEmail(Json::Value& root, const std::string& email, const std::string& pwd);
    bool VerifyUser(Json::Value& root, const std::string& email, const std::string& pwd, unsigned int& uid);
private:
    MysqlMgr() = default;
    MysqlDao  _dao;
};
