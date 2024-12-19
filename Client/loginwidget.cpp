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
#include <QDialog>
#include "const.h"
#include "httpmgr.h"

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget)
{
    ui->setupUi(this);

    initInterface();
}


LoginWidget::~LoginWidget()
{
    delete ui;
}

bool LoginWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->loginBtn)
    {
        if(event->type() == QEvent::Enter)
        {
            if(ui->emailLineEdit->text().length() > 0 && ui->passwdLineEdit->text().length() > 0)
            {
                ui->loginBtn->setEnabled(true);
            }
            else
            {
                // 加载自定义的图标
                QPixmap pixmap(":/Icon/register/forbidden.png");  // 替换为你自己的图片路径
                QCursor cursor(pixmap);

                // 设置当前窗口的鼠标指针为自定义图标
                ui->loginBtn->setEnabled(false);
                this->setCursor(cursor);
            }
        }
        else if(event->type() == QEvent::Leave)
        {
            this->setCursor(Qt::ArrowCursor);
        }
    }
    return QWidget::eventFilter(watched, event);
}

void LoginWidget::initInterface()
{
    this->setWindowTitle("TalkHub");
    QIcon window(":/Icon/global/talk.png");
    this->setWindowIcon(window);
    ui->emailLineEdit->setPlaceholderText("Email");
    // 添加左侧图标
    QIcon usericon(":/Icon/login/user.png");
    ui->emailLineEdit->addAction(usericon, QLineEdit::LeadingPosition);

    ui->passwdLineEdit->setPlaceholderText("Password");
    // 添加左侧图标
    QIcon passwdicon(":/Icon/login/passwd.png");
    ui->passwdLineEdit->addAction(passwdicon, QLineEdit::LeadingPosition);

    setupPasswordToggle(ui->passwdLineEdit, ":/Icon/login/visible.png", ":/Icon/login/unvisible.png");

    setupRegisterLabel();
    setupForgetPasswdLabel();

    QIcon icon = QIcon(":/Icon/login/line.png");
    QPixmap m_pic = icon.pixmap(icon.actualSize(QSize(300, 32)));//size自行调整
    ui->label->setPixmap(m_pic);

    // 固定窗口大小为 400x300
    this->setFixedSize(400, 300);

    ui->loginBtn->installEventFilter(this);
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
        emit sig_switchForget();
    });
}

void LoginWidget::loginFailed()
{

}

void LoginWidget::on_loginBtn_clicked()
{
    LoginFailedDialog* dialog = new LoginFailedDialog(this);
    connect(dialog, &LoginFailedDialog::sig_relogin, [this, dialog]{
        ui->passwdLineEdit->setText("");
        delete dialog;
    });

    connect(dialog, &LoginFailedDialog::sig_resetpasswd, [this, dialog]{
        emit sig_switchForget();
        delete dialog;
    });
    dialog->show();
}

