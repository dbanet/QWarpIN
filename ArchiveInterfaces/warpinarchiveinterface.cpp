#include "warpinarchiveinterface.h"
#include "globals.h"

WarpinArchiveInterface::WarpinArchiveInterface(QFile *archive) :
    archive(archive){
    this->archive->open(QIODevice::ReadOnly);
    try{
        this->readArcHeaders();
    } catch(exception *e){
        qDebug(e->what());
        return;
    }
    qDebug()<<"Minimum WarpIN version:"<<this->ArcHeader.wi_revision_needed;
    qDebug()<<"Number of packages in the archive:"<<this->ArcHeader.sPackages;
    qDebug()<<"Size of script:"<<this->ArcHeader.usScriptOrig<<"="<<this->script.size();
    qDebug()<<"Number of packages: "<<this->packHeadersList.length();
    for(int i=0;i<this->packHeadersList.length();++i)
        qDebug(QString(
            QString()
                + "Package number "  + QString::number(i)                                  + ": "
                + "name -> \""       + this->packHeadersList[i]->name                      + "\"; "
                + "files -> \""      + QString::number(this->packHeadersList[i]->files)    + "\"; "
                + "number -> \""     + QString::number(this->packHeadersList[i]->number)   + "\"; "
                + "size orig. -> \"" + QString::number(this->packHeadersList[i]->origsize) + "\"; "
                + "size comp. -> \"" + QString::number(this->packHeadersList[i]->compsize) + "\"; "
        ).toStdString().c_str());
    qDebug(QString("Archive filesystem: "+this->files()->toJSON()).toStdString().c_str());
}

QString WarpinArchiveInterface::id() const{
    return "WarpIN";
}

QString WarpinArchiveInterface::arcName() const{
    return this->archive->fileName();
}

QFile* WarpinArchiveInterface::arcFile() const{
    return this->archive;
}

void WarpinArchiveInterface::readArcHeaders(){
    qint64 ext4HeaderOffset,scriptOffset,extendedDataOffset,packHeadersOffset,archiveOffset;
    /* trying to read a header in the tail (lolwut btw?) */
    /* only for archives with an executable stub in them */
    if(!~(ext4HeaderOffset=this->readTailHeader())) // the tail is not a valid header, searching front (plain .wpi)
        if(!~(ext4HeaderOffset=this->readFrontHeader())) // the archive seems to be invalid
            throw new E_WPIAI_CannotReadArchiveHeader;

    /* checking which WarpIN version the archive requests */
    if(this->ArcHeader.wi_revision_needed<3)
        throw new E_WPIAI_OutdatedArchive;
    else if(this->ArcHeader.wi_revision_needed==3)
        scriptOffset=ext4HeaderOffset; // no extended headers, the script starts just after the header
    else if(this->ArcHeader.wi_revision_needed==4){
        this->readExt4Header(ext4HeaderOffset); // reading extended headers of version 4
        // this archive is extended, the script starts after the extended area and the stub
        scriptOffset=sizeof(WIArcHeader)+this->ArcExt4Header.cbSize+this->ArcExt4Header.stubSize;
    }
    else
        throw new E_WPIAI_UnsupportedArchive;

    if(!~(extendedDataOffset=this->readScript(scriptOffset)))
        throw new E_WPIAI_ErrorDecompressingInstallationScript;

    if(/* we have extended header data */this->ArcHeader.lExtended>0){
        if(!~(packHeadersOffset=this->readExtendedData(extendedDataOffset)))
            throw new E_WPIAI_ErrorReadingExtendedData;
    } else packHeadersOffset=extendedDataOffset;

    this->readPackageHeaders(packHeadersOffset);
}

bool WarpinArchiveInterface::verifyArcHeader(){
    return this->ArcHeader.v1==WI_VERIFY1
    &&     this->ArcHeader.v2==WI_VERIFY2
    &&     this->ArcHeader.v3==WI_VERIFY3
    &&     this->ArcHeader.v4==WI_VERIFY4;
}

/*!
 * @returns the position of the end of the header, or -1 if the header is not valid
 */
