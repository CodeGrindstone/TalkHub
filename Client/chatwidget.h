#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include "const.h"

namespace Ui {
class ChatWidget;
}

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);
    ~ChatWidget();
protected:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void on_sendBtn_clicked();
    void slot_switchChatPage();
    void slot_switchContactPage();

    void on_createGroupBtn_clicked();

private:
    void initInterface();
    void testAddfriend();
    void initTool();
private:
    Ui::ChatWidget *ui;
    ChatMode mode_;
};

#endif // CHATWIDGET_H
