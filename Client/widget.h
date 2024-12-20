#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "loginwidget.h"
#include "registerwidget.h"
#include "resetwidget.h"
#include "chatwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private:
    void switchLogin();
private:
    Ui::Widget *ui;
    LoginWidget* m_login;
    RegisterWidget* m_register;
    ResetWidget* m_reset;
    ChatWidget* m_chat;

private slots:
    void slot_switchRegister();
    void slot_switchForget();
    void slot_RegisSwitchLogin();
    void slot_ResetSwitchLogin();
    void slot_switchChatWidget();

};
#endif // WIDGET_H