qint64 WarpinArchiveInterface::readTailHeader(){
    qint64 savedOffset=this->archive->pos(),
           offset     =this->archive->size()
                      -sizeof(WIArcExt4Header)
                      -sizeof(WIArcHeader);
    this->archive->seek(offset);
    this->archive->read(
        (char*)&this->ArcHeader,
        sizeof(WIArcHeader)
    );
    this->archive->seek(savedOffset);
    return verifyArcHeader()?offset+sizeof(WIArcHeader):-1;
}

/*!
 * @returns the position of the end of the header, or -1 if the header is not valid
 */
qint64 WarpinArchiveInterface::readFrontHeader(){
    qint64 offset,savedOffset=this->archive->pos();
    for(offset=0;offset<200000;++offset){ /* yes, lol, well...               */
        /* that's a copy-paste from the original source code of WarpIN       */
        this->archive->seek(offset);
        this->archive->read(
            (char*)&this->ArcHeader,
            sizeof(WIArcHeader)
        );
        if(this->verifyArcHeader())
            break;
    }
    this->archive->seek(savedOffset);
    return this->verifyArcHeader()?offset+sizeof(WIArcHeader):-1;
}

/*!
 * @param ext4HeaderOffset the position of the ArcExt4Header
 * @returns the position of the end of the extended header
 */
qint64 WarpinArchiveInterface::readExt4Header(qint64 ext4HeaderOffset){
    qint64 savedOffset=this->archive->pos();
    this->archive->seek(ext4HeaderOffset);
    this->archive->read(
        (char*)&this->ArcExt4Header,
        sizeof(WIArcExt4Header)
    );
    this->archive->seek(savedOffset);
    return ext4HeaderOffset+sizeof(WIArcExt4Header);
}

/*!
 * @param scriptOffset the position of the script
 * @returns the position of the end of the script or -1 if the script is corrupted
 */
qint64 WarpinArchiveInterface::readScript(qint64 scriptOffset){
    if(this->ArcHeader.usScriptCompr<=0)
        return 0;
    qint64 savedOffset=this->archive->pos();
    this->archive->seek(scriptOffset);

    char *pTempBuffer=(char*)malloc(this->ArcHeader.usScriptCompr),
         *pszScript  =(char*)malloc(this->ArcHeader.usScriptOrig+1);
    this->archive->read(pTempBuffer,this->ArcHeader.usScriptCompr);

    bz_stream z;
    z.bzalloc=0;
    z.bzfree=0;
    z.opaque=NULL;
    z.bzalloc=NULL;
    z.bzfree=NULL;
    z.next_in=pTempBuffer;
    z.avail_in=this->ArcHeader.usScriptCompr;
    z.next_out=pszScript;
    z.avail_out=this->ArcHeader.usScriptOrig+1;

    if (BZ_OK!=BZ2_bzDecompressInit(&z,0,0))
        return -1;

    int rc; do{
        rc=BZ2_bzDecompress(&z);
        if(rc!=BZ_OK && rc!=BZ_STREAM_END)
            return -1;
    } while(rc!=BZ_STREAM_END);

    BZ2_bzDecompressEnd(&z);
    pszScript[this->ArcHeader.usScriptOrig]='\0';

    this->script=pszScript;
    this->archive->seek(savedOffset);
    return scriptOffset+this->ArcHeader.usScriptCompr;
}

/*!
 * @param extendedOffset the position of the extended area
 * @returns the position of the end of the extended area
 */
qint64 WarpinArchiveInterface::readExtendedData(qint64 extendedOffset){
    qint64 savedOffset=this->archive->pos();
    this->archive->seek(extendedOffset);
    this->archive->read(
        this->extendedData=(char*)malloc(this->ArcHeader.lExtended),
        sizeof(this->ArcHeader.lExtended)
    );
    this->archive->seek(savedOffset);
    return extendedOffset+sizeof(this->extendedData);
}

/*!
 * The function to read the description (header) of each WarpIN archive's package
 * @param packHeadersOffset the position of the package headers
 */
