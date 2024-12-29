#ifndef TCPMGR_H
#define TCPMGR_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include "Singleton.h"
#include "const.h"
#include <functional>
#include <QByteArray>
#include "userdata.h"

typedef std::function<void(ReqId, int, QByteArray)> FunCallBack;

class TcpMgr : public QObject, public Singleton<TcpMgr>, std::enable_shared_from_this<TcpMgr>{
    Q_OBJECT

    friend class Singleton<TcpMgr>;

public:
    void connect(const QString& ip, const QString& port);

private:
    TcpMgr();
    void RegHandler(ReqId id, FunCallBack callbk);
    void initHandlers();
    void handlemsg(ReqId id, int len, QByteArray data);

private:
    QTcpSocket m_socket;
    QByteArray m_buffer;
    bool _b_recv_pending;
    quint16 m_message_id;
    quint16 m_message_len;
    std::map<ReqId, FunCallBack> m_handlers;

signals:
    void sig_con_success(bool bsuccess);
    void sig_send_data(ReqId reqId, QString data);
    void sig_login_failed(int);
    void sig_swich_chatdlg();
    void sig_user_search(std::shared_ptr<UserInfo>);

    void sig_friend_apply(std::shared_ptr<AddFriendApply>);     // 其他人请求添加好友
    void sig_add_auth_friend(int uid);      // 收到对方的同意
    void sig_auth_rsp(int uid);            // 自己同意
    void sig_text_chat_msg(std::shared_ptr<TextChatMsg>);

public slots:
    void slot_send_data(ReqId Id, QString data);
};
#endif // TCPMGR_H
