#include "chatuserwid.h"
#include "ui_chatuserwid.h"

ChatUserWid::ChatUserWid(QWidget *parent) :
    ListItemBase(parent),
    ui(new Ui::ChatUserWid)
{
    ui->setupUi(this);
    SetItemType(ListItemType::CHAT_USER_ITEM);
    ShowRedPoint(false);
}

ChatUserWid::~ChatUserWid()
{
    delete ui;
}

void ChatUserWid::SetInfo(std::shared_ptr<UserInfo> user_info, QString msg)
{
    user_info_ = user_info;
    last_msg_ = msg;

    // 加载图片
    QPixmap pixmap(user_info_->profile_picture_);
    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);
    ui->user_name_lb->setText(user_info_->name_);
    ui->user_chat_lb->setText(last_msg_);
}

void ChatUserWid::ShowRedPoint(bool bshow)
{
    if(bshow){
        ui->red_point->show();
    }else{
        ui->red_point->hide();
    }
}

void ChatUserWid::updateLastMsg(std::shared_ptr<TextChatMsg> msgs)
{
    last_msg_ = msgs->content_;
    chat_msgs_.push_back(msgs);
    ui->user_chat_lb->setText(last_msg_);
}