qint64 WarpinArchiveInterface::readPackageHeaders(qint64 packHeadersOffset){
    qint64 savedOffset=this->archive->pos();
    this->archive->seek(packHeadersOffset);

    int packagesNumber=this->ArcHeader.sPackages;
    if(packagesNumber<0)
        throw new E_WPIAI_InvalidAmountOfPackagesInArchive;

    while(packagesNumber--){
        WIPackHeader *packHeader=new WIPackHeader;
        this->archive->read(
            (char*)packHeader,
            sizeof(WIPackHeader)
        );
        this->packHeadersList<<packHeader;
    }

    this->archive->seek(savedOffset);
    return 0;
}

/*!
 * For every package header from this->packHeadersList (should be populated previously)
 * scans the archive for files in the package, creating a complete WFileSystemTree
 * containing hierarchially each file and directory mentioned in the archive with full
 * information about each file (extract from each file's header, located just before it)
 * @returns the full archive's filesystem in the form of WFileSystemTree
 */
WFileSystemTree* WarpinArchiveInterface::createFileStructure(){
    QList<WIFileHeader> fileList; // building a complete list of file headers
    QList<int> filePositionsList; // WIFileHeader doesn't have a field for filepos, so making a list of them also
    for(auto i=this->packHeadersList.begin();i!=this->packHeadersList.end();++i){
        qint64 filesNumber=(*i)->files;
        if(filesNumber<0)
            throw new E_WPIAI_InvalidAmountOfFilesInPackage;

        qint64 curpos=(*i)->pos;
        while(filesNumber--){
            this->archive->seek(curpos);

            WIFileHeader fileHeader;
            this->archive->read(
                (char*)&fileHeader,
                sizeof(WIFileHeader)
            );

            if(QString(fileHeader.name).size()>MAXPATHLEN)
                throw new E_WPIAI_MaximumPathLengthExceededWhileReadingFiles;

            fileList<<fileHeader;
            filePositionsList<<curpos+sizeof(WIFileHeader);

            curpos+=sizeof(WIFileHeader)+fileHeader.compsize;
        }
    }

    /* building the very archive file system */
    auto root=new WFileSystemTree(new WFileSystemNode(new QDir()));
    int n=0; for(auto i=fileList.begin();i!=fileList.end();++i,++n){
        QString absoluteFilePath=i->name;

        /* trying to identify which package does the given file belong to */
        bool foundPackage=false;
        WIPackHeader *package;
        foreach(package,packHeadersList)
            if(package->number==i->package)
                foundPackage=true;
        if(!foundPackage)
            throw new E_WPIAI_FileBelongsToUndefinedPackage;

        auto file=new WFile(); // the WFile object for the file
        /* filling in all the information found in the file's header */
        file->setProperty("arcPos",filePositionsList[n]);
        file->setProperty("arcChecksum",i->checksum);
        file->setProperty("arcCompSize",(qint64)i->compsize);
        file->setProperty("arcCRC",(quint64)i->crc);
        QDateTime creationDateTime;
        creationDateTime.setTime_t(i->creation);
        file->setCreationDateTime(creationDateTime);
        QDateTime lastAccessDateTime;
        lastAccessDateTime.setTime_t(i->lastwrite);
        file->setLastAccessDateTime(lastAccessDateTime);
        file->setProperty("arcExtended",i->extended);
        file->setProperty("arcLastwrite",(quint64)i->lastwrite);
        file->setProperty("arcMagic",i->magic);
        file->setProperty("arcMethod",i->method);
        file->setSize((qint64)i->origsize);
        /* saving a link to this WAI */
        file->setProperty("archive",QVariant::fromValue((void*)this));
        /* providing file operation interfaces to WFile */
        file->setReadFn(&WAIFileReader::read);
        file->setSeekFn(&WAIFileReader::seek);
        file->setPosFn(&WAIFileReader::pos);
        /* constructing the WFileSystemNode chain to the file */
        auto node=parseFilePathToFSNode(absoluteFilePath,package->name,file);
        /* adding the chain to the whole FS */
        root->addChild(node);
    }
    return root;
}

