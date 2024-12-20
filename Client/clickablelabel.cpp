#include "clickablelabel.h"
#include <QMouseEvent>

ClickableLabel::ClickableLabel(QWidget* parent) : QLabel(parent),_curstate(ClickLbState::Normal)
{
}
// 处理鼠标点击事件
void ClickableLabel::mousePressEvent(QMouseEvent* event)  {
    if (event->button() == Qt::LeftButton) {
        if(_curstate == ClickLbState::Normal){
            // qDebug()<<"clicked , change to selected hover: "<< _selected_hover;
            _curstate = ClickLbState::Selected;
            setProperty("state",_selected_hover);
            repolish(this);
            update();
        }else{
            //qDebug()<<"clicked , change to normal hover: "<< _normal_hover;
            _curstate = ClickLbState::Normal;
            setProperty("state",_normal_hover);
            repolish(this);
            update();
        }
        emit clicked();
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(event);
}

bool ClickableLabel::eventFilter(QObject *obj, QEvent *event)
{
    // 在这里处理鼠标悬停进入的逻辑
    if(event->type() == QEvent::Enter){
        if(_curstate == ClickLbState::Normal){
            qDebug()<<"enter , change to normal hover: "<< _normal_hover;
            setProperty("state",_normal_hover);
            repolish(this);
            update();
        }else{
            qDebug()<<"enter , change to selected hover: "<< _selected_hover;
            setProperty("state",_selected_hover);
            repolish(this);
            update();
        }
    }
    else if(event->type() == QEvent::Leave){
        // 在这里处理鼠标悬停离开的逻辑
        if(_curstate == ClickLbState::Normal){
            qDebug()<<"leave , change to normal : "<< _normal;
            setProperty("state",_normal);
            repolish(this);
            update();
        }else{
            qDebug()<<"leave , change to normal hover: "<< _selected;
            setProperty("state",_selected);
            repolish(this);
            update();
        }
    }
    return QLabel::eventFilter(obj, event);
}

// // 处理鼠标悬停离开事件
// void ClickableLabel::leaveEvent(QEvent* event){
//     // 在这里处理鼠标悬停离开的逻辑
//     if(_curstate == ClickLbState::Normal){
//         qDebug()<<"leave , change to normal : "<< _normal;
//         setProperty("state",_normal);
//         repolish(this);
//         update();
//     }else{
//         qDebug()<<"leave , change to normal hover: "<< _selected;
//         setProperty("state",_selected);
//         repolish(this);
//         update();
//     }
//     QLabel::leaveEvent(event);
// }
void ClickableLabel::SetState(QString normal, QString hover, QString press,
                            QString select, QString select_hover, QString select_press)
{
    _normal = normal;
    _normal_hover = hover;
    _normal_press = press;
    _selected = select;
    _selected_hover = select_hover;
    _selected_press = select_press;
    setProperty("state",normal);
    repolish(this);
}
ClickLbState ClickableLabel::GetCurState(){
    return _curstate;
}
