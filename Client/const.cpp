#include "const.h"
#include <QToolButton>
#include <QHBoxLayout>
// 封装函数：添加密码显示/隐藏功能
void setupPasswordToggle(QLineEdit *lineEdit, const QString &visibleIconPath, const QString &invisibleIconPath) {
    // 初始状态：密码隐藏
    lineEdit->setEchoMode(QLineEdit::Password);

    // 创建 QToolButton 作为切换按钮
    QToolButton *toggleButton = new QToolButton(lineEdit);
    toggleButton->setCursor(Qt::PointingHandCursor); // 设置鼠标为手势
    toggleButton->setIcon(QIcon(invisibleIconPath)); // 初始状态：眼睛关闭图标
    toggleButton->setStyleSheet("QToolButton { border: none; padding: 0px; }"); // 去除边框

    // 创建布局，并将 QToolButton 添加到 QLineEdit 的右侧
    QHBoxLayout *layout = new QHBoxLayout(lineEdit);
    layout->setContentsMargins(0, 0, 5, 0); // 设置布局边距
    layout->addStretch();                  // 左侧自动填充
    layout->addWidget(toggleButton);       // 将按钮放在最右侧
    layout->setAlignment(Qt::AlignRight);  // 右对齐按钮

    // 状态变量
    bool isPasswordVisible = false;

    // 点击事件：切换密码显示/隐藏状态
    QObject::connect(toggleButton, &QToolButton::clicked, [=]() mutable {
        isPasswordVisible = !isPasswordVisible;

        if (isPasswordVisible) {
            lineEdit->setEchoMode(QLineEdit::Normal);  // 显示密码
            toggleButton->setIcon(QIcon(visibleIconPath)); // 切换到眼睛打开图标
        } else {
            lineEdit->setEchoMode(QLineEdit::Password); // 隐藏密码
            toggleButton->setIcon(QIcon(invisibleIconPath)); // 切换到眼睛关闭图标
        }
    });
}

std::function<void(QWidget*)> repolish = [](QWidget* w){
    w->style()->unpolish(w);
    w->style()->polish(w);
};
