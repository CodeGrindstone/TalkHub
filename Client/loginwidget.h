#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QMap>
#include <QJsonObject>
#include "const.h"
#include "loginfaileddialog.h"
namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    void initInterface();
    void setupRegisterLabel();
    void setupForgetPasswdLabel();
    void loginFailed();
private:
    Ui::LoginWidget *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> m_handlers;
signals:
    void sig_switchRegister();
    void sig_switchForget();
    void sig_switchChat();
private slots:
    void on_loginBtn_clicked();
};

#endif // LOGINWIDGET_H
