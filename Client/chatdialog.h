#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include <QAction>
#include <QListWidgetItem>
#include <QRandomGenerator>
#include <QVector>
#include <QString>
#include <QList>
#include <QMap>

#include "chatuserwid.h"
#include "const.h"
#include "loadingdiaglog.h"
#include "statewidget.h"
#include "usermgr.h"

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
protected:
    bool eventFilter(QObject *, QEvent *);
private:
    void initInterface();
    void initSearchBox();
    void initSideBar();
    void initConnect();
    void initSearchList();
    void ShowSearch(bool bsearch);
    void test_addChatUserList();
    void ClearLabelState(StateWidget *lb);
    void AddLBGroup(StateWidget *lb);
    void handleGlobalMousePress(QMouseEvent *event);
    void SetSelectChatItem(int uid = 0);
    void SetSelectChatPage(int uid = 0);
    void UpdateChatMsg(std::shared_ptr<TextChatMsg> msgdata);
private:
    Ui::ChatDialog *ui;
    ChatUIMode mode_;
    ChatUIMode state_;
    bool b_loading_;
    QList<StateWidget*> lb_list_;
    QWidget* last_widget_;
    QMap<int, QListWidgetItem*> chat_items_added_;
    int cur_chat_uid_;

private slots:
    void slot_loading_chat_user();
    void slot_side_chat();
    void slot_side_contact();
    void slot_text_changed(const QString &str);

    void slot_apply_friend(std::shared_ptr<AddFriendApply>);
    void slot_loading_contact_user();
    void slot_switch_apply_friend_page();
    void slot_show_search(bool show);


    void slot_jump_chat_item(std::shared_ptr<UserInfo> user_info);
    void slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> user_info);
    void slot_friend_info_page(std::shared_ptr<UserInfo> user_info);
    void slot_item_clicked(QListWidgetItem *item);
    void slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg);
};


#endif // CHATDIALOG_H
