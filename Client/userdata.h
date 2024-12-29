#ifndef USERDATA_H
#define USERDATA_H
#include <QString>
#include <memory>
#include <QJsonArray>
#include <vector>
#include <QJsonObject>
#include <QDir>
#include "const.h"
#include <QCoreApplication>


struct UserInfo
{
    UserInfo(int uid, QString name, QString email, Gender gender, QString profile_picture, QString bio) :
        uid_(uid), name_(name), email_(email), gender_(gender), bio_(bio)
    {
        // 获取当前应用程序的路径
        QString app_path = QCoreApplication::applicationDirPath();

        profile_picture_= QDir::toNativeSeparators(app_path +
                                                    QDir::separator() +
                                                    "static"+QDir::separator() +
                                                    profile_picture);
    }

    int uid_;
    QString name_;
    QString email_;
    Gender gender_;
    QString profile_picture_;
    QString bio_;
};

// 好友请求 -- 接收者
class AddFriendApply {
public:
    AddFriendApply(std::shared_ptr<UserInfo> user, const QString& request_message) :
        user_(user), request_message_(request_message), statu_(false){}

    AddFriendApply(std::shared_ptr<UserInfo> user, const QString& request_message, QString status) :
        user_(user), request_message_(request_message), statu_(false){
        if(status == "pending")
            statu_ = false;
        else if(status == "accepted")
            statu_ = true;
    }

    std::shared_ptr<UserInfo> user_;
    QString request_message_;
    bool statu_;
};


struct TextChatMsg
{
    TextChatMsg(int uid, bool is_send, QString content) :
        uid_(uid), is_send_(is_send), content_(content){}
    int uid_;
    bool is_send_;
    QString content_;
};


// struct AuthInfo {
//     AuthInfo(int uid, QString name, QString icon, SEX sex) :
//         uid_(uid), name_(name), icon_(icon),
//         sex_(sex){}

//     int uid_;
//     QString name_;
//     QString icon_;
//     SEX sex_;
// };

// struct AuthRsp {
//     AuthRsp(int peeruid_, QString peername_,
//             QString peer, QString peericon_, SEX peersex_)
//         :uid_(peeruid_),name_(peername_),icon_(peericon_),sex_(peersex_)
//     {}

//     int uid_;
//     QString name_;
//     QString icon_;
//     SEX sex_;
// };

// struct TextChatData;
// struct FriendInfo {
//     FriendInfo(int uid, QString name, QString icon, SEX sex, QString desc, QString back, QString last_msg=""):uid_(uid),
//         name_(name), icon_(icon), sex_(sex), desc_(desc), back_(back),last_msg_(last_msg){}

//     FriendInfo(std::shared_ptr<AuthInfo> auth_info):
//         uid_(auth_info->uid_),icon_(auth_info->icon_),name_(auth_info->name_), sex_(auth_info->sex_){}

//     FriendInfo(std::shared_ptr<AuthRsp> auth_rsp) :
//         uid_(auth_rsp->uid_), icon_(auth_rsp->icon_),
//         name_(auth_rsp->name_), sex_(auth_rsp->sex_){}

//     void AppendChatMsgs(const std::vector<std::shared_ptr<TextChatData>> text_vec);

//     int uid_;
//     QString name_;
//     QString icon_;
//     SEX sex_;
//     QString desc_;
//     QString back_;
//     QString last_msg_;
//     std::vector<std::shared_ptr<TextChatData>> chat_msgs_;
// };



// struct TextChatData{
//     TextChatData(QString msg_id, QString msg_content, int fromuid, int touid)
//         :msg_id_(msg_id),msg_content_(msg_content),from_uid_(fromuid),to_uid_(touid){

//     }
//     QString msg_id_;
//     QString msg_content_;
//     int from_uid_;
//     int to_uid_;
// };

// struct TextChatMsg{
//     TextChatMsg(int fromuid, int touid, QJsonArray arrays):
//         from_uid_(fromuid),to_uid_(touid){
//         for(auto  msg_data : arrays){
//             auto msg_obj = msg_data.toObject();
//             auto content = msg_obj["content"].toString();
//             auto msgid = msg_obj["msgid"].toString();
//             auto msg_ptr = std::make_shared<TextChatData>(msgid, content,fromuid, touid);
//             chat_msgs_.push_back(msg_ptr);
//         }
//     }
//     int to_uid_;
//     int from_uid_;
//     std::vector<std::shared_ptr<TextChatData>> chat_msgs_;
// };
#endif // USERDATA_H
