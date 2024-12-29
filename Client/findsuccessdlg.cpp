#include "findsuccessdlg.h"
#include "ui_findsuccessdlg.h"
#include <QString>
#include <QPixmap>
#include <QDir>

FindSuccessDlg::FindSuccessDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindSuccessDlg), parent_(parent)
{
    ui->setupUi(this);
    // 设置对话框标题
    setWindowTitle("添加");
    // 隐藏对话框标题栏
    //setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    ui->add_friend_btn->SetState("normal","hover","press");
    this->setModal(true);
}

FindSuccessDlg::~FindSuccessDlg()
{
    delete ui;
}

void FindSuccessDlg::SetSearchInfo(std::shared_ptr<UserInfo> user_info)
{
    QPixmap head_pix(user_info->profile_picture_);
    head_pix = head_pix.scaled(ui->head_lb->size(),
                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->head_lb->setPixmap(head_pix);
    ui->name_lb->setText(user_info->name_);
    user_info_ = user_info;
}



void FindSuccessDlg::on_add_friend_btn_clicked()
{
    this->hide();
    //弹出加好友界面
    auto applyFriend = new ApplyFriend(parent_);
    applyFriend->SetSearchInfo(user_info_);
    applyFriend->setModal(true);
    applyFriend->show();
}
