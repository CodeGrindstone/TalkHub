#include "widget.h"
#include "const.h"
#include <QFile>
#include <QApplication>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QSettings>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //解析config文件
    QString exeDirPath = QCoreApplication::applicationDirPath();
    QString iniFilePath = QDir(exeDirPath).filePath("config.ini");
    QSettings settings(iniFilePath, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/Ip", "").toString();
    QString gate_port = settings.value("GateServer/Port", "").toString();
    gate_url_prefix = "http://"+gate_host+":"+gate_port;

    Widget w;
    QFile qss(":/stylesheet.qss");
    if( qss.open(QFile::ReadOnly))
    {
        qDebug("open success");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }else{
        qDebug("Open failed");
    }
    return a.exec();
}
