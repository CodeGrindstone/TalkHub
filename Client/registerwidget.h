#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QJsonObject>
#include <QMap>
#include "const.h"
#include "getverifybutton.h"

namespace Ui {
class RegisterWidget;
}

class RegisterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWidget(QWidget *parent = nullptr);
    ~RegisterWidget();

protected:
    bool eventFilter(QObject *watched, QEvent *event);


private:
    void initInterface();
    void sendRequest();
    void initHttpHandlers();
private:
    Ui::RegisterWidget *ui;
    QLabel* hintLabel;
    bool isShowPasswdLb;
    QMap<ReqId, std::function<void(const QJsonObject&)>> m_handlers;
    bool isValidEmail_;
    bool isValidPassword_;

signals:
    void sig_switchLogin();
private slots:
    void on_getVerifyBtn_clicked();
    void slot_reg_mod_finish(ReqId id, QString res, bool recived);
    void on_registernowBtn_clicked();
};

#endif // REGISTERWIDGET_H
