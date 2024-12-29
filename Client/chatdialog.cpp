#include "chatdialog.h"
#include "ui_chatdialog.h"
#include "loadingdiaglog.h"
#include <QDebug>
#include <QPixmap>
#include "usermgr.h"
#include <QDir>
#include "TcpMgr.h"

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog)
    , mode_(ChatUIMode::ChatMode)
    , state_(ChatUIMode::ChatMode)
    , b_loading_(false)
    , last_widget_(nullptr)
    , cur_chat_uid_(0)
{
    ui->setupUi(this);

    initInterface();

    test_addChatUserList();

    ui->chat_user_list->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->chat_user_list->setSelectionBehavior(QAbstractItemView::SelectItems);


    //设置中心部件为chatpage
    ui->stackedWidget->setCurrentWidget(ui->chat_page);

    initConnect();
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

bool ChatDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        handleGlobalMousePress(mouseEvent);
    }
    return QDialog::eventFilter(watched, event);
}


void ChatDialog::initInterface()
{
    this->setWindowTitle("TalkHub");
    this->setWindowIcon(QIcon(":/Icon/global/talk.png"));

    initSearchBox();
    initSearchList();

    ShowSearch(false);

    // 加载联系人动画
    connect(ui->chat_user_list, &ChatUserList::sig_loading_chat_user,
            this, &ChatDialog::slot_loading_chat_user);

    initSideBar();// 初始化侧边栏

    //检测鼠标点击位置判断是否要清空搜索框
    this->installEventFilter(this); // 安装事件过滤器
    //设置聊天label选中状态
    ui->side_chat_lb->SetSelected(true);
}

void ChatDialog::initSearchBox()
{
    ui->add_btn->SetState("normal","hover","press");

    QAction *searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/Icon/chat/search.png"));
    ui->search_edit->addAction(searchAction,QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));
    // 创建一个清除动作并设置图标
    QAction *clearAction = new QAction(ui->search_edit);
    clearAction->setIcon(QIcon(":/Icon/chat/close_transparent.png"));
    // 初始时不显示清除图标
    // 将清除动作添加到LineEdit的末尾位置
    ui->search_edit->addAction(clearAction, QLineEdit::TrailingPosition);
    // 当需要显示清除图标时，更改为实际的清除图标
    connect(ui->search_edit, &QLineEdit::textChanged, [clearAction](const QString &text) {
        if (!text.isEmpty()) {
            clearAction->setIcon(QIcon(":/Icon/chat/close_search.png"));
        } else {
            clearAction->setIcon(QIcon(":/Icon/chat/close_transparent.png")); // 文本为空时，切换回透明图标
        }
    });
    // 连接清除动作的触发信号到槽函数，用于清除文本
    connect(clearAction, &QAction::triggered, [this, clearAction]() {
        ui->search_edit->clear();
        clearAction->setIcon(QIcon(":/Icon/chat/close_transparent.png")); // 清除文本后，切换回透明图标
        ui->search_edit->clearFocus();
        //清除按钮被按下则不显示搜索框
        ShowSearch(false);
    });
    ui->search_edit->SetMaxLength(15);


    connect(ui->search_edit, &QLineEdit::textChanged, this, &ChatDialog::slot_text_changed);
}

void ChatDialog::initSideBar()
{

    QPixmap pixmap(UserMgr::GetInstance()->getProfile_picture());

    ui->side_head_lb->setPixmap(pixmap.scaled(ui->side_head_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->side_head_lb->setScaledContents(true);

    ui->side_head_lb->setScaledContents(true); // 设置QLabel自动缩放图片内容以适应大小
    ui->side_chat_lb->setProperty("state","normal");
    ui->side_chat_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
    ui->side_contact_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
    AddLBGroup(ui->side_chat_lb);
    AddLBGroup(ui->side_contact_lb);

    connect(ui->side_chat_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);
    connect(ui->side_contact_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);
}

void ChatDialog::initConnect()
{
    //连接加载联系人的信号和槽函数
    connect(ui->con_user_list, &ContactUserList::sig_loading_contact_user,
            this, &ChatDialog::slot_loading_contact_user);


    //连接联系人页面 点击 好友申请 条目的信号
    connect(ui->con_user_list, &ContactUserList::sig_switch_apply_friend_page,
            this,&ChatDialog::slot_switch_apply_friend_page);

    //连接清除搜索框操作
    connect(ui->friend_apply_page, &ApplyFriendPage::sig_show_search,
            this, &ChatDialog::slot_show_search);

    //连接申请添加好友信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_friend_apply,
            this, &ChatDialog::slot_apply_friend);

    // 连接跳转聊天界面
    connect(ui->search_list, &SearchList::sig_jump_chat_item, this, &ChatDialog::slot_jump_chat_item);

    //连接点击联系人item发出的信号和用户信息展示槽函数
    connect(ui->con_user_list, &ContactUserList::sig_switch_friend_info_page,
            this,&ChatDialog::slot_friend_info_page);

    //连接好友信息界面发送的点击事件
    connect(ui->friend_info_page, &FriendInfoPage::sig_jump_chat_item, this,
            &ChatDialog::slot_jump_chat_item_from_infopage);

    //连接聊天列表点击信号
    connect(ui->chat_user_list, &QListWidget::itemClicked,
            this, &ChatDialog::slot_item_clicked);

    //连接对端消息通知
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_text_chat_msg,
            this, &ChatDialog::slot_text_chat_msg);

}

