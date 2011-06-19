#-------------------------------------------------
#
# Project created by QtCreator 2011-05-14T00:41:02
#
#-------------------------------------------------

QT       += core gui
QT       += webkit
QT       += xml
QT       += network
QT       += multimedia

TARGET = Symphony
TEMPLATE = app

SOURCES += main.cpp\
        browser.cpp \
    highlighter.cpp \
    browserview.cpp \
    tabs.cpp \
    history.cpp \
    completer.cpp \
    completionbox.cpp \
    listwidget.cpp \
    bookmarks.cpp \
    downloadmanager.cpp \
    preferences.cpp \
    networkaccessmanager.cpp \
    adblocksubscription.cpp \
    cookiejar.cpp \
    cookies.cpp \
    emptynetworkreply.cpp \
    loader.cpp \
    noscript.cpp \
    adblockpage.cpp \
    tabbar.cpp \
    tab.cpp \
    tabpreviewer.cpp \
    tabbutton.cpp \
    tabmenu.cpp \
    bookmarksitem.cpp \
    networkcookieitem.cpp \
    historyitem.cpp

HEADERS  += browser.h \
    highlighter.h \
    tabs.h \
    history.h \
    browserview.h \
    completer.h \
    completionbox.h \
    listwidget.h \
    bookmarks.h \
    downloadmanager.h \
    preferences.h \
    networkaccessmanager.h \
    adblocksubscription.h \
    adblockpage.h \
    cookiejar.h \
    cookies.h \
    emptynetworkreply.h \
    loader.h \
    noscript.h \
    tabbar.h \
    tab.h \
    tabpreviewer.h \
    tabbutton.h \
    tabmenu.h \
    bookmarksitem.h \
    networkcookieitem.h \
    historyitem.h

FORMS    += browser.ui \
    finddialog.ui \
    preferences.ui

OTHER_FILES +=

RESOURCES += \
    resource.qrc

INCLUDEPATH += C:/Users/Untamed/Desktop/boost_1_46_1
