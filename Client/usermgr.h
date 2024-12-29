#ifndef USERMGR_H
#define USERMGR_H

#include <QObject>
#include <memory>
#include <singleton.h>
#include "userdata.h"
#include <vector>
#include <unordered_map>


class UserMgr:public QObject,public Singleton<UserMgr>,
                public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
    friend class Singleton<UserMgr>;
    ~UserMgr();
    void setMyselfUserInfo(std::shared_ptr<UserInfo> user_info);
    const std::shared_ptr<UserInfo>& getMyselfUserInfo() { return myself_info_; }
    void setToken(QString token);
    int getUid();
    QString getName();
    QString getProfile_picture();
    QString getToken();
    void setChatServer(QString chat_server){
        chat_server_ = chat_server;
    }
    QString getChatServer(){ return chat_server_; }
    void setIp_Port(QString host, QString port) { host_ = host; port_ = port; }
    QString getHost()   { return host_; }
    QString getPort()   { return port_; }

    void setUserInfo(int uid, std::shared_ptr<UserInfo> user_info);
    std::shared_ptr<UserInfo> getUserInfo(int uid);
    // std::shared_ptr<UserInfo> GetUserInfo();
    // void AppendApplyList(QJsonArray array);
    // void AppendFriendList(QJsonArray array);

    const std::unordered_map<int, std::shared_ptr<AddFriendApply>>& GetApplyInfo() { return add_friend_req_map_; }

    void AddFriendApplyInfo(int uid, std::shared_ptr<AddFriendApply> app);
    void DeleteFriendApplyInfo(int uid);
    bool AlreadyApply(int uid);

    // //std::vector<std::shared_ptr<FriendInfo>> GetChatListPerPage();
    // bool IsLoadChatFin();
    // void UpdateChatLoadedCount();
    // //std::vector<std::shared_ptr<FriendInfo>> GetConListPerPage();
    // void UpdateContactLoadedCount();
    // bool IsLoadConFin();
    bool CheckFriendById(int uid);
    void AddFriend(int uid);
    const std::unordered_map<int, std::shared_ptr<UserInfo>>& getFriendMap() { return friend_map_; }
    // void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);
    // void AddFriend(std::shared_ptr<AuthInfo> auth_info);
    //std::shared_ptr<FriendInfo> GetFriendById(int uid);
    void AppendFriendChatMsg(int friend_id, std::shared_ptr<TextChatMsg>);

    bool getFriendChatMsg(int uid, std::vector<std::shared_ptr<TextChatMsg>>&);
private:
    UserMgr() = default;
    std::shared_ptr<UserInfo> myself_info_;
    QString token_;
    QString chat_server_;
    QString host_;
    QString port_;
    std::unordered_map<int, std::shared_ptr<UserInfo>> users_info_;
    std::unordered_map<int, std::shared_ptr<AddFriendApply>> add_friend_req_map_;
    std::unordered_map<int, std::vector<std::shared_ptr<TextChatMsg>>> friend_chat_msg_;
    // std::vector<std::shared_ptr<ApplyInfo>> _apply_list;
    std::unordered_map<int, std::shared_ptr<UserInfo>> friend_map_;
    // QMap<int, std::shared_ptr<FriendInfo>> _friend_map;

    // int _chat_loaded;
    // int _contact_loaded;

public slots:
    // void SlotAddFriendRsp(std::shared_ptr<AuthRsp> rsp);
    // void SlotAddFriendAuth(std::shared_ptr<AuthInfo> auth);
};
#endif // USERMGR_H
