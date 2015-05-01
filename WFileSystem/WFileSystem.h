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
#include "WFileSystemTree.h"
#include "WFileSystemNode.h"
#include "WFile.h"
#endif // WFILESYSTEM_H
