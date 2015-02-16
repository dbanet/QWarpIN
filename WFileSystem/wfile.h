#ifndef WFILE_H
#define WFILE_H

#include "wfilesystem.h"

class WFile : public QFile
{
    Q_OBJECT
private:
    qint64 fileSize;
    virtual qint64 readData(char *data,qint64 length);
    typedef qint64 (*readDataCallbackFn)(char *data,qint64 length,const WFile*);
    readDataCallbackFn readDataFn;

public:
    explicit WFile(QString fileName,QObject *parent=0);
    WFileSystemNode *fsNode;
    void setSize(qint64);
    void setReadDataFn(readDataCallbackFn fn);
    virtual qint64 size() const;

signals:

public slots:

};

#endif // WFILE_H
