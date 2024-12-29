#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include "usermgr.h"

namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();
    void SetUserInfo(std::shared_ptr<UserInfo> user_info);
    void AppendChatMsg(std::shared_ptr<TextChatMsg> msg);
private slots:
    void on_send_btn_clicked();

private:
    void paintEvent(QPaintEvent *event) override
    {
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
private:
    Ui::ChatPage *ui;
    std::shared_ptr<UserInfo> user_info_;
};

#endif // CHATPAGE_H
