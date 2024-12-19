#include "httpmgr.h"

HttpMgr::HttpMgr()
{
    connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}

void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    // 创建一个HTTP Post请求，并设置请求头和请求体
    QByteArray data = QJsonDocument(json).toJson();
    // 通过url构造请求
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    // 发送请求，并处理响应，获取自己的智能指针，构造伪闭包增加智能指针引用计数
    auto self = shared_from_this();
    QNetworkReply* reply = m_manager.post(request, data);
    // 设置信号和槽等待发送完成
    QObject::connect(reply, &QNetworkReply::finished, [reply, self, req_id, mod](){
        //处理错误的情况
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            //发送信号通知完成
            emit self->sig_http_finish(req_id, "", false, mod);
            reply->deleteLater();
            return;
        }
        //无错误则读回请求
        QString res = reply->readAll();
        //发送信号通知完成
        emit self->sig_http_finish(req_id, res, true, mod);
        reply->deleteLater();
        return;
    });
}

void HttpMgr::slot_http_finish(ReqId id, QString res, bool recived, Modules mod)
{
    // 发送信号通知指定模块的http的响应结束了
    if(mod == Modules::RegisterMod)
    {
        emit sig_reg_mod_finish(id, res, recived);
    }

    if(mod == Modules::ForgetPasswdMod)
    {
        emit sig_reset_mod_finish(id, res, recived);
    }

    if(mod == Modules::LoginMod){
        emit sig_login_mod_finish(id, res, recived);
    }
}

HttpMgr::~HttpMgr()
{

}


