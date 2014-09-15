#-------------------------------------------------
#
# Project created by QtCreator 2014-08-09T17:55:57
#
#-------------------------------------------------

QT       += core gui
QT       += opengl
QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = prog
TEMPLATE = app


SOURCES += main.cpp \
    game.cpp \
    endlivel.cpp \
    selectlivel.cpp \
    helpwnd.cpp \
    about.cpp

HEADERS  += \
    game.h \
    endlivel.h \
    selectlivel.h \
    helpwnd.h \
    about.h

RESOURCES += \
    res/res.qrc

FORMS += \
    endlivel.ui \
    selectlivel.ui \
    helpwnd.ui \
    about.ui
