#ifndef LOGINFAILEDDIALOG_H
#define LOGINFAILEDDIALOG_H

#include <QDialog>

namespace Ui {
class LoginFailedDialog;
}

class LoginFailedDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginFailedDialog(QWidget *parent = nullptr);
    ~LoginFailedDialog();

private:
    Ui::LoginFailedDialog *ui;
signals:
    void sig_relogin();
    void sig_resetpasswd();
};

#endif // LOGINFAILEDDIALOG_H
