#ifndef HOVEREVENTFILTER_H
#define HOVEREVENTFILTER_H

#include <QObject>
#include <functional>

class HoverEventFilter : public QObject {
    Q_OBJECT
public:
    explicit HoverEventFilter(std::function<void(bool)> callback, QObject *parent = nullptr);
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    std::function<void(bool)> hoverCallback;
};


#endif // HOVEREVENTFILTER_H
