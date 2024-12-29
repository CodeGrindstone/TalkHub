#ifndef FINDSUCCESSDLG_H
#define FINDSUCCESSDLG_H

#include <QDialog>
#include <memory>
#include "userdata.h"
#include "applyfriend.h"

namespace Ui {
class FindSuccessDlg;
}

class FindSuccessDlg : public QDialog
{
    Q_OBJECT
public:
    explicit FindSuccessDlg(QWidget *parent = nullptr);
    ~FindSuccessDlg();
    void SetSearchInfo(std::shared_ptr<UserInfo> si);
private slots:
    void on_add_friend_btn_clicked();
private:
    Ui::FindSuccessDlg *ui;
    QWidget * parent_;
    std::shared_ptr<UserInfo> user_info_;
};

#endif // FINDSUCCESSDLG_H
