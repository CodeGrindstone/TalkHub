#include "loginfaileddialog.h"
#include "ui_loginfaileddialog.h"

LoginFailedDialog::LoginFailedDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginFailedDialog)
{
    ui->setupUi(this);

    connect(ui->resetBtn, &QPushButton::clicked, this, &LoginFailedDialog::sig_resetpasswd);
    connect(ui->returnBtn, &QPushButton::clicked, this, &LoginFailedDialog::sig_relogin);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
}

LoginFailedDialog::~LoginFailedDialog()
{
    delete ui;
}
