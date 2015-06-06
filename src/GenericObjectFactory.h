#ifndef GENERICOBJECTFACTORY_H
#define GENERICOBJECTFACTORY_H
#include <QMap>

template<class ID,class Base,class ... Args> class GenericObjectFactory{
private:
    typedef Base* (*fInstantiator)(Args ...);
    template<class Derived> static Base* instantiator(Args ... args){
        return new Derived(args ...);
    }
    QMap<ID,fInstantiator> classes;

public:
    GenericObjectFactory(){}
    template<class Derived> void add(ID id){
        if(classes.contains(id))
            ;
        else
            classes.insert(id,&instantiator<Derived>);
    }
    fInstantiator get(ID id){
        return classes.contains(id)?
                    classes[id]:
                    nullptr;
    }
    QList<ID> list(){
        return classes.keys();
    }

};

#endif // GENERICOBJECTFACTORY_H