void ChatDialog::initSearchList()
{
    //为searchlist 设置search edit
    ui->search_list->SetSearchEdit(ui->search_edit);
}

void ChatDialog::ShowSearch(bool bsearch)
{
    if(bsearch){
        ui->chat_user_list->hide();
        ui->con_user_list->hide();
        ui->search_list->show();
        mode_ = ChatUIMode::SearchMode;
    }else if(state_ == ChatUIMode::ChatMode){
        ui->chat_user_list->show();
        ui->con_user_list->hide();
        ui->search_list->hide();
        mode_ = ChatUIMode::ChatMode;
        ui->search_list->CloseFindDlg();
        ui->search_edit->clear();
        ui->search_edit->clearFocus();
    }else if(state_ == ChatUIMode::ContactMode){
        ui->chat_user_list->hide();
        ui->search_list->hide();
        ui->con_user_list->show();
        mode_ = ChatUIMode::ContactMode;
        ui->search_list->CloseFindDlg();
        ui->search_edit->clear();
        ui->search_edit->clearFocus();
    }
}


void ChatDialog::test_addChatUserList()
{
    // 创建QListWidgetItem，并设置自定义的widget
    // for(int i = 0; i < 13; i++){
    //     int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    //     int str_i = randomValue%strs.size();
    //     int head_i = randomValue%heads.size();
    //     int name_i = randomValue%names.size();
    //     auto *chat_user_wid = new ChatUserWid();
    //     // TODO
    //     // chat_user_wid->SetInfo(names[name_i], heads[head_i], strs[str_i]);
    //     chat_user_wid->ShowRedPoint(false);
    //     QListWidgetItem *item = new QListWidgetItem;
    //     //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    //     item->setSizeHint(chat_user_wid->sizeHint());
    //     ui->chat_user_list->addItem(item);
    //     ui->chat_user_list->setItemWidget(item, chat_user_wid);
    // }

    // 添加好友到聊天item
    // auto& friend_map = UserMgr::GetInstance()->getFriendMap();
    // for(auto& friend_info : friend_map)
    // {
    //     auto *chat_user_wid = new ChatUserWid();
    //     std::vector<std::shared_ptr<TextChatMsg>> msgs;
    //     if(UserMgr::GetInstance()->getFriendChatMsg(friend_info.first, msgs))
    //     {
    //         for(auto& msg : msg)
    //     }
    //     chat_user_wid->SetInfo(friend_info.second, "hello world!");
    //     QListWidgetItem *item = new QListWidgetItem;
    //     chat_user_wid->ShowRedPoint(false);
    //     //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    //     item->setSizeHint(chat_user_wid->sizeHint());
    //     ui->chat_user_list->addItem(item);
    //     ui->chat_user_list->setItemWidget(item, chat_user_wid);
    // }

}

void ChatDialog::ClearLabelState(StateWidget *lb)
{
    for(auto & ele: lb_list_){
        if(ele == lb){
            continue;
        }
        ele->ClearState();
    }
}

void ChatDialog::AddLBGroup(StateWidget *lb)
{
    lb_list_.push_back(lb);
}

void ChatDialog::handleGlobalMousePress(QMouseEvent *event)
{
    // 实现点击位置的判断和处理逻辑
    // 先判断是否处于搜索模式，如果不处于搜索模式则直接返回
    if( mode_ != ChatUIMode::SearchMode){
        return;
    }
    // 将鼠标点击位置转换为搜索列表坐标系中的位置
    QPoint posInSearchList =(ui->search_list->mapFromGlobal(event->globalPosition())).toPoint();
    // 判断点击位置是否在聊天列表的范围内
    if (!ui->search_list->rect().contains(posInSearchList)) {
        // 如果不在聊天列表内，清空输入框
        ui->search_edit->clear();
        ShowSearch(false);
    }
}

