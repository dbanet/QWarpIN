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
    WFile *f=(this->files()->rootNode()->children[0]->children[0]->file);
    char *d=new char[DEFAULT_BUFFER_SIZE];
    qDebug()<<"opening...";
    f->open(QIODevice::ReadOnly);
    qDebug()<<"reading...";
    f->read(d,DEFAULT_BUFFER_SIZE); QString out;
    //for(int i=0;i!=DEFAULT_BUFFER_SIZE;++i) out+=QChar(d[i]=='\0'?'.':d[i]);
    //qDebug(out.toStdString().c_str());
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
        file->setProperty("arcOrigSize",(quint64)i->origsize);
        /* saving a link to this WAI */
        file->setProperty("archive",QVariant::fromValue((void*)this));
        /* providing file operation interfaces to WFile */
        file->setReadFn(&WAIFileReader::read);
        file->setSeekFn(&WAIFileReader::seek);
        file->setPosFn(&WAIFileReader::pos);
        /* constructing the WFileSystemNode chain to the file */
        auto node=parseFilePathToFSNode(absoluteFilePath,package->name,file);
        /* adding the chain to the whole FS */
        root->addChild(node);qDebug()<<file->property("arcPos");
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
    this->z.bzalloc=0;
    this->z.bzfree=0;
    this->z.opaque=NULL;
    this->z.bzalloc=NULL;
    this->z.bzfree=NULL;

    this->compCur=fsNode->file->property("arcPos").toLongLong();
    this->decompCur=0;

    this->inputBuffer=(char*)malloc(this->bufferSize);
    this->outputBuffer=(char*)malloc(this->bufferSize);

    //BZ2_bzDecompressInit(&(this->z),0,0);
}

qint64 WAIFileReader::read(char *data,qint64 maxlen){
    qDebug()<<"::read";
    QFile *arcFile=((WarpinArchiveInterface*)this->fsNode->file->property("archive").value<void*>())->arcFile();
    qint64 savedPos=arcFile->pos();
    arcFile->seek(this->compCur);

    qint64 bytesRead=-1;
    if(!~(bytesRead=arcFile->read(this->inputBuffer,this->bufferSize)))
            throw new E_WPIAI_FileReadError;
    qDebug()<<"read "<<bytesRead<<" bytes";
    this->compCur+=bytesRead;

    if(bytesRead<this->bufferSize) // reached the end of archive
        ;

    if(this->compCur>this->fsNode->file->property("arcCompSize").toLongLong()) // ran out of the compressed file data
        this->z.avail_in=this->fsNode->file->property("arcCompSize").toLongLong()-this->compCur;
    else
        this->z.avail_in=this->bufferSize;

    this->z.next_out=this->outputBuffer;
    this->z.avail_out=this->bufferSize;
    qDebug()<<"starting decompression...";
    BZ2_bzDecompressInit(&(this->z),0,0);
    while(this->z.avail_in){
        if(0>BZ2_bzDecompress(&(this->z)))
            throw new E_WPIAI_FileDecompressionError;
    }

    data=this->outputBuffer;

    arcFile->seek(savedPos);
    return bytesRead;
}

qint64 WAIFileReader::pos(){
    return this->decompCur;
}

bool WAIFileReader::seek(qint64 offset){
    return true;
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
