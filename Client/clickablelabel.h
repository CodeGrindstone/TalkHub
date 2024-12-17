#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QMouseEvent>
#include "const.h"
class ClickableLabel:public QLabel
{
    Q_OBJECT
public:
    ClickableLabel(QWidget* parent);
    virtual void mousePressEvent(QMouseEvent *ev) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    // virtual void leaveEvent(QEvent* event) override;
    void SetState(QString normal="", QString hover="", QString press="",
                  QString select="", QString select_hover="", QString select_press="");
    ClickLbState GetCurState();
protected:
private:
    QString _normal;
    QString _normal_hover;
    QString _normal_press;
    QString _selected;
    QString _selected_hover;
    QString _selected_press;
    ClickLbState _curstate;
signals:
    void clicked(void);
};

#endif // CLICKABLELABEL_H
