#ifndef INDEX_H
#define INDEX_H
#include "wabstractarchiveinterface.h"
#include "../genericobjectfactory.h"

typedef GenericObjectFactory<QString,WAbstractArchiveInterface,QFile*> WArchiveInterfaceFactory;
WArchiveInterfaceFactory* getArchiveInterfaces();
#endif // INDEX_H
