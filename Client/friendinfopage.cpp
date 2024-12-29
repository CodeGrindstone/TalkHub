#include "friendinfopage.h"
#include "ui_friendinfopage.h"
#include <QDebug>

FriendInfoPage::FriendInfoPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FriendInfoPage),user_info_(nullptr)
{
    ui->setupUi(this);
    ui->msg_chat->SetState("normal","hover","press");
    ui->video_chat->SetState("normal","hover","press");
    ui->voice_chat->SetState("normal","hover","press");
}

FriendInfoPage::~FriendInfoPage()
{
    delete ui;
}

void FriendInfoPage::SetInfo(std::shared_ptr<UserInfo> user_info)
{
    user_info_ = user_info;
    // 加载图片
    QPixmap pixmap(user_info->profile_picture_);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->name_lb->setText(user_info->name_);
    ui->email_lb->setText(user_info->email_);
    ui->bio_lb->setText(user_info->bio_);

    if(user_info->gender_ == Gender::MALE)
    {
        QPixmap pixmap(":/Icon/chat/male.png");
        // 设置图片自动缩放
        ui->sex_lb->setPixmap(pixmap.scaled(ui->sex_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->sex_lb->setScaledContents(true);
    }
    else if(user_info->gender_ == Gender::FEMALE)
    {
        QPixmap pixmap(":/Icon/chat/female.png");
        // 设置图片自动缩放
        ui->sex_lb->setPixmap(pixmap.scaled(ui->sex_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->sex_lb->setScaledContents(true);
    }
}

void FriendInfoPage::on_msg_chat_clicked()
{
    qDebug() << "msg chat btn clicked";
    emit sig_jump_chat_item(user_info_);
}
