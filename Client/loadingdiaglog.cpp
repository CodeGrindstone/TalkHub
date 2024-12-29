#include "loadingdiaglog.h"
#include "ui_loadingdiaglog.h"
#include <QMovie>

LoadingDiaglog::LoadingDiaglog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoadingDiaglog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground); // 设置背景透明
    // 获取屏幕尺寸
    setFixedSize(parent->size()); // 设置对话框为全屏尺寸

    QMovie *movie = new QMovie(":/Icon/chat/loading.gif"); // 加载动画的资源文件
    ui->loading_lb->setMovie(movie);
    movie->start();
}

LoadingDiaglog::~LoadingDiaglog()
{
    delete ui;
}