/*!
 * Fully qualified path -> WFileSystemNode chain converter routine
 * @param relativeFilePath the path to be parsed into WFileSystemNode chain
 * @param currentDir the name of the root node
 * @param file the WFile object the last node (of type=file) to be associated with
 * @returns a pointer to a WFileSystemNode chain, generated from a slash/backslash-separated
 *          string where for each part the one next to it is its only child (the last one is
 *          a file, all the rest are directories)
 */
WFileSystemNode* WarpinArchiveInterface::parseFilePathToFSNode(QString relativeFilePath,QString currentDir,WFile *file){
    WFileSystemNode *node;
    if(relativeFilePath.contains('\\'))
        node=new WFileSystemNode(
            new QDir(currentDir),
            (*(new QList<WFileSystemNode*>))<<(
                parseFilePathToFSNode(
                    relativeFilePath.section('\\',1),
                    relativeFilePath.split('\\')[0],
                    file
                )
            )
        );
    else{
        file->setFileName(relativeFilePath);
        node=new WFileSystemNode(new QDir(currentDir),
            (*(new QList<WFileSystemNode*>))<<
                (new WFileSystemNode(file))
        );
    }
    return node;
}

WFileSystemTree* WarpinArchiveInterface::files(){
    if(this->fs.isNull())
        this->fs=this->createFileStructure();
    return this->fs;
}

WarpinArchiveInterface::~WarpinArchiveInterface(){
    for(auto i=this->packHeadersList.begin();i!=this->packHeadersList.end();)
        delete (*i), i=this->packHeadersList.erase(i);
}

/*****************/
/* WAIFileReader */
/*****************/

WAIFileReader::WAIFileReader(WFileSystemNode *fsNode, qint64 bufferSize):
    fsNode(fsNode),
    bufferSize(bufferSize){
    this->arcFile=((WarpinArchiveInterface*)this->fsNode->file->property("archive").value<void*>())->arcFile();
    this->inputBuffer=(char*)malloc(this->bufferSize);
    this->outputBuffer=(char*)malloc(this->bufferSize);
    this->seek(0);
}

/**
 * @brief Decompresses maxlen (or until the file's end) bytes of the
 *        compressed file to the first input parameter char *data.
 *
 * First outputs this->outputBuffer since this->bufCur til its end or
 * until maxlen is reached, then, if maxlen is not reached, decompresses
 * a new portion of data to this->outputBuffer, and outputs it again.
 *
 * @returns The amount of decompressed bytes output to char *data.
 */
