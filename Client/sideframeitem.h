#ifndef SIDEFRAMEITEM_H
#define SIDEFRAMEITEM_H

#include <QLabel>
#include <QString>
#include <QCursor>
#include <QEvent>
class SideFrameItem : public QLabel
{
public:
    explicit SideFrameItem(QWidget *parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags());
    void setPic(QString normal, QString clicked);
    void showNormalPic();
    void showClickedPic();
protected:

    bool event(QEvent *e);

private:
    bool clicked;
    QString normal_;
    QString clicked_;
};

#endif // SIDEFRAMEITEM_H
