#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include "chatfrienditem.h"

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

private:
    void initInterface();
    void testAddfriend();
    void initTool();
private:
    Ui::ChatWidget *ui;
};

#endif // CHATWIDGET_H
