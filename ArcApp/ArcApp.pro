#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T23:55:49
#
#-------------------------------------------------
QT       += sql
QT       += core gui
QT       += multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ArcApp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    databasemanager.cpp \
    loginprompt.cpp \
    bookingmanager.cpp \
    booking.cpp \
    takephoto.cpp \
    transaction.cpp \
    payment.cpp \
    client.cpp \
    mymodel.cpp

HEADERS  += mainwindow.h \
    databasemanager.h \
    shared.h \
    dbconfig.h \
    loginprompt.h \
    bookingmanager.h \
    booking.h \
    takephoto.h \
    transaction.h \
    payment.h \
    client.h \
    mymodel.h

FORMS    += mainwindow.ui \
    loginprompt.ui \
    takephoto.ui \
    payment.ui

RESOURCES += arcstyle/styling.qrc

RC_FILE = ArcApp.rc

DISTFILES +=
