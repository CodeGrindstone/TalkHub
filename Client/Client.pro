QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chatfrienditem.cpp \
    chatwidget.cpp \
    clickablelabel.cpp \
    const.cpp \
    elidedlabel.cpp \
    friendlistwidget.cpp \
    getverifybutton.cpp \
    httpmgr.cpp \
    loginfaileddialog.cpp \
    loginwidget.cpp \
    main.cpp \
    registerwidget.cpp \
    resetwidget.cpp \
    sideframe.cpp \
    sideframeitem.cpp \
    widget.cpp

HEADERS += \
    Singleton.h \
    chatfrienditem.h \
    chatwidget.h \
    clickablelabel.h \
    const.h \
    elidedlabel.h \
    friendlistwidget.h \
    getverifybutton.h \
    httpmgr.h \
    loginfaileddialog.h \
    loginwidget.h \
    registerwidget.h \
    resetwidget.h \
    sideframe.h \
    sideframeitem.h \
    widget.h

FORMS += \
    chatwidget.ui \
    loginfaileddialog.ui \
    loginwidget.ui \
    registerwidget.ui \
    resetwidget.ui \
    sideframe.ui \
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