qint64 WAIFileReader::read(char *data,qint64 maxlen){
    qint64 savedPos=arcFile->pos();
    // bz2 at this moment has already decompressed some data, because this->seek
    // has been already performed, at least once. So to maintain the relevant
    // information about the amounts of data decompressed at each very call of this
    // function, a special variable qint64 bz2bytesReadErr is used. Every time
    // a necessity to calculate the number of bytes decompressed by bz2 since the
    // call of this function arises, the total number of bytes decompressed by bz2,
    // which is calculated by the following formulae:
    //     (qint64(this->z->total_out_hi32)<<32)+qint64(this->z->total_out_lo32);
    // must by decreased by qint64 bz2bytesReadErr.
    qint64 bz2bytesReadErr=(qint64(this->z->total_out_hi32)<<32)+qint64(this->z->total_out_lo32);

    qint64 bytesLeftToCopy,bytesToBeCopied;
    bytesToBeCopied=(maxlen<this->fsNode->file->size()-this->decompCur)?
                maxlen:
                this->fsNode->file->size()-this->decompCur;
    bytesLeftToCopy=bytesToBeCopied;

    // there is some data in this->outputBuffer initially, so dealing with it
    qint64 bytesInBufferInitially=this->bytesBuffered-this->bufCur;
    if(bytesLeftToCopy<bytesInBufferInitially){ // got enough data in this->outputBuffer
        memcpy(data,this->outputBuffer+this->bufCur,bytesLeftToCopy); // so return
        this->bufCur+=bytesLeftToCopy;
        this->decompCur+=bytesLeftToCopy;
        return bytesLeftToCopy;
    }
    else{ // got not enough data in this->outputBuffer, so just copy it, and decompress some more
        memcpy(data,this->outputBuffer+this->bufCur,this->bufferSize-this->bufCur);
        bytesLeftToCopy-=this->bufferSize-this->bufCur;
        this->bufCur+=bytesLeftToCopy;
        this->decompCur+=bytesLeftToCopy;
    }

    forever{
        // okay, so decompressing more data
        // the data in the buffers is no longer needed, so purging them for the later use
        memset(this->inputBuffer,0,this->bufferSize);
        memset(this->outputBuffer,0,this->bufferSize);
        this->z->next_in=this->inputBuffer;
        this->z->next_out=this->outputBuffer;
        this->z->avail_out=this->bufferSize;

        // reading this->bufferSize bytes
        qint64 bytesRead=-1;
        if(!~(bytesRead=arcFile->read(this->inputBuffer,this->bufferSize)))
                throw new E_WPIAI_FileReadError;
        this->arcCur+=bytesRead;
        this->compCur+=bytesRead;
        this->z->next_in=this->inputBuffer;

        // handling errors (reached the end of the archive)
        if(bytesRead<this->bufferSize)
            ;

        // read more than needed: ran out of the compressed file data (overread compCur-compSize bytes)
        if(this->compCur>this->fsNode->file->property("arcCompSize").toLongLong()){
            // calculating bytes of compressed data left available (til the end of it)
            this->z->avail_in=this->fsNode->file->property("arcCompSize").toLongLong()-(this->compCur-bytesRead);

            // unrolling pos to the end of the comp file
            this->arcCur-=this->compCur-this->fsNode->file->property("arcCompSize").toLongLong();
            this->compCur-=this->compCur-this->fsNode->file->property("arcCompSize").toLongLong();
            arcFile->seek(this->arcCur);
        }
        // all fine, got this->bufferSize of compressed data
        else
            this->z->avail_in=this->bufferSize;

        // decompressing the whole this->inputBuffer
        forever{
            int rc=BZ2_bzDecompress(this->z);

            // an unrecoverable decompression error has occured
            if(BZ_PARAM_ERROR      == rc
            || BZ_DATA_ERROR       == rc
            || BZ_DATA_ERROR_MAGIC == rc
            || BZ_MEM_ERROR        == rc
            ) throw new E_WPIAI_FileDecompressionError;

            // compressed file's end has been reached (all data has been decompressed and is now in this->outputBuffer)
            if(BZ_STREAM_END==rc){
                // so just outputting bytesLeftToCopy or all of it, and then returning
                qint64 bytesAlreadyCopied=bytesToBeCopied-bytesLeftToCopy;
                qint64 bytesDecompressedAll=(qint64(this->z->total_out_hi32)<<32)+qint64(this->z->total_out_lo32)-bz2bytesReadErr;
                qint64 bytesJustDecompressed=bytesDecompressedAll-(bytesAlreadyCopied-bytesInBufferInitially);
                this->bytesBuffered=bytesJustDecompressed;
                qint64 bytesToCopyNow=(bytesLeftToCopy<bytesJustDecompressed)?
                            bytesLeftToCopy:
                            bytesJustDecompressed;
                this->decompCur+=bytesToCopyNow;
                this->bufCur=bytesToCopyNow;
                memcpy(data+bytesAlreadyCopied,this->outputBuffer,bytesToCopyNow);
                bytesLeftToCopy-=bytesToCopyNow;
                this->arcFile->seek(savedPos);
                return bytesToBeCopied;
            }

            // the output buffer has ended
            if(0==this->z->avail_out
            && 0< this->z->avail_in){
                qint64 bytesAlreadyCopied=bytesToBeCopied-bytesLeftToCopy;
                qint64 bytesJustDecompressed=this->bufferSize;
                this->bytesBuffered=bytesJustDecompressed;
                qint64 bytesToCopyNow=(bytesLeftToCopy<bytesJustDecompressed)?
                            bytesLeftToCopy:
                            bytesJustDecompressed;
                this->decompCur+=bytesToCopyNow;
                this->bufCur=bytesToCopyNow;
                memcpy(data+bytesAlreadyCopied,this->outputBuffer,bytesToCopyNow);
                bytesLeftToCopy-=bytesToCopyNow;
                if(0==bytesLeftToCopy){
                    this->arcFile->seek(savedPos);
                    return bytesToBeCopied;
                }
                else{
                    memset(this->outputBuffer,0,this->bufferSize);
                    this->z->next_out=this->outputBuffer;
                }
            }
            // the input data has ended
            else{
                qint64 bytesAlreadyCopied=bytesToBeCopied-bytesLeftToCopy;
                qint64 bytesDecompressedAll=(qint64(this->z->total_out_hi32)<<32)+qint64(this->z->total_out_lo32)-bz2bytesReadErr;
                qint64 bytesJustDecompressed=bytesDecompressedAll-(bytesAlreadyCopied-bytesInBufferInitially);
                this->bytesBuffered=bytesJustDecompressed;
                qint64 bytesToCopyNow=(bytesLeftToCopy<bytesJustDecompressed)?
                            bytesLeftToCopy:
                            bytesJustDecompressed;
                this->decompCur+=bytesToCopyNow;
                this->bufCur=bytesToCopyNow;
                memcpy(&data[bytesAlreadyCopied],this->outputBuffer,bytesToCopyNow);
                bytesLeftToCopy-=bytesToCopyNow;
                if(0==bytesLeftToCopy){
                    this->arcFile->seek(savedPos);
                    return bytesToBeCopied;
                }
                else break;
            }
        }
    }
    return 0;
}

