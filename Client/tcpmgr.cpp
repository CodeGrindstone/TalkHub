#include "tcpmgr.h"

#include "TcpMgr.h"
#include "usermgr.h"
#include <QNetworkProxy>
#include <QJsonDocument>
#include <QJsonObject>


void TcpMgr::connect(const QString &ip, const QString& port)
{
    // 尝试连接到服务器
    qDebug() << "Connecting to server...";

    m_socket.setProxy(QNetworkProxy::NoProxy);
    m_socket.connectToHost(ip, static_cast<unsigned short>(port.toUShort()));
}

TcpMgr::TcpMgr() : _b_recv_pending(false), m_message_id(0), m_message_len(0)
{
    QObject::connect(&m_socket, &QTcpSocket::connected, [&](){
        qDebug() << "connected to server!";
        //建立连接后发送信息
        emit sig_con_success(true);
    });

    QObject::connect(&m_socket, &QTcpSocket::readyRead, [&](){
        //当有数据可读时，读取所有数据
        m_buffer.append(m_socket.readAll());

        QDataStream stream(&m_buffer, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_6_0);

        forever{
            //解析头部数据
            if(!_b_recv_pending){
                //正在读取头部数据
                //读取到的数据小于头部数据长度
                if(m_buffer.size() < static_cast<int>(sizeof(quint16) * 2)){
                    return;
                }
                // 预读出消息Id和消息长度，但不从缓冲区移除
                stream >> m_message_id >> m_message_len;

                //将buffer中的前4个字节移除
                m_buffer = m_buffer.mid(sizeof(quint16)*2);

                //读出取出的数据
                //qDebug() << "Message ID: " << m_message_id << ", Message Len: " << m_message_len;
            }

            //头部解析完成
            if(m_buffer.size() < m_message_len){
                _b_recv_pending = true;
                return;
            }

            _b_recv_pending = false;
            // 读取消息体
            QByteArray messageBody = m_buffer.mid(0, m_message_len);
            //qDebug() << "receive body is " << messageBody;
            m_buffer = m_buffer.mid(m_message_len);
            handlemsg((ReqId)m_message_id, m_message_len, messageBody);
        }
    });

    QObject::connect(
        &m_socket,
        QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
        [&](QAbstractSocket::SocketError socketError) {
            Q_UNUSED(socketError)
            qDebug() << "Error:" << m_socket.errorString();
        });

    // 处理断开连接
    QObject::connect(&m_socket, &QTcpSocket::disconnected, [&](){
        qDebug() << "Disconnected from server";
    });

    QObject::connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);

    initHandlers();

}

void TcpMgr::RegHandler(ReqId id, FunCallBack callbk)
{
    m_handlers.insert(std::make_pair(id, callbk));
}

