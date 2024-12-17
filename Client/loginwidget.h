#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();
private:
    void initial();
    void setupRegisterLabel();
    void setupForgetPasswdLabel();
private:
    Ui::LoginWidget *ui;
signals:
    void sig_switchRegister();
    void sig_switchForget();
};

#endif // LOGINWIDGET_H
