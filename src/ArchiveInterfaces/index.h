#ifndef INDEX_H
#define INDEX_H
#include "WAbstractArchiveInterface.h"
#include "GenericObjectFactory.h"

typedef GenericObjectFactory<QString,WAbstractArchiveInterface,QFile*> WArchiveInterfaceFactory;
WArchiveInterfaceFactory* getArchiveInterfaces();
#endif // INDEX_H