void TcpMgr::initHandlers()
{
    RegHandler(ID_CHAT_LOGIN_RSP, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
            // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        int err = jsonObj["code"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "Login Failed, err is " << err ;
            emit sig_login_failed(err);
            return;
        }

        emit sig_swich_chatdlg();
    });

    RegHandler(ID_SEARCH_USER_RSP, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        qDebug()<< "handle id is "<< id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();

        int err = jsonObj["code"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "search Failed, err is " << err ;
            emit sig_user_search(nullptr);
            return;
        }
        auto search_user_info = jsonObj["users_info"].toArray();

        int search_user_uid;
        QString search_user_name;
        QString search_user_email;
        Gender search_user_gender;
        QString search_user_profile_pic;
        QString search_user_bio;

        for(const QJsonValue& value : search_user_info)
        {
            QJsonObject userObj = value.toObject();

            search_user_uid = userObj["uid"].toInt();
            search_user_name = userObj["name"].toString();
            search_user_email = userObj["email"].toString();
            search_user_gender = static_cast<Gender>(userObj["gender"].toInt());
            search_user_profile_pic = userObj["profile_picture"].toString();
            search_user_bio = userObj["bio"].toString();
        }

        auto user_info = std::make_shared<UserInfo>(
            search_user_uid, search_user_name,
            search_user_email,search_user_gender,
            search_user_profile_pic,search_user_bio
        );

        UserMgr::GetInstance()->setUserInfo(search_user_uid, user_info);

        emit sig_user_search(user_info);
    });


    RegHandler(ID_NOTIFY_ADD_FRIEND_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        int err = jsonObj["code"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "notify add friend request failed, err is " << err;
        }
        //     emit sig_user_search(nullptr);
        //     return;
        // }

        auto add_friend_req_user_info = jsonObj["users_info"].toArray();
        auto add_friend_req_message = jsonObj["request_message"].toString();

        int add_friend_req_user_uid;
        QString add_friend_req_user_name;
        QString add_friend_req_user_email;
        Gender add_friend_req_user_gender;
        QString add_friend_req_user_profile_pic;
        QString add_friend_req_user_bio;

        for(const QJsonValue& value : add_friend_req_user_info)
        {
            QJsonObject userObj = value.toObject();

            add_friend_req_user_uid = userObj["uid"].toInt();
            add_friend_req_user_name = userObj["name"].toString();
            add_friend_req_user_email = userObj["email"].toString();
            add_friend_req_user_gender = static_cast<Gender>(userObj["gender"].toInt());
            add_friend_req_user_profile_pic = userObj["profile_picture"].toString();
            add_friend_req_user_bio = userObj["bio"].toString();
        }

        auto user_info = std::make_shared<UserInfo>(
            add_friend_req_user_uid, add_friend_req_user_name,
            add_friend_req_user_email,add_friend_req_user_gender,
            add_friend_req_user_profile_pic,add_friend_req_user_bio
        );

        UserMgr::GetInstance()->setUserInfo(add_friend_req_user_uid, user_info);

        emit sig_friend_apply(std::make_shared<AddFriendApply>
                              (user_info, add_friend_req_message));
    });


    RegHandler(ID_AUTH_FRIEND_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }

        int new_friend_uid = jsonObj["new_friend_uid"].toInt();
        emit sig_auth_rsp(new_friend_uid);
        qDebug() << "Auth Friend Success " ;
    });


    RegHandler(ID_NOTIFY_AUTH_FRIEND_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        int err = jsonObj["code"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            return;
        }

        int uid = jsonObj["touid"].toInt();
        if(uid == UserMgr::GetInstance()->getUid())
        {
            int friend_uid = jsonObj["fromuid"].toInt();
            qDebug() << friend_uid << "同意了好友申请";
            emit sig_auth_rsp(friend_uid);
            qDebug() << "Auth Friend Success " ;
        }
    });



    RegHandler(ID_TEXT_CHAT_MSG_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        int err = jsonObj["code"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            return;
        }

        qDebug() << "Receive Text Chat Rsp Success " ;
        //ui设置送达等标记 todo...
    });

    RegHandler(ID_NOTIFY_TEXT_CHAT_MSG_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        int err = jsonObj["code"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            return;
        }

        if(jsonObj["type"].toString() == "text" && jsonObj["touid"].toInt() == UserMgr::GetInstance()->getUid())
        {
            qDebug() << "Receive Text Chat Notify Success " ;
            auto receive_msg = std::make_shared<TextChatMsg>(
                jsonObj["fromuid"].toInt(),
                false,
                jsonObj["content"].toString()
            );

            emit sig_text_chat_msg(receive_msg);
        }
    });
}

void TcpMgr::handlemsg(ReqId id, int len, QByteArray data)
{
    auto find_iter = m_handlers.find(id);
    if(find_iter == m_handlers.end()){
        qDebug() << "NOT FOUND handle id is " << id;
        return;
    }
    m_handlers[id](id, len, data);
}


void TcpMgr::slot_send_data(ReqId Id, QString data)
{
    uint16_t id = static_cast<uint16_t>(Id);

    QByteArray databytes = data.toUtf8();

    //计算长度
    uint16_t len = static_cast<quint16>(data.size());

    //创建一个QByteArray用来存储要发送的数据
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    //设置数据流使用网络字节序
    out.setByteOrder(QDataStream::BigEndian);

    //写入ID和长度
    out << id << len;

    //添加字符串数据
    block.append(databytes);

    //发送数据
    m_socket.write(block);
}
