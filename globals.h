#ifndef GLOBALS_H
#define GLOBALS_H
#include <QDebug>
#include <iostream>
#include <cassert>

#define MAXPATHLEN 256
#define DEFAULT_BUFFER_SIZE 1048576

#include "exceptions.h"

QString loadModule(QString path);

#endif // GLOBALS_H
