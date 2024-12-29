#include "MysqlMgr.h"
MysqlMgr::~MysqlMgr() {
}

bool MysqlMgr::RegisterUser(Json::Value& root, std::string const &email, std::string const &pwd)
{
    try
    {
        int errorCode;
        if(_dao.RegisterUser(email, pwd, errorCode) && errorCode == 0) // 成功插入
        {
            root["code"] = ErrorCodes::SUCCESS;
            return true;
        }

        switch (errorCode) // 失败
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
        }
        return true;  
    }
    catch(std::exception& e)
    {
        return false;
    }
}

bool MysqlMgr::isUserExistByEmail(std::string const &email)
{
    int errorCode;
    if(_dao.isUserExistByEmail(email, errorCode) && errorCode == 1)
        return true;
    else
        return false;  
}

bool MysqlMgr::ResetPasswdByEmail(Json::Value &root, std::string const &email, std::string const &pwd) 
{
    try
    {
        int errorCode;
        if(_dao.ResetPasswdByEmail(email, pwd, errorCode) && errorCode == 0)
        {
            root["code"] = ErrorCodes::SUCCESS;
            return true;
        }
        
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
            case 2:
            {
                root["code"] = ErrorCodes::ERR_USER_NOT_FOUND;
                break;
            }
        }
        return true;
    }catch(std::exception& e)
    {
        return false;
    }
}

bool MysqlMgr::VerifyUser(Json::Value &root, std::string const &email, std::string const &pwd, int& uid) 
{
    try
    {
        bool isValid;
        int errorCode;
        std::string name; 
        std::string sex;
        std::string icon; 
        std::string bio;

        _dao.VerifyUser(email, pwd, isValid, errorCode, uid, name, sex, icon, bio);
        if(isValid && errorCode == 0)
        {
            root["code"] = ErrorCodes::SUCCESS;
            root["name"] = name;
                if(sex_map.find(sex) == sex_map.end())
                    root["gender"] = Gender::OTHER;
                else
                    root["gender"] = sex_map.find(sex)->second;
                root["profile_picture"] = icon; 
                root["bio"] = bio;
            
            std::vector<int> request_uids;
            std::vector<std::string> request_messages;
            std::vector<std::string> status;

            _dao.GetReceivedFriendReq(uid, request_uids, request_messages, status, errorCode);

            for(int i = 0; i < request_uids.size(); i++)
            {
                Json::Value apply_add_friend_info;
                apply_add_friend_info["uid"] = request_uids.at(i);
                apply_add_friend_info["request_messages"] = request_messages.at(i);
                apply_add_friend_info["statu"] = status.at(i);
                root["apply_add_friend_info"].append(std::move(apply_add_friend_info));
            }

            std::vector<int> friends_vec;
            _dao.GetUserFriend(uid, friends_vec, errorCode);

            for(int i = 0; i < friends_vec.size(); i++)
            {
                root["friends_id"].append(friends_vec.at(i));
            }
            std::vector<int> need_get_user_info_uids;
            // 使用 insert 方法将 vec2 合并到 vec1 的末尾
            request_uids.insert(request_uids.end(), friends_vec.begin(), friends_vec.end());
            

            std::vector<int> receiver_id;
            std::vector<std::string> contents;
            _dao.GetUserSendMes(uid, receiver_id, contents, errorCode);
            for(int i = 0; i < receiver_id.size(); i++)
            {
                Json::Value SendMsg;
                SendMsg["receiver_uid"] = receiver_id.at(i);
                SendMsg["content"] = contents.at(i);
                root["send_msg"].append(std::move(SendMsg));
            }
            
            std::vector<int> sender_id;
            contents.clear();
            _dao.GetUserRecvMes(uid, sender_id, contents, errorCode);
            for(int i = 0; i < sender_id.size(); i++)
            {
                Json::Value RecvMsg;
                RecvMsg["sender_uid"] = sender_id.at(i);
                RecvMsg["content"] = contents.at(i);
                root["recv_msg"].append(std::move(RecvMsg));
            }
            
            
            GetUserBaseInfo(root, request_uids);

        }

        // 失败
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
            case 2:
            {
                root["code"] = ErrorCodes::ERR_USER_NOT_FOUND;
                break;
            }
            case 3:
            {
                root["code"] = ErrorCodes::ERR_PASSWORD_MISMATCH; // 密码不匹配
                break;
            }
        }
        
    }catch(std::exception& e){
        return false;
    }
    return true;
}

