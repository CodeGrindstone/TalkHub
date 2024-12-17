#include "hovereventfilter.h"
#include <QEvent>

HoverEventFilter::HoverEventFilter(std::function<void(bool)> callback, QObject *parent)
    : QObject(parent), hoverCallback(callback) {}

bool HoverEventFilter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Enter) {
        hoverCallback(true); // 鼠标进入
        return true;
    } else if (event->type() == QEvent::Leave) {
        hoverCallback(false); // 鼠标离开
        return true;
    }
    return QObject::eventFilter(obj, event);
}
