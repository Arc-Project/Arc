#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T23:55:49
#
#-------------------------------------------------
QT       += sql
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ArcApp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    databasemanager.cpp \
    loginprompt.cpp \
    bookingmanager.cpp \
    booking.cpp

HEADERS  += mainwindow.h \
    databasemanager.h \
    shared.h \
    dbconfig.h \
    loginprompt.h \
    bookingmanager.h \
    booking.h

FORMS    += mainwindow.ui \
    loginprompt.ui

RESOURCES += qdarkstyle/styling.qrc

DISTFILES +=
