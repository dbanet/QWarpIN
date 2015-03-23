#ifndef WFILESYSTEM_H
#define WFILESYSTEM_H
#include <QObject>
#include <QDebug>
#include <QPointer>
#include <QList>
#include <QFile>
#include <QDir>
#include <QVariant>
#include <QDateTime>
class WFileSystemTree;
class WFileSystemNode;
class WFile;
namespace WFileSystem{
    typedef enum{
        directory,file
    } types;
}
#include "wfilesystemtree.h"
#include "wfilesystemnode.h"
#include "wfile.h"
#endif // WFILESYSTEM_H
