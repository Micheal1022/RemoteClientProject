#-------------------------------------------------
#
# Project created by QtCreator 2018-09-03T16:15:02
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets




# 版本信息
VERSION = 1.0.0
## 图标
RC_ICONS = Image/app.ico
## 公司名称
QMAKE_TARGET_COMPANY = "MICHEAL.CHEN"
## 产品名称
QMAKE_TARGET_PRODUCT = "RemoteClient"
## 文件说明
QMAKE_TARGET_DESCRIPTION = "Qt Creator based on Qt 5.10.0"
## 版权信息
QMAKE_TARGET_COPYRIGHT = "Copyright 2018-2020 The Qt Company Ltd. All rights reserved."
## 中文（简体）
RC_LANG = 0x0004

DESTDIR = ./app
MOC_DIR = ./moc_dir
UI_DIR  = ./ui_dir
RCC_DIR = ./rcc_dir
OBJECTS_DIR = ./object_dir

TARGET = RemoteClient
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    MainPage/homepage.cpp \
    MainPage/main.cpp \
    Display/displaycontainer.cpp \
    Display/efinfo.cpp \
    MainPage/guimanager.cpp \
    Record/record.cpp \
    SystemSet/systemset.cpp \
#    SystemInfo/systeminfo.cpp \
    SqlManager/sqlmanager.cpp \
    Display/udpthread.cpp \
    Display/infohintdlg.cpp \
    UserLogin/userlogin.cpp \
    Display/pminfo.cpp \
    MsgBox/infobox.cpp \
    MsgBox/msgbox.cpp \
    MsgBox/questbox.cpp

HEADERS += \
    MainPage/homepage.h \
    Display/displaycontainer.h \
    Display/efinfo.h \
    MainPage/guimanager.h \
    Record/record.h \
    SystemSet/systemset.h \
#    SystemInfo/systeminfo.h \
    SqlManager/sqlmanager.h \
    Display/udpthread.h \
    Display/infohintdlg.h \
    UserLogin/userlogin.h \
    Display/pminfo.h \
    MsgBox/infobox.h \
    MsgBox/msgbox.h \
    MsgBox/questbox.h

FORMS += \
    MainPage/homepage.ui \
    Display/displaycontainer.ui \
    Display/efinfo.ui \
    Record/record.ui \
    SystemSet/systemset.ui \
#    SystemInfo/systeminfo.ui \
    Display/infohintdlg.ui \
    UserLogin/userlogin.ui \
    Display/pminfo.ui \
    MsgBox/infobox.ui \
    MsgBox/questbox.ui

RESOURCES += \
    image.qrc

#RC_FILE = icon.rc