void ChatDialog::SetSelectChatItem(int uid)
{
    if(ui->chat_user_list->count() <= 0){
        return;
    }

    if(uid == 0){
        ui->chat_user_list->setCurrentRow(0);
        QListWidgetItem *firstItem = ui->chat_user_list->item(0);
        if(!firstItem){
            return;
        }

        //转为widget
        QWidget *widget = ui->chat_user_list->itemWidget(firstItem);
        if(!widget){
            return;
        }

        auto con_item = qobject_cast<ChatUserWid*>(widget);
        if(!con_item){
            return;
        }

        cur_chat_uid_ = con_item->GetUserInfo()->uid_;

        return;
    }

    auto find_iter =  chat_items_added_.find(uid);
    if(find_iter == chat_items_added_.end()){
        qDebug() << "uid " <<uid<< " not found, set curent row 0";
        ui->chat_user_list->setCurrentRow(0);
        return;
    }

    ui->chat_user_list->setCurrentItem(find_iter.value());

    cur_chat_uid_ = uid;
}

void ChatDialog::SetSelectChatPage(int uid)
{
    if(ui->chat_user_list->count() <= 0){
        return;
    }

    if (uid == 0) {
        auto item = ui->chat_user_list->item(0);
        //转为widget
        QWidget* widget = ui->chat_user_list->itemWidget(item);
        if (!widget) {
            return;
        }

        auto con_item = qobject_cast<ChatUserWid*>(widget);
        if (!con_item) {
            return;
        }

        //设置信息
        auto user_info = con_item->GetUserInfo();
        ui->chat_page->SetUserInfo(user_info);
        return;
    }

    auto find_iter = chat_items_added_.find(uid);
    if(find_iter == chat_items_added_.end()){
        return;
    }

    //转为widget
    QWidget *widget = ui->chat_user_list->itemWidget(find_iter.value());
    if(!widget){
        return;
    }

    //判断转化为自定义的widget
    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "qobject_cast<ListItemBase*>(widget) is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if(itemType == CHAT_USER_ITEM){
        auto con_item = qobject_cast<ChatUserWid*>(customItem);
        if(!con_item){
            return;
        }

        //设置信息
        auto user_info = con_item->GetUserInfo();
        ui->chat_page->SetUserInfo(user_info);

        return;
    }
}

void ChatDialog::UpdateChatMsg(std::shared_ptr<TextChatMsg> msg)
{
    if(msg->uid_ != cur_chat_uid_)
        return;

    ui->chat_page->AppendChatMsg(msg);
}


