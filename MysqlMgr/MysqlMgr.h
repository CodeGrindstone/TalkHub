#pragma once

#include "../const.h"
#include <json/json.h>
#include <json/value.h>
#include "MysqlDao.h"
#include "../Singleton.h"
#include <vector>

class MysqlMgr: public Singleton<MysqlMgr>
{
    friend class Singleton<MysqlMgr>;
public:
    ~MysqlMgr();
    bool RegisterUser(Json::Value& root, const std::string& email, const std::string& pwd);
    bool isUserExistByEmail(const std::string& email);
    bool ResetPasswdByEmail(Json::Value& root, const std::string& email, const std::string& pwd);
    bool VerifyUser(Json::Value& root, const std::string& email, const std::string& pwd, int& uid);
    bool SearchUserByUid(Json::Value&root, int search_uid);
    bool Applytoaddfriends(Json::Value&root, int uid, int peer_id, const std::string& remark_name, const std::string& request_message);
    bool AuthFriendReq(Json::Value& root, int requester_uid, int receiver_uid);
    bool GetUserBaseInfo(Json::Value& root, const std::vector<int>& vec_uids);
    bool GetUserBaseInfo(Json::Value& root, int uid); 
    bool SavePrivateMes(Json::Value& root, int fromUid, int toUid, const std::string& message); 
private:
    MysqlMgr() = default;
    MysqlDao  _dao;
};
