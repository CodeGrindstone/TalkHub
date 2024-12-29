#include "chatpage.h"
#include "ui_chatpage.h"
#include "const.h"
#include "chatitembase.h"
#include "textbubble.h"
#include "picturebubble.h"
#include "usermgr.h"
#include "tcpmgr.h"
#include <QJsonDocument>

ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatPage)
{
    ui->setupUi(this);

    //设置按钮样式
    //ui->receive_btn->SetState("normal","hover","press");
    ui->send_btn->SetState("normal","hover","press");
    //设置图标样式
    ui->emo_lb->SetState("normal","hover","press","normal","hover","press");
    ui->file_lb->SetState("normal","hover","press","normal","hover","press");
}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::SetUserInfo(std::shared_ptr<UserInfo> user_info)
{
    user_info_ = user_info;
    //设置ui界面
    ui->title_lb->setText(user_info_->name_);
    ui->chat_data_list->removeAllItem();
    // TODO... 新增聊天记录
    std::vector<std::shared_ptr<TextChatMsg> > msgs;
    if(UserMgr::GetInstance()->getFriendChatMsg(user_info->uid_, msgs))
    {
        for(auto& msg : msgs)
        {
            AppendChatMsg(msg);
        }
    }
}

void ChatPage::AppendChatMsg(std::shared_ptr<TextChatMsg> msg)
{
    auto self_info = UserMgr::GetInstance()->getMyselfUserInfo();
    ChatRole role;
    //todo... 添加聊天显示
    if(msg->is_send_)
    {
        role = ChatRole::Self;
        ChatItemBase* pChatItem = new ChatItemBase(role);

        pChatItem->setUserName(self_info->name_);
        pChatItem->setUserIcon(QPixmap(self_info->profile_picture_));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->content_);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }
    else
    {
        role = ChatRole::Other;
        ChatItemBase* pChatItem = new ChatItemBase(role);
        auto friend_info = UserMgr::GetInstance()->getUserInfo(msg->uid_);
        if(friend_info == nullptr)
        {
            return;
        }
        pChatItem->setUserName(friend_info->name_);
        pChatItem->setUserIcon(QPixmap(friend_info->profile_picture_));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->content_);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }
}

void ChatPage::on_send_btn_clicked()
{
    auto pTextEdit = ui->chatEdit;
    ChatRole role = ChatRole::Self;
    // TODO...
    // QString userName = QStringLiteral(UserMgr::GetInstance()->getName());
    QString userName = UserMgr::GetInstance()->getName();
    QString userIcon = UserMgr::GetInstance()->getProfile_picture();

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    for(int i=0; i<msgList.size(); ++i)
    {
        //消息内容长度不合规就跳过
        if(msgList[i].content.length() > 1024){
            continue;
        }

        QString type = msgList[i].msgFlag;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;
        if(type == "text")
        {
            QJsonObject jsonObj;
            jsonObj["from_uid"] = UserMgr::GetInstance()->getUid();
            jsonObj["token"] = UserMgr::GetInstance()->getToken();
            jsonObj["to_uid"] = user_info_->uid_;
            jsonObj["type"] = "text";
            QByteArray utf8Message = msgList[i].content.toUtf8();
            jsonObj["content"] = QString::fromUtf8(utf8Message);
            QJsonDocument doc(jsonObj);
            QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

            emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);

            pBubble = new TextBubble(role, msgList[i].content);

        }
        else if(type == "image")
        {
            pBubble = new PictureBubble(QPixmap(msgList[i].content) , role);
        }
        else if(type == "file")
        {
        }
        if(pBubble != nullptr)
        {
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
        }
    }

    // qDebug() << "textArray is " << textArray ;
    // //发送给服务器
    // textObj["text_array"] = textArray;
    // textObj["fromuid"] = user_info->_uid;
    // textObj["touid"] = _user_info->_uid;
    // QJsonDocument doc(textObj);
    // QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    // //发送并清空之前累计的文本列表
    // txt_size = 0;
    // textArray = QJsonArray();
    // textObj = QJsonObject();
    // //发送tcp请求给chat server
    // emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);

}





