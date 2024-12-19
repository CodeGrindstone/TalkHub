#include "resetwidget.h"
#include "ui_resetwidget.h"
#include "httpmgr.h"
#include <QMessageBox>

ResetWidget::ResetWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ResetWidget)
{
    ui->setupUi(this);

    initInterface();

    initHttpHandlers();

    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reset_mod_finish,
            this, &ResetWidget::slot_reset_mod_finish);
}

ResetWidget::~ResetWidget()
{
    delete ui;
}

void ResetWidget::initInterface()
{
    this->setWindowTitle("TalkHub");
    QIcon window(":/Icon/global/talk.png");
    this->setWindowIcon(window);

    setupDynamicLineEditIcon(ui->resetEmailEdit, [this](const QString &email){
            isValidEmail_ = isValidEmail(email);
            return isValidEmail_;
        },
        ":/Icon/register/yes.png", ":/Icon/register/error.png");

    setupDynamicLineEditIcon(ui->resetPasswdEdit, [this](const QString &email){
            isValidPassword_ = isValidPassword(email);
            return isValidPassword_;
        },
        ":/Icon/register/yes.png", ":/Icon/register/error.png");

    auto confrimCheck = [this](const QString& lineedit){
        return isValidConfirm_=(lineedit == this->ui->resetPasswdEdit->text());
    };

    ui->resetConfrimEdit->setEchoMode(QLineEdit::Password);
    setupDynamicLineEditIcon(ui->resetConfrimEdit, confrimCheck,
                             ":/Icon/register/yes.png", ":/Icon/register/error.png");


    // 创建 QLabel 显示输入格式提示
    hintLabel = new QLabel("不能包含空格\n"
                           "长度为8-16个字符\n"
                           "只能包含字母、数字、符号\n");
    hintLabel->setFixedSize(200, 60); // 设置宽度为200，高度为50
    hintLabel->setStyleSheet("color: gray; font-size: 12px;");


    connect(ui->resetEmailEdit, &QLineEdit::textChanged, [this]{
        if(isShowPasswdLb){
            ui->mainLayout->removeWidget(hintLabel); // 从布局中移除 label2
            hintLabel->hide();
        }
        isShowPasswdLb = false;
    });

    connect(ui->resetPasswdEdit, &QLineEdit::textEdited, [this]{
        ui->mainLayout->insertWidget(2, hintLabel);
        hintLabel->show();
        isShowPasswdLb = true;
    });

    connect(ui->resetVerifyEdit, &QLineEdit::textChanged, [this]{
        if(isShowPasswdLb){
            ui->mainLayout->removeWidget(hintLabel); // 从布局中移除 label2
            hintLabel->hide();
        }
        isShowPasswdLb = false;
    });

    connect(ui->resetReturnBtn, &QPushButton::clicked, [this]{
        emit sig_switchLogin();
    });
}

void ResetWidget::initHttpHandlers()
{
    m_handlers.insert(ReqId::ID_GET_VERIFY_CODE, [this](const QJsonObject& JsonObj){
        ErrorCodes code = static_cast<ErrorCodes>(JsonObj["code"].toInt());
        switch (code){
        case ErrorCodes::SUCCESS:{
            QMessageBox::information(this, "提示", "验证码已经发送到您的邮箱");
            break;
        }
        case ErrorCodes::ERR_USER_NOT_FOUND:{
            QMessageBox::critical(this, "错误", "此邮箱未被注册过");
            break;
        }
        case ErrorCodes::ERR_INVALID_PARAMS:{
            QMessageBox::critical(this, "错误", "参数错误");
            break;
        }
        default:{
            QMessageBox::critical(this, "错误", "未知错误");
            break;
        }
        }
    });

    m_handlers.insert(ReqId::ID_RESET_PWD, [this](const QJsonObject& JsonObj){
        ErrorCodes code = static_cast<ErrorCodes>(JsonObj["code"].toInt());
        switch (code){
        case ErrorCodes::SUCCESS:{
            QMessageBox::information(this, "提示", "密码修改成功！");
            break;
        }
        case ErrorCodes::ERR_USER_NOT_FOUND:{
            QMessageBox::critical(this, "错误", "此邮箱未被注册过");
            break;
        }
        case ErrorCodes::ERR_TOO_MANY_CONNECTIONS:{
            QMessageBox::critical(this, "错误", "服务器忙碌，请重试");
            break;
        }
        case ErrorCodes::ERR_INVALID_PARAMS:{
            QMessageBox::critical(this, "错误", "参数错误");
            break;
        }
        default:{
            QMessageBox::critical(this, "错误", "未知错误");
            break;
        }
        }
    });
}

void ResetWidget::on_getVerifyBtn_clicked()
{
    const QString& email = ui->resetEmailEdit->text();
    if(email.length() == 0){
        QMessageBox::critical(this, "错误", "邮箱为空");
        return;
    }
    if(!isValidEmail(email)){
        QMessageBox::critical(this, "错误", "您输入的邮箱格式有误");
        return;
    }
    QJsonObject json_obj;
    json_obj["email"] = email;
    json_obj["type"] = VerifyType::RESET_PASSWD;
    HttpMgr::GetInstance()->PostHttpReq(
        QUrl(gate_url_prefix + "/api/v1/auth/verification/code"),
        json_obj, ReqId::ID_GET_VERIFY_CODE, Modules::ForgetPasswdMod
    );
}

void ResetWidget::slot_reset_mod_finish(ReqId id, QString res, bool recived)
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


void ResetWidget::on_resetConfirmBtn_clicked()
{
    if(!isValidEmail_ || !isValidPassword_ || !isValidConfirm_ || !( ui->resetVerifyEdit->text().length() > 0))
    {
        QMessageBox::critical(this, "错误", "输入格式有误");
        return;
    }
    const QString& email = ui->resetEmailEdit->text();
    const QString& pwd = ui->resetPasswdEdit->text();
    const QString& verifyCode = ui->resetVerifyEdit->text();
    QJsonObject json_obj;
    json_obj["email"] = email;
    json_obj["passwd"] = pwd;
    json_obj["verifyCode"] = verifyCode;
    HttpMgr::GetInstance()->PostHttpReq(
        QUrl(gate_url_prefix + "/api/v1/auth/reset_passwd"),
        json_obj, ReqId::ID_RESET_PWD, Modules::ForgetPasswdMod
    );
}