bool MysqlMgr::SearchUserByUid(Json::Value &root, int search_uid)
{
    return GetUserBaseInfo(root, search_uid);
}

bool MysqlMgr::Applytoaddfriends(Json::Value &root, int uid, int peer_id,
                                 std::string const &remark_name,
                                 std::string const &request_message) 
{
    try
    {
        int errorCode;
        if(_dao.Applytoaddfriends(uid, peer_id, remark_name, request_message, errorCode) && errorCode == 0)
        {
            root["code"] = ErrorCodes::SUCCESS;
            return true;
        }

        // 失败
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
            case 2:
            {
                root["code"] = ErrorCodes::ERR_ALREADY_FRIENDS;
                break;
            }
            case 3:
            {
                root["code"] = ErrorCodes::ERR_DUPLICATE_REQUEST; 
                break;
            }
        }
        
    }catch(std::exception& e){
        return false;
    }
    return true;
}

bool MysqlMgr::AuthFriendReq(Json::Value &root, int requester_uid,
                             int receiver_uid) 
{
    try
    {
        int errorCode;
        if(_dao.AuthFriendReq(requester_uid, receiver_uid, errorCode) && errorCode == 0)
        {
            root["code"] = ErrorCodes::SUCCESS;
            return true;
        }

        // 失败
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
            case 2:
            {
                root["code"] = ErrorCodes::ERR_REQUEST_NOT_FOUND;
                break;
            }
            // case 3:
            // {
            //     root["code"] = ErrorCodes::ERR_DUPLICATE_REQUEST; 
            //     break;
            // }
        }
        
    }catch(std::exception& e){
        return false;
    }
    return true;  
}

bool MysqlMgr::GetUserBaseInfo(Json::Value &root, std::vector<int> const &vec_uids) 
{
    int errorCode;
    for(int i = 0; i < vec_uids.size(); i++)
    {
        int uid = vec_uids.at(i);
        std::string name;
        std::string email;
        std::string gender;
        std::string profile_picture;
        std::string bio;

        _dao.GetUserBaseInfo(uid, name, email, gender, profile_picture, bio, errorCode);

        Json::Value user_info;
        user_info["uid"] = uid;
        user_info["name"] = name;
        user_info["email"] = email;
        user_info["profile_picture"] = profile_picture;
        user_info["bio"] = bio;
        user_info["gender"] = sex_map.find(gender)->second;
        
        root["users_info"].append(user_info);
    }
    return true;
}

bool MysqlMgr::GetUserBaseInfo(Json::Value &root, int uid) 
{
    int errorCode;

    std::string name;
    std::string email;
    std::string gender;
    std::string profile_picture;
    std::string bio;

    if(_dao.GetUserBaseInfo(uid, name, email, gender, profile_picture, bio, errorCode) && errorCode == 0)
    {
        Json::Value user_info;
        user_info["uid"] = uid;
        user_info["name"] = name;
        user_info["email"] = email;
        user_info["profile_picture"] = profile_picture;
        user_info["bio"] = bio;
        user_info["gender"] = sex_map.find(gender)->second;
        
        root["users_info"].append(user_info);
        
        return true;
    }
    return false;
}

bool MysqlMgr::SavePrivateMes(Json::Value &root, int fromUid, int toUid,
                              std::string const &message) 
{
    try
    {
        int errorCode;
        if(_dao.SendPrivateMes(fromUid, toUid, message, errorCode) && errorCode == 0)
        {
            root["code"] = ErrorCodes::SUCCESS;
            return true;
        }

        // 失败
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
            case 2:
            {
                root["code"] = ErrorCodes::ERR_UNKNOWN;    // 对方不是你的好友
                break;
            }
            // case 3:
            // {
            //     root["code"] = ErrorCodes::ERR_DUPLICATE_REQUEST; 
            //     break;
            // }
        }
        
    }catch(std::exception& e){
        return false;
    }
    return true;  
}
