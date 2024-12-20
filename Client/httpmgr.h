#ifndef HTTPMGR_H
#define HTTPMGR_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include "Singleton.h"
#include "registerwidget.h"
#include "resetwidget.h"
#include "const.h"
#include "loginwidget.h"


class HttpMgr : public QObject, public Singleton<HttpMgr>, public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
public:
    ~HttpMgr();
private:
    friend class Singleton;
    friend class LoginWidget;
    friend class RegisterWidget;
    friend class ResetWidget;
    HttpMgr();
    QNetworkAccessManager m_manager;
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod); // 发送Http的post请求

private slots:
    void slot_http_finish(ReqId id, QString res, bool recived, Modules mod);

signals:
    void sig_http_finish(ReqId id, QString res, bool recived, Modules mod); // 发送完http请求的信号
    void sig_reg_mod_finish(ReqId id, QString res, bool recived);
    void sig_reset_mod_finish(ReqId id, QString res, bool recived);
    void sig_login_mod_finish(ReqId id, QString res, bool recived);
};
#endif // HTTPMGR_H
