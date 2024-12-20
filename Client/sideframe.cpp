#include "sideframe.h"
#include "ui_sideframe.h"
#include <QIcon>
#include <QPixmap>
#include <QSpacerItem>
#include <QMouseEvent>

SideFrame::SideFrame(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::SideFrame)
{
    ui->setupUi(this);

    initInterface();
}

SideFrame::~SideFrame()
{
    delete ui;
}

bool SideFrame::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton){
            if(watched == messageLabel_)
            {
                messageLabel_->showClickedPic();
                friendLabel_->showNormalPic();
            }
            else if(watched == friendLabel_)
            {
                messageLabel_->showNormalPic();
                friendLabel_->showClickedPic();
            }
        }
    }
    return QFrame::eventFilter(watched, event);
}

void SideFrame::initInterface()
{

    messageLabel_ = new SideFrameItem(this);
    messageLabel_->setPic(":/Icon/chat/message.png", ":/Icon/chat/message_clicked.png");
    ui->mainLayout->addWidget(messageLabel_, 0, Qt::AlignCenter);


    friendLabel_ = new SideFrameItem(this);
    friendLabel_->setPic(":/Icon/chat/friend.png", ":/Icon/chat/friend_clicked.png");
    ui->mainLayout->addWidget(friendLabel_, 0, Qt::AlignCenter);


    // 添加垂直弹簧
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->mainLayout->addSpacerItem(verticalSpacer);

    messageLabel_->showNormalPic();
    friendLabel_->showNormalPic();

    messageLabel_->installEventFilter(this);
    friendLabel_->installEventFilter(this);

}
