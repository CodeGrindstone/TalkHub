#ifndef LOADINGDIAGLOG_H
#define LOADINGDIAGLOG_H

#include <QDialog>

namespace Ui {
class LoadingDiaglog;
}

class LoadingDiaglog : public QDialog
{
    Q_OBJECT

public:
    explicit LoadingDiaglog(QWidget *parent = nullptr);
    ~LoadingDiaglog();

private:
    Ui::LoadingDiaglog *ui;
};

#endif // LOADINGDIAGLOG_H
