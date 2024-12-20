QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    clickablelabel.cpp \
    const.cpp \
    getverifybutton.cpp \
    httpmgr.cpp \
    loginfaileddialog.cpp \
    loginwidget.cpp \
    main.cpp \
    registerwidget.cpp \
    resetwidget.cpp \
    widget.cpp

HEADERS += \
    Singleton.h \
    clickablelabel.h \
    const.h \
    getverifybutton.h \
    httpmgr.h \
    loginfaileddialog.h \
    loginwidget.h \
    registerwidget.h \
    resetwidget.h \
    widget.h

FORMS += \
    loginfaileddialog.ui \
    loginwidget.ui \
    registerwidget.ui \
    resetwidget.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

# 可执行文件输出路径
DESTDIR = ../../

DISTFILES += \
    Icon/login/line.png
