#include "applyfrienditem.h"
#include "ui_applyfrienditem.h"

ApplyFriendItem::ApplyFriendItem(QWidget *parent) :
    ListItemBase(parent), _added(false),
    ui(new Ui::ApplyFriendItem)
{
    ui->setupUi(this);
    SetItemType(ListItemType::APPLY_FRIEND_ITEM);
    ui->addBtn->SetState("normal","hover", "press");
    ui->addBtn->hide();

    connect(ui->addBtn, &ClickedBtn::clicked,  [this](){
        qDebug() << "123";
        emit sig_auth_friend(apply_info_);
    });
}

ApplyFriendItem::~ApplyFriendItem()
{
    delete ui;
}

void ApplyFriendItem::SetInfo(std::shared_ptr<AddFriendApply> apply_info)
{
    apply_info_ = apply_info;
    // 加载图片
    QPixmap pixmap(apply_info_->user_->profile_picture_);
    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);
    ui->user_name_lb->setText(apply_info_->user_->name_);
    ui->user_chat_lb->setText(apply_info_->request_message_);
}


void ApplyFriendItem::ShowAddBtn(bool bshow)
{
    if (bshow) {
        ui->addBtn->show();
        ui->already_add_lb->hide();
        _added = false;
    }
    else {
        ui->addBtn->hide();
        ui->already_add_lb->show();
        _added = true;
    }
}
int ApplyFriendItem::GetUid() {
    //return apply_info_->uid_;
    return apply_info_->user_->uid_;
}



