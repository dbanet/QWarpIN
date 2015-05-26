#ifndef WAIFILEREADER_H
#define WAIFILEREADER_H
#include "WarpINArchiveInterface.h"

/*!
 * This class implements reading compressed files from WPI with support of lazy
 * initialization, arbitrary cursor position manipulations and zlib state
 * saving between calls.
 */
class WAIFileReader{
public:
    WAIFileReader(WFileSystemNode*,qint64 bufferSize=DEFAULT_BUFFER_SIZE);

    /* actual routines working with compressed files */
    qint64 read(char*,qint64);
    qint64 pos();
    bool seek(qint64 offset);
    WFileSystemNode *fsNode;

    /* convenience routines to support lazy initialization */
    static QList<WAIFileReader*> readersList; // instances container
    static qint64 read(char*,qint64,const WFile*); // gets instance by WFile* and reads using it
    static qint64 pos(const WFile*); // gets instance by WFile* and tells the current cursor pos
    static bool seek(qint64 offset,const WFile*); // gets instance by WFile* and seeks the cursor
    static WAIFileReader* getReaderByFSNode(WFileSystemNode*); // provides/creates instances

    ~WAIFileReader();

private:
    char *inputBuffer;
    char *outputBuffer;
    qint64 bufferSize;
    qint64 bytesBuffered;
    bz_stream *z;
    QFile *arcFile;
    qint64 arcCur;
    qint64 compCur;
    qint64 decompCur;
    qint64 bufCur; // the piece of (decompressed) file mapped onto outputBuffer
};

#endif // WAIFILEREADER_H
