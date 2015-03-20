#-------------------------------------------------
#
# Project created by QtCreator 2014-06-30T14:21:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QWarpIN
TEMPLATE = app
LIBS += -llibbz2

QMAKE_CXXFLAGS += -std=c++11 -O0

SOURCES += main.cpp\
        wpi.cpp \
    globals.cpp \
    ArchiveInterfaces/wabstractarchiveinterface.cpp \
    ArchiveInterfaces/index.cpp \
    ArchiveInterfaces/warpinarchiveinterface.cpp \
    exceptions.cpp \
    warchive.cpp \
    WFileSystem/wfilesystemtree.cpp \
    WFileSystem/wfilesystemnode.cpp \
    WFileSystem/wfile.cpp

HEADERS  += wpi.h \
    globals.h \
    ArchiveInterfaces/wabstractarchiveinterface.h \
    ArchiveInterfaces/index.h \
    ArchiveInterfaces/warpinarchiveinterface.h \
    genericobjectfactory.h \
    exceptions.h \
    warchive.h \
    WFileSystem/wfilesystemtree.h \
    WFileSystem/wfilesystem.h \
    WFileSystem/wfilesystemnode.h \
    WFileSystem/wfile.h

FORMS    += wpi.ui
