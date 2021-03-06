#-------------------------------------------------
#
# Project created by QtCreator 2017-01-09T19:06:07
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = QWeave
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9

SOURCES += main.cpp\
        mainwindow.cpp \
    weave.cpp \
    configdialog.cpp \
    genpatterndlg.cpp \
    modifydlg.cpp \
    duplicatpatterndlg.cpp

HEADERS  += mainwindow.h \
    weave.h \
    configdialog.h \
    genpatterndlg.h \
    modifydlg.h \
    duplicatpatterndlg.h

RESOURCES += \
    images.qrc
