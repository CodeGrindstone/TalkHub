#include "conuseritem.h"
#include "ui_conuseritem.h"

ConUserItem::ConUserItem(QWidget *parent) :
    ListItemBase(parent),
    ui(new Ui::ConUserItem)
{
    ui->setupUi(this);
    SetItemType(ListItemType::CONTACT_USER_ITEM);
    ui->red_point->raise(); // 提升为最高层
    ShowRedPoint(false);
}
ConUserItem::~ConUserItem()
{
    delete ui;
}
QSize ConUserItem::sizeHint() const
{
    return QSize(250, 70); // 返回自定义的尺寸
}
// void ConUserItem::SetInfo(std::shared_ptr<AuthInfo> auth_info)
// {
//     info_ = std::make_shared<UserInfo>(auth_info);
//     // 加载图片
//     QPixmap pixmap(info_->icon_);
//     // 设置图片自动缩放
//     ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
//     ui->icon_lb->setScaledContents(true);
//     ui->user_name_lb->setText(info_->name_);
// }


void ConUserItem::SetInfo(int uid, QString name, QString icon)
{
    if(uid == -1)
    {
        // 加载图片
        QPixmap pixmap(icon);
        // 设置图片自动缩放
        ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->icon_lb->setScaledContents(true);
        ui->user_name_lb->setText(name);

        return;
    }



    // 加载图片
    QPixmap pixmap(icon);
    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);
    ui->user_name_lb->setText(name);
}

void ConUserItem::SetInfo(std::shared_ptr<UserInfo> user_info)
{
    user_info_ = user_info;

    // 加载图片
    QPixmap pixmap(user_info->profile_picture_);
    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);
    ui->user_name_lb->setText(user_info->name_);
}

void ConUserItem::ShowRedPoint(bool show)
{
    if(show){
        ui->red_point->show();
    }else{
        ui->red_point->hide();
    }
}
