#ifndef WSCRIPTHOST_H
#define WSCRIPTHOST_H

#include "globals.h"
#include <QObject>
#include <QtScript/QtScript>

class WScriptHost : public QObject
{
    Q_OBJECT
public:
    explicit WScriptHost(QObject *parent = 0);

signals:

public slots:

private:
    QScriptEngine engine;
    static QScriptValue log(QScriptContext *context,QScriptEngine *engine);
};

#endif // WSCRIPTHOST_H
