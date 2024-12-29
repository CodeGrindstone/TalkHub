QT       += core gui network websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    adduseritem.cpp \
    applyfriend.cpp \
    applyfrienditem.cpp \
    applyfriendlist.cpp \
    applyfriendpage.cpp \
    bubbleframe.cpp \
    chatdialog.cpp \
    chatitembase.cpp \
    chatpage.cpp \
    chatuserlist.cpp \
    chatuserwid.cpp \
    chatview.cpp \
    clickablelabel.cpp \
    clickedbtn.cpp \
    clickedoncelabel.cpp \
    const.cpp \
    contactuserlist.cpp \
    conuseritem.cpp \
    findfaildlg.cpp \
    findsuccessdlg.cpp \
    friendinfopage.cpp \
    friendlabel.cpp \
    getverifybutton.cpp \
    grouptipitem.cpp \
    httpmgr.cpp \
    listitembase.cpp \
    loadingdiaglog.cpp \
    loginfaileddialog.cpp \
    loginwidget.cpp \
    main.cpp \
    messagetextedit.cpp \
    picturebubble.cpp \
    registerwidget.cpp \
    resetwidget.cpp \
    searchboxedit.cpp \
    searchlist.cpp \
    statewidget.cpp \
    tcpmgr.cpp \
    textbubble.cpp \
    userdata.cpp \
    usermgr.cpp \
    widget.cpp

HEADERS += \
    Singleton.h \
    adduseritem.h \
    applyfriend.h \
    applyfrienditem.h \
    applyfriendlist.h \
    applyfriendpage.h \
    bubbleframe.h \
    chatdialog.h \
    chatitembase.h \
    chatpage.h \
    chatuserlist.h \
    chatuserwid.h \
    chatview.h \
    clickablelabel.h \
    clickedbtn.h \
    clickedoncelabel.h \
    const.h \
    contactuserlist.h \
    conuseritem.h \
    findfaildlg.h \
    findsuccessdlg.h \
    friendinfopage.h \
    friendlabel.h \
    getverifybutton.h \
    grouptipitem.h \
    httpmgr.h \
    listitembase.h \
    loadingdiaglog.h \
    loginfaileddialog.h \
    loginwidget.h \
    messagetextedit.h \
    picturebubble.h \
    registerwidget.h \
    resetwidget.h \
    searchboxedit.h \
    searchlist.h \
    statewidget.h \
    tcpmgr.h \
    textbubble.h \
    userdata.h \
    usermgr.h \
    widget.h

FORMS += \
    adduseritem.ui \
    applyfriend.ui \
    applyfrienditem.ui \
    applyfriendpage.ui \
    chatdialog.ui \
    chatpage.ui \
    chatuserwid.ui \
    conuseritem.ui \
    findfaildlg.ui \
    findsuccessdlg.ui \
    friendinfopage.ui \
    friendlabel.ui \
    grouptipitem.ui \
    loadingdiaglog.ui \
    loginfaileddialog.ui \
    loginwidget.ui \
    registerwidget.ui \
    resetwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

# 可执行文件输出路径
TARGET = TalkHub
DESTDIR = ../../

DISTFILES += \
    Icon/login/line.png
