#include "index.h"
#include "WarpIN/WarpINArchiveInterface.h"

/* to add your own custom interface to archive files, include its declaration ↑ and register it with the factory ↓ */

WArchiveInterfaceFactory* getArchiveInterfaces(){
    auto interfacesFactory=new WArchiveInterfaceFactory;

    interfacesFactory->add<WarpinArchiveInterface>("WarpinArchiveInterface");

    return interfacesFactory;
}
