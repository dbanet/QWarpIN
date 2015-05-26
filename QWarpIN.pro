#-------------------------------------------------
#
# Project created by QtCreator 2014-06-30T14:21:32
#
#-------------------------------------------------

QT       += core gui script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QWarpIN
TEMPLATE = app
LIBS += -llibbz2

QMAKE_CXXFLAGS += -std=c++11 -O0

SOURCES += main.cpp\
        WPI.cpp \
    globals.cpp \
    ArchiveInterfaces/WAbstractArchiveInterface.cpp \
    ArchiveInterfaces/index.cpp \
    ArchiveInterfaces/WarpIN/WarpINArchiveInterface.cpp \
    exceptions.cpp \
    WArchive.cpp \
    WFileSystem/WFileSystemTree.cpp \
    WFileSystem/WFileSystemNode.cpp \
    WFileSystem/WFile.cpp \
    WScriptHost/WScriptHost.cpp

HEADERS  += WPI.h \
    globals.h \
    ArchiveInterfaces/WAbstractArchiveInterface.h \
    ArchiveInterfaces/index.h \
    ArchiveInterfaces/WarpIN/WarpINArchiveInterface.h \
    GenericObjectFactory.h \
    exceptions.h \
    WArchive.h \
    WFileSystem/WFileSystemTree.h \
    WFileSystem/WFileSystem.h \
    WFileSystem/WFileSystemNode.h \
    WFileSystem/WFile.h \
    WScriptHost/WScriptHost.h

FORMS    += WPI.ui

RESOURCES += \
    WScriptHost/script.qrc

OTHER_FILES += \
    WScriptHost/js/Host.js
