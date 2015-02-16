#ifndef WFILESYSTEM_H
#define WFILESYSTEM_H
#include <QObject>
#include <QPointer>
#include <QList>
#include <QFile>
#include <QDir>
#include <QVariant>
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