qint64 WAIFileReader::pos(){
    return this->decompCur;
}

/**
 * Starts reading and decompressing compressed file until the
 * given position (the first input parameter qint64 offset) is
 * reached in the decompressed data.
 *
 * After this function's completion:
 * this->arcCur    is set to the position of the first byte of
 *                 this->inputBuffer in the archive file;
 * this->compCur   is set to the same value, but relatively to the
 *                 compressed file beginning;
 * this->decompCur is set to the first input parameter qint64 offset
 *                 (the requested position to seek to in the terms of
 *                 decompressed data);
 * this->bufCur    is set to the value representing the byte of
 *                 this->outputBuffer which is requested to seek to;
 * the position of the cursor of the archive file is left untouched.
 */
bool WAIFileReader::seek(qint64 offset){
    // purging garbage
    memset(this->inputBuffer,0,this->bufferSize);
    memset(this->outputBuffer,0,this->bufferSize);

    // initializing bz2
    this->z=new bz_stream;
    this->z->bzalloc=0;
    this->z->bzfree=0;
    this->z->opaque=NULL;
    this->z->bzalloc=NULL;
    this->z->bzfree=NULL;
    this->z->next_in=this->inputBuffer;
    this->z->next_out=this->outputBuffer;
    this->z->avail_out=this->bufferSize;
    BZ2_bzDecompressInit(this->z,0,0);

    // seeking to zero
    qint64 savedPos=arcFile->pos();
    this->arcCur=fsNode->file->property("arcPos").toLongLong();
    this->compCur=0;
    this->decompCur=0;
    this->bufCur=0;
    arcFile->seek(this->arcCur);

    qint64 outLen=0; // the amount of data we will get decompressed
    forever{
        // reading this->bufferSize bytes
        qint64 bytesRead=-1;
        if(!~(bytesRead=arcFile->read(this->inputBuffer,this->bufferSize)))
                throw new E_WPIAI_FileReadError;
        this->arcCur+=bytesRead;
        this->compCur+=bytesRead;
        this->z->next_in=this->inputBuffer;

        // handling errors (reached the end of the archive)
        if(bytesRead<this->bufferSize)
            ;

        // read more than needed: ran out of the compressed file data (overread compCur-compSize bytes)
        if(this->compCur>this->fsNode->file->property("arcCompSize").toLongLong()){
            // calculating bytes of compressed data left available (til the end of it)
            this->z->avail_in=this->fsNode->file->property("arcCompSize").toLongLong()-(this->compCur-bytesRead);

            // unrolling pos to the end of the comp file
            this->arcCur-=this->compCur-this->fsNode->file->property("arcCompSize").toLongLong();
            this->compCur-=this->compCur-this->fsNode->file->property("arcCompSize").toLongLong();
            arcFile->seek(this->arcCur);
        }
        // all fine, got this->bufferSize of compressed data
        else
            this->z->avail_in=this->bufferSize;

        // decompressing the whole this->inputBuffer
        forever{
            int rc=BZ2_bzDecompress(this->z);

            // an unrecoverable decompression error has occured
            if(BZ_PARAM_ERROR      == rc
            || BZ_DATA_ERROR       == rc
            || BZ_DATA_ERROR_MAGIC == rc
            || BZ_MEM_ERROR        == rc
            ) throw new E_WPIAI_FileDecompressionError;

            // compressed file's end has been reached (all data has been decompressed)
            if(BZ_STREAM_END==rc){
                outLen=(qint64(this->z->total_out_hi32)<<32)+qint64(this->z->total_out_lo32);

                // if we've gotten our needed offset in this->outputBuffer, we're done
                if(offset<outLen){
                    this->decompCur=offset;
                    this->bufCur=offset%this->bufferSize;
                    this->bytesBuffered=(0==outLen%this->bufferSize)?
                                this->bufferSize:
                                outLen%this->bufferSize;
                    this->arcFile->seek(savedPos);
                    return true;
                }
                // otherwise, we're unable to seek
                else return this->arcFile->seek(savedPos),false;
            }

            // the output buffer has ended
            if(0==this->z->avail_out
            && 0< this->z->avail_in){
                outLen=(qint64(this->z->total_out_hi32)<<32)+qint64(this->z->total_out_lo32);

                // if we've gotten our needed offset in this->outputBuffer, we're done
                if(offset<outLen){
                    this->decompCur=offset;
                    this->bufCur=offset%this->bufferSize;
                    this->bytesBuffered=(0==outLen%this->bufferSize)?
                                this->bufferSize:
                                outLen%this->bufferSize;
                    this->arcFile->seek(savedPos);
                    return true;
                }
                // otherwise purge the obtained decompressed data (we don't need it anyway)
                // to get the space for the new portion and continue
                else{
                    memset(this->outputBuffer,0,this->bufferSize);
                    this->z->next_out=this->outputBuffer;
                }
            }
            // the input data has ended
            else{
                outLen=(qint64(this->z->total_out_hi32)<<32)+qint64(this->z->total_out_lo32);

                // if we've gotten our needed offset in this->outputBuffer, we're done
                if(offset<outLen){
                    this->decompCur=offset;
                    this->bufCur=offset%this->bufferSize;
                    this->bytesBuffered=(0==outLen%this->bufferSize)?
                                this->bufferSize:
                                outLen%this->bufferSize;
                    this->arcFile->seek(savedPos);
                    return true;
                }
                // otherwise break out for the new portion of input data
                else break;
            }
        }
    }
}

qint64 WAIFileReader::read(char *data,qint64 len,const WFile *file){
    WAIFileReader *reader=WAIFileReader::getReaderByFSNode(file->fsNode);
    return reader->read(data,len);
}

qint64 WAIFileReader::pos(const WFile *file){
    WAIFileReader *reader=WAIFileReader::getReaderByFSNode(file->fsNode);
    return reader->pos();
}

bool WAIFileReader::seek(qint64 offset,const WFile *file){
    WAIFileReader *reader=WAIFileReader::getReaderByFSNode(file->fsNode);
    return reader->seek(offset);
}

QList<WAIFileReader*> WAIFileReader::readersList;
WAIFileReader* WAIFileReader::getReaderByFSNode(WFileSystemNode *fsNode){
    bool readerAlreadyExists=false;
    WAIFileReader *reader;
    foreach(reader,WAIFileReader::readersList)
        if(reader->fsNode==fsNode){
            readerAlreadyExists=true;
            break;
        }
    if(readerAlreadyExists)
        return reader;
    else{
        reader=new WAIFileReader(fsNode);
        WAIFileReader::readersList<<reader;
        return reader;
    }
}

WAIFileReader::~WAIFileReader(){
    free(this->inputBuffer);
    free(this->outputBuffer);
}
