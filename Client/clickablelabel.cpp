#include "clickablelabel.h"
#include <QMouseEvent>

ClickableLabel::ClickableLabel(QWidget* parent) : QLabel(parent),_curstate(ClickLbState::Normal)
{
    this->installEventFilter(this);
}
// 处理鼠标点击事件
void ClickableLabel::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if(_curstate == ClickLbState::Normal){
            _curstate = ClickLbState::Selected;
            setProperty("state",_selected_press);
            repolish(this);
            update();

        }else{
            _curstate = ClickLbState::Normal;
            setProperty("state",_normal_press);
            repolish(this);
            update();
        }
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(event);
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if(_curstate == ClickLbState::Normal){
            setProperty("state",_normal_hover);
            repolish(this);
            update();
        }else{
            setProperty("state",_selected_hover);
            repolish(this);
            update();
        }
        emit clicked(this->text(), _curstate);
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(event);
}

void ClickableLabel::enterEvent(QEnterEvent *event)
{
    // 在这里处理鼠标悬停进入的逻辑
    if(_curstate == ClickLbState::Normal){
        setProperty("state",_normal_hover);
        repolish(this);
        update();

    }else{
        setProperty("state",_selected_hover);
        repolish(this);
        update();
    }

    QLabel::enterEvent(event);
}

// 处理鼠标悬停离开事件
void ClickableLabel::leaveEvent(QEvent* event){
    // 在这里处理鼠标悬停离开的逻辑
    if(_curstate == ClickLbState::Normal){
        setProperty("state",_normal);
        repolish(this);
        update();
    }else{
        setProperty("state",_selected);
        repolish(this);
        update();
    }
    QLabel::leaveEvent(event);
}


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

bool ClickableLabel::SetCurState(ClickLbState state)
{
    _curstate = state;
    if (_curstate == ClickLbState::Normal) {
        setProperty("state", _normal);
        repolish(this);
    }
    else if (_curstate == ClickLbState::Selected) {
        setProperty("state", _selected);
        repolish(this);
    }

    return true;
}

void ClickableLabel::ResetNormalState()
{
    _curstate = ClickLbState::Normal;
    setProperty("state", _normal);
    repolish(this);
}
