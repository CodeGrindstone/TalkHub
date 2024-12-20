#ifndef CHATFRIENDITEM_H
#define CHATFRIENDITEM_H

#include <QWidget>
#include <QString>
#include <QIcon>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "elidedlabel.h"

class ChatFriendItem : public QWidget
{
    Q_OBJECT
public:
    explicit ChatFriendItem(const QString &avatarPath, const QString &friendName,
                            const QString &latestMessage = QString(), const QString &messageTime = QString(),
                            QWidget *parent = nullptr)
        : QWidget(parent) {
        // 设置布局
        QHBoxLayout *mainLayout = new QHBoxLayout(this);

        // 创建头像控件
        QLabel *avatarLabel = new QLabel(this);
        QPixmap avatarPixmap(avatarPath);
        avatarLabel->setPixmap(avatarPixmap.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 调整头像大小
        avatarLabel->setFixedSize(48, 48); // 固定头像大小
        mainLayout->addWidget(avatarLabel);

        // 垂直布局：名字和消息
        QVBoxLayout *textLayout = new QVBoxLayout();

        // 水平布局：名字 + 时间
        QHBoxLayout *nameAndTimeLayout = new QHBoxLayout();


        // 好友名字
        ElidedLabel *nameLabel = new ElidedLabel(friendName, this);
        nameLabel->setStyleSheet("font-size: 13px; font-weight: bold;");
        nameAndTimeLayout->addWidget(nameLabel);

        // 时间
        QLabel *timeLabel = new QLabel(messageTime, this);
        timeLabel->setStyleSheet("font-size: 12px; color: gray;");
        timeLabel->setAlignment(Qt::AlignRight);
        timeLabel->setFixedWidth(80); // 固定时间宽度
        nameAndTimeLayout->addWidget(timeLabel);

        // 添加名字和时间布局到文本布局
            textLayout->addLayout(nameAndTimeLayout);

        // 最新消息
        ElidedLabel *messageLabel = new ElidedLabel(latestMessage, this);
        messageLabel->setStyleSheet("font-size: 12px; color: gray;");
        messageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        textLayout->addWidget(messageLabel);

        // 添加文本布局到主布局
        mainLayout->addLayout(textLayout);

        // 设置主布局
        mainLayout->setContentsMargins(8, 8, 8, 8); // 调整间距
        mainLayout->setSpacing(6);
    }

    // 更新最新消息
    void updateMessage(const QString &newMessage) {
        messageLabel_->setText(newMessage);
    }

    // 更新消息时间
    void updateTime(const QString &newTime) {
        timeLabel_->setText(newTime);
    }

private:
    QLabel *messageLabel_; // 最新消息标签
    QLabel *timeLabel_;    // 消息时间标签
};

#endif // CHATFRIENDITEM_H
