#ifndef CONUSERITEM_H
#define CONUSERITEM_H

#include <QWidget>
#include "listitembase.h"
#include "userdata.h"

namespace Ui {
class ConUserItem;
}

// 联系人列表item
class ConUserItem : public ListItemBase
{
    Q_OBJECT
public:
    explicit ConUserItem(QWidget *parent = nullptr);
    ~ConUserItem();
    QSize sizeHint() const override;
    // void SetInfo(std::shared_ptr<AuthInfo> auth_info);
    // void SetInfo(std::shared_ptr<AuthRsp> auth_rsp);
    void SetInfo(std::shared_ptr<UserInfo> user_info);
    void SetInfo(int uid, QString name, QString icon);
    std::shared_ptr<UserInfo> GetInfo() { return user_info_; }
    void ShowRedPoint(bool show = false);
private:
    Ui::ConUserItem *ui;
    std::shared_ptr<UserInfo> user_info_;
};
#endif // CONUSERITEM_H
