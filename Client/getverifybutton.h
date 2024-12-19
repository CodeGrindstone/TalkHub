#ifndef GETVERIFYBUTTON_H
#define GETVERIFYBUTTON_H

#include <QPushButton>
#include <QTimer>
class GetVerifyButton : public QPushButton
{
public:
    explicit GetVerifyButton(QWidget *parent = nullptr, int time = 30);
private:
    QTimer* timer_;
    int remainingTime_;
private slots:
    void startTimer();
    void updateTimer();
};

#endif // GETVERIFYBUTTON_H
