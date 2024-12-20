#ifndef SIDEFRAME_H
#define SIDEFRAME_H

#include <QFrame>
#include <QLabel>
#include "sideframeitem.h"
namespace Ui {
class SideFrame;
}

class SideFrame : public QFrame
{
    Q_OBJECT

public:
    explicit SideFrame(QWidget *parent = nullptr);
    ~SideFrame();
protected:
    bool eventFilter(QObject *watched, QEvent *event);
private:
    void initInterface();
private:
    Ui::SideFrame *ui;
    SideFrameItem* messageLabel_;
    SideFrameItem* friendLabel_;
};

#endif // SIDEFRAME_H
