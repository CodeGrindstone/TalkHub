#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_login = new LoginWidget();
    m_register = new RegisterWidget();
    m_reset = new ResetWidget();
    switchLogin();

    connect(m_login, &LoginWidget::sig_switchRegister, this, &Widget::slot_switchRegister);
    connect(m_register, &RegisterWidget::sig_switchLogin, this, &Widget::slot_RegisSwitchLogin);
    connect(m_login, &LoginWidget::sig_switchForget, this, &Widget::slot_switchForget);
    connect(m_reset, &ResetWidget::sig_switchLogin, this, &Widget::slot_ResetSwitchLogin);
    connect(m_login, &LoginWidget::sig_switchChat, this, &Widget::slot_switchChatWidget);
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
    m_register->show();
}

void Widget::slot_switchForget()
{
    m_login->hide();
    qDebug() << "切换忘记密码页面";
    m_reset->show();
}

void Widget::slot_RegisSwitchLogin()
{
    m_register->hide();
    m_login->update();
    m_login->show();
}

void Widget::slot_ResetSwitchLogin()
{
    m_reset->hide();
    m_login->update();
    m_login->show();
}

void Widget::slot_switchChatWidget()
{
    m_login->hide();
    delete m_login;
    if(m_reset)
    {
        delete m_reset;
    }
    if(m_register)
    {
        delete m_register;
    }

    m_chat = new ChatDialog();
    m_chat->show();
}
