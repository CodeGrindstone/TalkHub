#ifndef APPLYFRIENDPAGE_H
#define APPLYFRIENDPAGE_H

#include <QWidget>
#include "applyfrienditem.h"
#include "userdata.h"

namespace Ui {
class ApplyFriendPage;
}

class AddFriendApply;

// 申请添加好友页
class ApplyFriendPage : public QWidget
{
    Q_OBJECT
public:
    explicit ApplyFriendPage(QWidget *parent = nullptr);
    ~ApplyFriendPage();
    void AddNewApply(std::shared_ptr<AddFriendApply> apply);
protected:
    void paintEvent(QPaintEvent *event);
private:
    void loadApplyList();
    Ui::ApplyFriendPage *ui;
    std::unordered_map<int, ApplyFriendItem*> _unauth_items;    // 未点击同一的好友Item
public slots:
    void slot_auth_rsp(int friend_uid);
signals:
    void sig_show_search(bool);
};
#endif // APPLYFRIENDPAGE_H
