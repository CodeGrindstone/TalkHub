#include "registerwidget.h"
#include "ui_registerwidget.h"
#include "httpmgr.h"
#include <QMessageBox>
RegisterWidget::RegisterWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegisterWidget)
    , isShowPasswdLb(false)
{
    ui->setupUi(this);

    initInterface();

    initHttpHandlers();

    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish,
            this, &RegisterWidget::slot_reg_mod_finish);

}

RegisterWidget::~RegisterWidget()
{
    delete ui;
}

bool RegisterWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->registernowBtn)
    {
        if(event->type() == QEvent::Enter)
        {
            if(isValidPassword_ && isValidEmail_ && ui->regisVerifyEdit->text().length() > 0)
            {
                ui->registernowBtn->setEnabled(true);
            }
            else
            {
                // 加载自定义的图标
                QPixmap pixmap(":/Icon/register/forbidden.png");  // 替换为你自己的图片路径
                QCursor cursor(pixmap);

                // 设置当前窗口的鼠标指针为自定义图标
                ui->registernowBtn->setEnabled(false);
                this->setCursor(cursor);
            }
        }
        else if(event->type() == QEvent::Leave)
        {
            this->setCursor(Qt::ArrowCursor);
        }
    }
    else if(watched == ui->getVerifyBtn)
    {
        if(event->type() == QEvent::Enter)
        {
            if(isValidEmail_)
            {
                ui->getVerifyBtn->setEnabled(true);
            }
            else
            {
                // 加载自定义的图标
                QPixmap pixmap(":/Icon/register/forbidden.png");  // 替换为你自己的图片路径
                QCursor cursor(pixmap);

                // 设置当前窗口的鼠标指针为自定义图标
                ui->getVerifyBtn->setEnabled(false);
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

void RegisterWidget::initInterface()
{
    this->setWindowTitle("TalkHub");
    QIcon window(":/Icon/global/talk.png");
    this->setWindowIcon(window);

    setupDynamicLineEditIcon(ui->regisEmailEdit, [this](const QString &email){
            isValidEmail_ = isValidEmail(email);
            return isValidEmail_;
        },
        ":/Icon/register/yes.png", ":/Icon/register/error.png");

    setupDynamicLineEditIcon(ui->regisPasswdEdit, [this](const QString &passwd){
            isValidPassword_ = isValidPassword(passwd);
            return isValidPassword_;
        },
        ":/Icon/register/yes.png", ":/Icon/register/error.png");

    // 创建 QLabel 显示输入格式提示
    hintLabel = new QLabel("不能包含空格\n"
                            "长度为8-16个字符\n"
                            "只能包含字母、数字、符号\n");
    hintLabel->setFixedSize(200, 60); // 设置宽度为200，高度为50
    hintLabel->setStyleSheet("color: gray; font-size: 12px;");


    connect(ui->regisEmailEdit, &QLineEdit::textChanged, [this]{
        if(isShowPasswdLb){
            ui->mainLayout->removeWidget(hintLabel); // 从布局中移除 label2
            hintLabel->hide();
        }
        isShowPasswdLb = false;
    });

    connect(ui->regisPasswdEdit, &QLineEdit::textEdited, [this]{
        ui->mainLayout->insertWidget(3, hintLabel);
        hintLabel->show();
        isShowPasswdLb = true;
    });

    connect(ui->regisVerifyEdit, &QLineEdit::textChanged, [this]{
        if(isShowPasswdLb){
            ui->mainLayout->removeWidget(hintLabel); // 从布局中移除 label2
            hintLabel->hide();
        }
        isShowPasswdLb = false;
    });

    connect(ui->registerReturnBtn, &QPushButton::clicked, [this]{
        emit sig_switchLogin();
    });

    ui->getVerifyBtn->installEventFilter(this);
    ui->registernowBtn->installEventFilter(this);
}



void RegisterWidget::on_getVerifyBtn_clicked()
{
    const QString& email = ui->regisEmailEdit->text();
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
    json_obj["type"] = VerifyType::REGISTER;
    HttpMgr::GetInstance()->PostHttpReq(
        QUrl(gate_url_prefix + "/api/v1/auth/verification/code"),
        json_obj, ReqId::ID_GET_VERIFY_CODE, Modules::RegisterMod
    );
}

void RegisterWidget::initHttpHandlers()
{
    m_handlers.insert(ReqId::ID_GET_VERIFY_CODE, [this](const QJsonObject& JsonObj){
        ErrorCodes code = static_cast<ErrorCodes>(JsonObj["code"].toInt());
        switch (code){
        case ErrorCodes::SUCCESS:{
            QMessageBox::information(this, "提示", "验证码已经发送到您的邮箱");
            break;
        }
        case ErrorCodes::ERR_EMAIL_ALREADY_REGISTERED:{
            QMessageBox::critical(this, "错误", "此邮箱已被注册");
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

    m_handlers.insert(ReqId::ID_REG_USER, [this](const QJsonObject& JsonObj){
        ErrorCodes code = static_cast<ErrorCodes>(JsonObj["code"].toInt());

        switch (code){
            case ErrorCodes::SUCCESS:{
                int uid = JsonObj["id"].toInt();
                qDebug() << "uid:" << uid;
                QMessageBox::information(this, "提示", "您已注册成功！");
                break;
            }
            case ErrorCodes::ERR_INVALID_PARAMS:{
                QMessageBox::critical(this, "错误", "参数错误");
                break;
            }
            case ErrorCodes::ERR_VERIFICATION_EXPIRED:{
                QMessageBox::critical(this, "错误", "验证码已过期");
                break;
            }
            case ErrorCodes::ERR_VERIFICATION_INVALID:{
                QMessageBox::critical(this, "错误", "验证码错误");
                break;
            }
            case ErrorCodes::ERR_ACCOUNT_ALREADY_EXISTS:{
                QMessageBox::critical(this, "错误", "邮箱已被注册");
                break;
            }
            default:{
                QMessageBox::critical(this, "错误", "未知错误");
                break;
            }
        }
    });
}


void RegisterWidget::slot_reg_mod_finish(ReqId id, QString res, bool recived)
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


void RegisterWidget::on_registernowBtn_clicked()
{
    const QString& email = ui->regisEmailEdit->text();
    const QString& pwd = ui->regisPasswdEdit->text();
    const QString& verifyCode = ui->regisVerifyEdit->text();
    QJsonObject json_obj;
    json_obj["email"] = email;
    json_obj["passwd"] = pwd;
    json_obj["verifyCode"] = verifyCode;
    HttpMgr::GetInstance()->PostHttpReq(
        QUrl(gate_url_prefix + "/api/v1/auth/register"),
        json_obj, ReqId::ID_REG_USER, Modules::RegisterMod
    );
}

