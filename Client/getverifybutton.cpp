#include "getverifybutton.h"


GetVerifyButton::GetVerifyButton(QWidget *parent, int time) : QPushButton(parent), remainingTime_(time)
{
    connect(this, &GetVerifyButton::clicked, this, &GetVerifyButton::startTimer);
}

void GetVerifyButton::startTimer()
{
    // 禁用按钮
    setEnabled(false);

    // 显示倒计时文本
    setText(QString("重新获取 (%1s)").arg(remainingTime_));

    // 启动定时器
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &GetVerifyButton::updateTimer);
    timer_->start(1000); // 每 1000 毫秒（1 秒）触发一次
}

void GetVerifyButton::updateTimer() {
    remainingTime_--;
    if (remainingTime_ > 0) {
        // 更新按钮文本
        setText(QString("重新获取 (%1s)").arg(remainingTime_));
    } else {
        // 倒计时结束
        timer_->stop();
        setText("获取验证码");
        setEnabled(true);
    }
}
