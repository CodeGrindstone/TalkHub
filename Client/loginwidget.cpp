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
#include <QMessageBox>
#include <QJsonDocument>
#include "const.h"
#include "httpmgr.h"
#include "tcpmgr.h"
#include "usermgr.h"

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget)
{
    ui->setupUi(this);

    initInterface();

    initHttpHandlers();

    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_login_mod_finish,
            this, &LoginWidget::slot_login_mod_finish);

    connect(this, &LoginWidget::sig_send_data, TcpMgr::GetInstance().get(), &TcpMgr::sig_send_data);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_swich_chatdlg, this, &LoginWidget::sig_switchChat);
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

void LoginWidget::initHttpHandlers()
{
    m_handlers.insert(ReqId::ID_LOGIN_USER, [this](const QJsonObject& JsonObj){
        ErrorCodes code = static_cast<ErrorCodes>(JsonObj["code"].toInt());
        switch (code){
        case ErrorCodes::SUCCESS:{
            QMessageBox::information(this, "提示", "您登陆成功");

            // TODO...加载用户信息
            auto myself = std::make_shared<UserInfo>(
                JsonObj["uid"].toInt(),
                JsonObj["name"].toString(),
                JsonObj["email"].toString(),
                static_cast<Gender>(JsonObj["gender"].toInt()),
                JsonObj["profile_picture"].toString(),
                JsonObj["bio"].toString()
            );

            UserMgr::GetInstance()->setMyselfUserInfo(myself);
            UserMgr::GetInstance()->setToken(JsonObj["token"].toString());
            UserMgr::GetInstance()->setChatServer(JsonObj["chatserver"].toString());
            // 加载其他用户信息
            auto users_info = JsonObj["users_info"].toArray();

            for(const QJsonValue& value : users_info)
            {
                QJsonObject userObj = value.toObject();

                auto user_info = std::make_shared<UserInfo>(
                    userObj["uid"].toInt(), userObj["name"].toString(),
                    userObj["email"].toString(),static_cast<Gender>(userObj["gender"].toInt()),
                    userObj["profile_picture"].toString(),userObj["bio"].toString()
                );


                UserMgr::GetInstance()->setUserInfo(userObj["uid"].toInt(), user_info);
            }


            // 加载好友信息
            auto friends_id = JsonObj["friends_id"].toArray();
            for(const QJsonValue& value : friends_id)
            {
                int uid = value.toInt();

                UserMgr::GetInstance()->AddFriend(uid);
            }

            // 加载收到用户添加好友请求信息
            auto apply_add_friend_info = JsonObj["apply_add_friend_info"].toArray();
            for(const QJsonValue& value : apply_add_friend_info)
            {
                QJsonObject apply_info = value.toObject();

                int uid = apply_info["uid"].toInt();

                UserMgr::GetInstance()->AddFriendApplyInfo(
                    uid,
                    std::make_shared<AddFriendApply>(
                        UserMgr::GetInstance()->getUserInfo(uid),
                        apply_info["request_messages"].toString(),
                        apply_info["statu"].toString())
                );
            }

            // 加载信息
            auto send_friend_msg = JsonObj["send_msg"].toArray();
            for(const QJsonValue& value : send_friend_msg)
            {
                QJsonObject msg_info = value.toObject();
                int to_uid = msg_info["receiver_uid"].toInt();
                QString content = msg_info["content"].toString();
                UserMgr::GetInstance()->AppendFriendChatMsg(
                    to_uid,
                    std::make_shared<TextChatMsg>(to_uid, true, content)
                );

            }

            auto recv_friend_msg = JsonObj["recv_msg"].toArray();
            for(const QJsonValue& value : recv_friend_msg)
            {
                QJsonObject msg_info = value.toObject();
                int from_uid = msg_info["sender_uid"].toInt();
                QString content = msg_info["content"].toString();
                UserMgr::GetInstance()->AppendFriendChatMsg(
                    from_uid,
                    std::make_shared<TextChatMsg>(from_uid, false, content)
                    );
            }

            UserMgr::GetInstance()->setIp_Port(JsonObj["host"].toString(), JsonObj["port"].toString());

            loginChatServer(UserMgr::GetInstance()->getHost(), UserMgr::GetInstance()->getPort());

            break;
        }
        case ErrorCodes::ERR_INVALID_PARAMS:{
            QMessageBox::critical(this, "错误", "参数错误");
            break;
        }
        case ErrorCodes::ERR_USER_NOT_FOUND:{   // 用户查找不到
            showLoginFailedDialog();
            break;
        }
        case ErrorCodes::ERR_PASSWORD_MISMATCH:{// 密码不匹配
            showLoginFailedDialog();
            break;
        }
        default:{
            QMessageBox::critical(this, "错误", "未知错误");
            break;
        }
        }
    });
}

void LoginWidget::showLoginFailedDialog()
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

void LoginWidget::loginChatServer(QString host, QString ip)
{
    TcpMgr::GetInstance()->connect(host, ip);

    QJsonObject json_obj;
    json_obj["uid"] = UserMgr::GetInstance()->getUid();
    json_obj["token"] = UserMgr::GetInstance()->getToken();
    json_obj["chatserver"] = UserMgr::GetInstance()->getChatServer();
    // 使用 QJsonDocument 将 QJsonObject 转化为 JSON 格式的字符串
    QJsonDocument doc(json_obj);
    emit sig_send_data(ID_CHAT_LOGIN, QString(doc.toJson()));

    // TODO..利用用户信息登陆聊天服务器
}

void LoginWidget::on_loginBtn_clicked()
{
    // if(false)
    // {
    //     LoginFailedDialog* dialog = new LoginFailedDialog(this);
    //     connect(dialog, &LoginFailedDialog::sig_relogin, [this, dialog]{
    //         ui->passwdLineEdit->setText("");
    //         delete dialog;
    //     });

    //     connect(dialog, &LoginFailedDialog::sig_resetpasswd, [this, dialog]{
    //         emit sig_switchForget();
    //         delete dialog;
    //     });
    //     dialog->show();
    // }

    // 原本代码
    QJsonObject json_obj;
    json_obj["email"] = ui->emailLineEdit->text();
    json_obj["passwd"] = ui->passwdLineEdit->text();
    HttpMgr::GetInstance()->PostHttpReq(
        QUrl(gate_url_prefix + "/api/v1/auth/login"),
        json_obj, ReqId::ID_LOGIN_USER, Modules::LoginMod
    );

    // test
    //emit sig_switchChat();
}

void LoginWidget::slot_login_mod_finish(ReqId id, QString res, bool recived)
{
    if(!recived)
    {
        QMessageBox::critical(this, "错误", "无法连接到服务器");
        return;
    }
    //解析JSon字符串 res转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());

    // 将json文档转化为json对象  jsonDoc.object()
    m_handlers[id](jsonDoc.object());
}

