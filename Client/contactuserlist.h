#ifndef CONTACTUSERLIST_H
#define CONTACTUSERLIST_H

#include <QListWidget>
#include "usermgr.h"

class ConUserItem;
// 联系人列表
class ContactUserList : public QListWidget
{
    Q_OBJECT
public:
    ContactUserList(QWidget *parent = nullptr);
    void ShowRedPoint(bool bshow = true);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override ;
private:
    void addContactUserList();
public slots:
    void slot_item_clicked(QListWidgetItem *item);   // Item被点击
    void slot_add_auth_firend(int friend_uid);// 添加好友申请被同意
    void slot_auth_rsp(int uid);
signals:
    void sig_loading_contact_user();
    void sig_switch_apply_friend_page();
    void sig_switch_friend_info_page(std::shared_ptr<UserInfo> user_info);
private:
    ConUserItem* _add_friend_item;
    QListWidgetItem * _groupitem;
};
#endif // CONTACTUSERLIST_H
