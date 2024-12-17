#include "loginwidget.h"
#include "ui_loginwidget.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include "const.h"
LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
    initial();
}


LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::initial()
{
    this->setWindowTitle("TalkHub");
    QIcon window(":/Icon/global/talk.png");
    this->setWindowIcon(window);
    ui->userLineEdit->setPlaceholderText("Username or email");
    // 添加左侧图标
    QIcon usericon(":/Icon/login/user.png");
    ui->userLineEdit->addAction(usericon, QLineEdit::LeadingPosition);

    ui->passwdLineEdit->setPlaceholderText("Password");
    // 添加左侧图标
    QIcon passwdicon(":/Icon/login/passwd.png");
    ui->passwdLineEdit->addAction(passwdicon, QLineEdit::LeadingPosition);

    setupPasswordToggle(ui->passwdLineEdit, ":/Icon/login/visible.png", ":/Icon/login/unvisible.png");

    setupRegisterLabel();
    setupForgetPasswdLabel();
    // 固定窗口大小为 400x300
    this->setFixedSize(400, 300);
}

void LoginWidget::setupRegisterLabel()
{
    ui->registerLabel->setCursor(Qt::PointingHandCursor); // 设置鼠标悬浮时为手型光标
    connect(ui->registerLabel, &ClickableLabel::clicked, this, [this]() {
        emit sig_switchRegister();
    });
}

void LoginWidget::setupForgetPasswdLabel()
{
    ui->forgetLabel->setCursor(Qt::PointingHandCursor); // 设置鼠标悬浮时为手型光标
    connect(ui->forgetLabel, &ClickableLabel::clicked, this, [this]() {
        emit sig_switchRegister();
    });
}
