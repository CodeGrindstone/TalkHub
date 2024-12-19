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


void setupDynamicLineEditIcon(QLineEdit *lineEdit,
                              std::function<bool(const QString&)> checkInput,
                              const QString &successIconPath,
                              const QString &errorIconPath) {
    // 初始化图标动作，但不立即添加
    QAction *iconAction = new QAction(lineEdit);

    // 连接 textChanged 信号
    QObject::connect(lineEdit, &QLineEdit::textChanged, [=](const QString &text) {
        if (text.isEmpty()) {
            // 如果没有文字，移除图标
            lineEdit->removeAction(iconAction);
        } else {
            // 根据检测结果显示不同的图标
            if (checkInput(text)) {
                iconAction->setIcon(QIcon(successIconPath)); // 成功图标
            } else {
                iconAction->setIcon(QIcon(errorIconPath));   // 错误图标
            }
            // 添加图标到最右侧
            lineEdit->addAction(iconAction, QLineEdit::TrailingPosition);
        }
    });
}

// 检查输入是否符合邮箱格式的函数
bool isValidEmail(const QString &email) {
    // 定义邮箱格式的正则表达式
    QRegularExpression emailRegex(
        R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
        );

    // 使用正则表达式进行匹配
    QRegularExpressionMatch match = emailRegex.match(email);
    return match.hasMatch(); // 如果匹配成功，则返回 true
}

// 检查密码是否符合规则的函数
bool isValidPassword(const QString &password) {
    // 定义密码规则的正则表达式：
    // ^ 表示字符串开始
    // [^\s] 表示不包含空格
    // {8,16} 限定长度为 8-16 个字符
    // [a-zA-Z0-9!@#$%^&*()_+\-=\[\]{};':"\\|,.<>\/?~`] 表示允许的字符范围
    // $ 表示字符串结束
    QRegularExpression passwordRegex(
        R"(^[a-zA-Z0-9!@#$%^&*()_+\-=\[\]{};':"\\|,.<>\/?~`]{8,16}$)"
        );

    // 匹配密码
    QRegularExpressionMatch match = passwordRegex.match(password);

    // 检查是否匹配并且不包含空格
    return match.hasMatch() && !password.contains(' ');
}

bool isValidVerifyCode(const QString &password)
{
    return password.length() == 6;
}


QString gate_url_prefix = "";
