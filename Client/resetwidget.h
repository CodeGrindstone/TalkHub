#ifndef RESETWIDGET_H
#define RESETWIDGET_H

#include <QWidget>
#include "const.h"
#include <QLabel>
#include <QMap>
#include <QJsonObject>
namespace Ui {
class ResetWidget;
}

class ResetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResetWidget(QWidget *parent = nullptr);
    ~ResetWidget();
private:
    void initInterface();
    void initHttpHandlers();

private:
    Ui::ResetWidget *ui;
    QLabel* hintLabel;
    bool isShowPasswdLb;
    QMap<ReqId, std::function<void(const QJsonObject&)>> m_handlers;
    bool isValidEmail_;
    bool isValidPassword_;
    bool isValidConfirm_;
signals:
    void sig_switchLogin();
private slots:
    void on_getVerifyBtn_clicked();
    void slot_reset_mod_finish(ReqId id, QString res, bool recived);
    void on_resetConfirmBtn_clicked();
};

#endif // RESETWIDGET_H
