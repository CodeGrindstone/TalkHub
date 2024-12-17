#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_login = new LoginWidget();
    switchLogin();

    connect(m_login, &LoginWidget::sig_switchRegister, this, &Widget::slot_switchRegister);
}

void switchLogin()
{

}

Widget::~Widget()
{
    delete ui;
    delete m_login;
}

void Widget::switchLogin()
{
    this->hide();
    m_login->show();
}

void Widget::slot_switchRegister()
{
    m_login->hide();
    qDebug() << "切换注册页面";
}

void Widget::slot_switchForget()
{
    m_login->hide();
    qDebug() << "切换忘记密码页面";
}