void ChatDialog::slot_loading_chat_user()
{
    if(b_loading_){
        return;
    }
    b_loading_ = true;
    LoadingDiaglog *loadingDialog = new LoadingDiaglog(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    qDebug() << "add new data to list.....";

    // TODO...
    //addChatUserList();

    // 加载完成后关闭对话框
    loadingDialog->deleteLater();
    b_loading_ = false;
}

void ChatDialog::slot_side_chat()
{
    ClearLabelState(ui->side_chat_lb);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    state_ = ChatUIMode::ChatMode;
    ShowSearch(false);
}

void ChatDialog::slot_side_contact()
{
    ClearLabelState(ui->side_contact_lb);
    //设置

    if(last_widget_ == nullptr){
        qDebug() << "1";
        ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
        last_widget_ = ui->friend_apply_page;
    }else{
        ui->stackedWidget->setCurrentWidget(last_widget_);
    }

    state_ = ChatUIMode::ContactMode;
    ShowSearch(false);
}

void ChatDialog::slot_text_changed(const QString &str)
{
    //qDebug()<< "receive slot text changed str is " << str;
    if (!str.isEmpty()) {
        ShowSearch(true);
    }
}

void ChatDialog::slot_apply_friend(std::shared_ptr<AddFriendApply> apply)
{
    qDebug() << "receive apply friend slot, applyuid is " << apply->user_->uid_ << " name is "
             << apply->user_->name_ << " desc is " << apply->request_message_;

    // 检测是否接受过此通知
    int uid = apply->user_->uid_;
    bool b_already = UserMgr::GetInstance()->AlreadyApply(uid);
    if(b_already){
        return;
    }

    UserMgr::GetInstance()->AddFriendApplyInfo(uid, apply);

    ui->side_contact_lb->ShowRedPoint(true);
    ui->con_user_list->ShowRedPoint(true);
    ui->friend_apply_page->AddNewApply(apply);
}

void ChatDialog::slot_loading_contact_user()
{
    qDebug() << "slot loading contact user";
    if(b_loading_){
        return;
    }

    b_loading_ = true;
    LoadingDiaglog *loadingDialog = new LoadingDiaglog(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    qDebug() << "add new data to list.....";
    // TODO... 加载更多联系人loadMoreConUser();
    // 加载完成后关闭对话框
    loadingDialog->deleteLater();

    b_loading_ = false;
}

void ChatDialog::slot_switch_apply_friend_page()
{
    last_widget_ = ui->friend_apply_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
}

void ChatDialog::slot_show_search(bool show)
{
    ShowSearch(show);
}

void ChatDialog::slot_jump_chat_item(std::shared_ptr<UserInfo> user_info)
{
    qDebug() << "slot jump chat item ";
    auto find_iter = chat_items_added_.find(user_info->uid_);
    if(find_iter != chat_items_added_.end()){
        qDebug() << "jump to chat item , uid is " << user_info->uid_;
        ui->chat_user_list->scrollToItem(find_iter.value());
        ui->side_chat_lb->SetSelected(true);
        SetSelectChatItem(user_info->uid_);
        // 更新聊天界面信息
        SetSelectChatPage(user_info->uid_);
        slot_side_chat();
        return;
    }

    //如果没找到，则创建新的插入listwidget

    auto* chat_user_wid = new ChatUserWid();
    // TODO..最近聊天信息
    chat_user_wid->SetInfo(user_info, "");
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);

    chat_items_added_.insert(user_info->uid_, item);

    ui->side_chat_lb->SetSelected(true);
    SetSelectChatItem(user_info->uid_);
    //更新聊天界面信息
    SetSelectChatPage(user_info->uid_);
    slot_side_chat();
}

void ChatDialog::slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> user_info)
{
    qDebug() << "slot jump chat item ";
    auto find_iter = chat_items_added_.find(user_info->uid_);
    if(find_iter != chat_items_added_.end()){
        qDebug() << "jump to chat item , uid is " << user_info->uid_;
        ui->chat_user_list->scrollToItem(find_iter.value());
        ui->side_chat_lb->SetSelected(true);
        SetSelectChatItem(user_info->uid_);
        //更新聊天界面信息
        SetSelectChatPage(user_info->uid_);
        slot_side_chat();
        return;
    }

    //如果没找到，则创建新的插入listwidget

    auto* chat_user_wid = new ChatUserWid();
    chat_user_wid->SetInfo(user_info, "");
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);

    chat_items_added_.insert(user_info->uid_, item);

    ui->side_chat_lb->SetSelected(true);
    SetSelectChatItem(user_info->uid_);
    //更新聊天界面信息
    SetSelectChatPage(user_info->uid_);
    slot_side_chat();
}

void ChatDialog::slot_friend_info_page(std::shared_ptr<UserInfo> user_info)
{
    qDebug()<<"receive switch friend info page sig";
    last_widget_ = ui->friend_info_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_info_page);
    ui->friend_info_page->SetInfo(user_info);
}

void ChatDialog::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = ui->chat_user_list->itemWidget(item); // 获取自定义widget对象
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM
        || itemType == ListItemType::GROUP_TIP_ITEM){
        qDebug()<< "slot invalid item clicked ";
        return;
    }


    if(itemType == ListItemType::CHAT_USER_ITEM){
        // 创建对话框，提示用户
        qDebug()<< "contact user item clicked ";

        auto chat_wid = qobject_cast<ChatUserWid*>(customItem);
        auto user_info = chat_wid->GetUserInfo();
        //跳转到聊天界面
        ui->chat_page->SetUserInfo(user_info);
        cur_chat_uid_ = user_info->uid_;
        return;
    }
}

void ChatDialog::slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg)
{
    auto find_iter = chat_items_added_.find(msg->uid_);
    if(find_iter != chat_items_added_.end()){
        qDebug() << "set chat item msg, uid is " << msg->uid_;
        QWidget *widget = ui->chat_user_list->itemWidget(find_iter.value());
        auto chat_wid = qobject_cast<ChatUserWid*>(widget);
        if(!chat_wid){
            return;
        }
        chat_wid->updateLastMsg(msg);
        //更新当前聊天页面记录
        UpdateChatMsg(msg);
        UserMgr::GetInstance()->AppendFriendChatMsg(msg->uid_, msg);
        return;
    }


    //如果没找到，则创建新的插入listwidget
    auto* chat_user_wid = new ChatUserWid();
    //查询好友信息
    auto friend_info = UserMgr::GetInstance()->getUserInfo(msg->uid_);
    if(nullptr == friend_info)
    {
        return;
    }

    chat_user_wid->SetInfo(friend_info, msg->content_);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    chat_user_wid->updateLastMsg(msg);
    UserMgr::GetInstance()->AppendFriendChatMsg(msg->uid_,msg);
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    chat_items_added_.insert(msg->uid_, item);
}


