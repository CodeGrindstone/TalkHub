#include "chatwidget.h"
#include "ui_chatwidget.h"
#include <QAction>
#include <QEvent>
#include <QCursor>
#include <QFont>
#include <windows.h>

struct FriendData {
    QString avatarPath;    // 头像路径
    QString friendName;    // 好友名字
    QString latestMessage; // 最新消息
    QString messageTime;   // 消息时间
};

// 测试数据
QVector<FriendData> generateTestData() {
    QVector<FriendData> testData = {
        {":/Icon/test/test1.png", "Alice", "Hey, how are you?", "10:30 AM"},
        {":/Icon/test/test2.png", "Bob", "Can we talk later?", "9:45 AM"},
        {":/Icon/test/test3.png", "Charlie", "Let's catch up tomorrow!", "Yesterday"},
        {":/Icon/test/test4.png", "Diana", "Thanks for the help!", "Monday"},
        {":/Icon/test/test5.png", "Eve", "Happy Birthday! 🎉", "2 days ago"},
        {":/Icon/test/test6.png", "Alice", "Hey, how are you?", "10:30 AM"},
        {":/Icon/test/test7.png", "Bob", "Can we talk later?", "9:45 AM"},
        {":/Icon/test/test8.png", "Charlie", "Let's catch up tomorrow!", "Yesterday"},
        {":/Icon/test/test9.png", "Diana", "Thanks for the help!", "Monday"},
        {":/Icon/test/test10.png", "Eve", "Happy Birthday! 🎉", "2 days ago"},
    };
    return testData;
}



ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatWidget)
{
    ui->setupUi(this);

    initInterface();
}

ChatWidget::~ChatWidget()
{
    delete ui;
}

bool ChatWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->createGroupBtn)
    {
        if(event->type() == QEvent::Enter)
        {
            ui->friendListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else
        {
            ui->friendListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }
    if(watched == ui->friendListWidget)
    {
        if(event->type() == QEvent::Enter)
        {
            ui->createGroupBtn->setCursor(Qt::PointingHandCursor);
        }
        else
        {
            unsetCursor();
        }
    }
    return QWidget::eventFilter(watched, event);
}

void ChatWidget::initInterface()
{
    this->setWindowTitle("TalkHub");
    QIcon window(":/Icon/global/talk.png");
    this->setWindowIcon(window);

    // 在搜索框开头添加放大镜
    QAction *iconAction = new QAction(this);
    iconAction->setIcon(QIcon(":/Icon/chat/search.png"));
    ui->searchLineEdit->addAction(iconAction, QLineEdit::LeadingPosition);

    // 添加按钮
    ui->createGroupBtn->installEventFilter(this);


    // 添加好友消息
    ui->friendListWidget->addFriend(":/Icon/chat/system_message.png", "系统消息", "测试", "16:55");

    testAddfriend();

    initTool();

    ui->inputEdit->setStyleSheet("border: none;");

    QFont fixedFont("Microsoft YaHei", 12); // 使用固定宽度字体
    ui->inputEdit->setFont(fixedFont);

}

void ChatWidget::testAddfriend()
{
    QVector<FriendData> data = generateTestData();
    for (const FriendData &entry : data) {
        ui->friendListWidget->addFriend(entry.avatarPath, entry.friendName, entry.latestMessage, entry.messageTime);
    }
}

void ChatWidget::initTool()
{
    // 创建头像控件
    QLabel *emojoLabel = new QLabel(this);
    QPixmap emojoPixmap(":/Icon/chat/emoji.png");
    emojoLabel->setPixmap(emojoPixmap.scaled(25, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    emojoLabel->setFixedSize(25, 25);
    emojoLabel->setStyleSheet("border: none;");
    ui->toolLayout->addWidget(emojoLabel);
}
