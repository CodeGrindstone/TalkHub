#ifndef CHATUSERWID_H
#define CHATUSERWID_H

#include <QWidget>
#include <QPixmap>
#include <QMouseEvent>
#include "usermgr.h"
#include "listitembase.h"

namespace Ui {
class ChatUserWid;
}
class ChatUserWid : public ListItemBase
{
    Q_OBJECT
public:
    explicit ChatUserWid(QWidget *parent = nullptr);
    ~ChatUserWid();
    QSize sizeHint() const override {
        return QSize(250, 70); // 返回自定义的尺寸
    }
    const std::shared_ptr<UserInfo>& GetUserInfo() { return user_info_; }
    void SetInfo(std::shared_ptr<UserInfo> user_info, QString msg);
    void ShowRedPoint(bool bshow);
    void updateLastMsg(std::shared_ptr<TextChatMsg> msgs);
private:
    Ui::ChatUserWid *ui;
    std::shared_ptr<UserInfo> user_info_;
    std::vector<std::shared_ptr<TextChatMsg>> chat_msgs_;
    QString last_msg_;
};
#endif // CHATUSERWID_H
