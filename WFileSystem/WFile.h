#ifndef WFILE_H
#define WFILE_H

#include "WFileSystem.h"

class WFile : public QFile
{
    Q_OBJECT
private:
    qint64 fileSize;
    QDateTime fileCreationDateTime;
    QDateTime fileLastAccessDateTime;

    virtual qint64 readData(char *data,qint64 length);
    qint64 readCachedData(char *data,qint64 length);
    virtual qint64 pos() const;

    typedef qint64 (*readCallbackFn)(char *data,qint64 length,const WFile*);
    typedef bool (*seekCallbackFn)(qint64 offset,const WFile*);
    typedef qint64 (*posCallbackFn)(const WFile*);

    readCallbackFn readFn;
    seekCallbackFn seekFn;
    posCallbackFn posFn;

    OpenMode mode;

    char *cache;

public:
    explicit WFile(QString fileName="",QObject *parent=0);
    WFileSystemNode *fsNode;

    bool open(OpenMode mode);
    OpenMode openMode() const;

    void setSize(qint64);
    virtual qint64 size() const;

    void setCreationDateTime(QDateTime dateTime);
    QDateTime creationDateTime();

    void setLastAccessDateTime(QDateTime dateTime);
    QDateTime lastAccessDateTime();

    void setReadFn(readCallbackFn fn);
    qint64 read(char *data, qint64 maxlen);
    virtual bool seek(qint64 offset);
    void setSeekFn(seekCallbackFn fn);
    void setPosFn(posCallbackFn fn);

    void setFSNode(WFileSystemNode *node);

    void forceCache();
    bool cached=false;

    ~WFile();

signals:

public slots:

};

#endif // WFILE_H
