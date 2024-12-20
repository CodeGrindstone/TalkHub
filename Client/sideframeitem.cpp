#include "sideframeitem.h"

SideFrameItem::SideFrameItem(QWidget *parent, Qt::WindowFlags f) : QLabel(parent, f), clicked(false)
{

}

void SideFrameItem::setPic(QString normal, QString clicked)
{
    normal_ = normal;
    clicked_ = clicked;
}

void SideFrameItem::showNormalPic()
{
    QPixmap m_pic(normal_);
    setPixmap(m_pic.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    setFixedSize(20, 20); // 固定头像大小
}

void SideFrameItem::showClickedPic()
{
    QPixmap m_pic(clicked_);
    setPixmap(m_pic.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    setFixedSize(20, 20); // 固定头像大小
}

bool SideFrameItem::event(QEvent *e)
{
    if(e->type() == QEvent::Enter)
    {
        setCursor(Qt::PointingHandCursor);
    }
    else if(e->type() == QEvent::Leave)
    {
        unsetCursor();
    }
    return QLabel::event(e);
}



