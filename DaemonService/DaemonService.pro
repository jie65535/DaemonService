#-------------------------------------------------
#
# Project created by QtCreator 2018-12-27T16:26:24
#
#-------------------------------------------------

QT       += core gui network sql
RESOURCES += res.qrc
RC_ICONS = Daemon.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DaemonService
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

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    daemonservice.cpp \
    worker.cpp \
    md5.cpp \
    dal.cpp \
    log.cpp \
    ipsechelper.cpp

HEADERS += \
        mainwindow.h \
    daemonservice.h \
    worker.h \
    md5.h \
    dal.h \
    model.h \
    log.h \
    ipsechelper.h

FORMS += \
        mainwindow.ui
		
RC_FILE = uac.rc
		
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
