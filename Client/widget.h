#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "loginwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private:
    void switchLogin();
private:
    Ui::Widget *ui;
    LoginWidget* m_login;
private slots:
    void slot_switchRegister();
    void slot_switchForget();

};
#endif // WIDGET_H
