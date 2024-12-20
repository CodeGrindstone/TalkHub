#ifndef FRIENDLISTWIDGET_H
#define FRIENDLISTWIDGET_H

#include <QListWidget>
#include "chatfrienditem.h"

class FriendListWidget : public QListWidget {
    Q_OBJECT

public:
    explicit FriendListWidget(QWidget *parent = nullptr) : QListWidget(parent) {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁止水平滚动条
        this->setFocusPolicy(Qt::NoFocus); // 禁用焦点框
    }

    // 添加好友
    void addFriend(const QString &avatarPath, const QString &friendName, const QString &latestMessage,  const QString &messageTime) {
        QListWidgetItem *item = new QListWidgetItem(this); // 创建数据项
        ChatFriendItem *friendItem = new ChatFriendItem(avatarPath, friendName, latestMessage, messageTime); // 创建自定义控件

        // 设置自定义控件到 QListWidgetItem
        item->setSizeHint(friendItem->sizeHint()); // 调整项高度
        this->addItem(item);                      // 添加到 QListWidget
        this->setItemWidget(item, friendItem);    // 设置自定义控件
    }
};
#endif // FRIENDLISTWIDGET_H
