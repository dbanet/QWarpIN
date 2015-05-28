#include "WarpINArchiveInterface.h"
#include "globals.h"

WarpINArchiveInterface::WarpINArchiveInterface(QFile *archive) :
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
    qDebug()<<"Size of script:"<<this->ArcHeader.usScriptOrig<<"="<<this->scr.size();
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
    qDebug("Archive filesystem: %s",qPrintable(this->files()->toJSON()));
    WFile *f=this->files()->rootNode()->children[0]->children[0]->file;
    f->open(QIODevice::ReadOnly);
    f->forceCache();
    char *d=new char[f->size()];
    memset(d,0,f->size());
    f->read(d,f->size());
    qDebug()<<d;
}

QString WarpINArchiveInterface::id() const{
    return "WarpIN";
}

QString WarpINArchiveInterface::arcName() const{
    return this->archive->fileName();
}

QFile* WarpINArchiveInterface::arcFile() const{
    return this->archive;
}

void WarpINArchiveInterface::readArcHeaders(){
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

bool WarpINArchiveInterface::verifyArcHeader(){
    return this->ArcHeader.v1==WI_VERIFY1
    &&     this->ArcHeader.v2==WI_VERIFY2
    &&     this->ArcHeader.v3==WI_VERIFY3
    &&     this->ArcHeader.v4==WI_VERIFY4;
}

/*!
 * @returns the position of the end of the header, or -1 if the header is not valid
 */
qint64 WarpINArchiveInterface::readTailHeader(){
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
qint64 WarpINArchiveInterface::readFrontHeader(){
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
qint64 WarpINArchiveInterface::readExt4Header(qint64 ext4HeaderOffset){
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
qint64 WarpINArchiveInterface::readScript(qint64 scriptOffset){
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

    this->scr=pszScript;
    this->archive->seek(savedOffset);
    return scriptOffset+this->ArcHeader.usScriptCompr;
}

/*!
 * @param extendedOffset the position of the extended area
 * @returns the position of the end of the extended area
 */
qint64 WarpINArchiveInterface::readExtendedData(qint64 extendedOffset){
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
qint64 WarpINArchiveInterface::readPackageHeaders(qint64 packHeadersOffset){
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
WFileSystemTree* WarpINArchiveInterface::createFileStructure(){
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
WFileSystemNode* WarpINArchiveInterface::parseFilePathToFSNode(QString relativeFilePath,QString currentDir,WFile *file){
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

WFileSystemTree* WarpINArchiveInterface::files(){
    if(this->fs.isNull())
        this->fs=this->createFileStructure();
    return this->fs;
}

QString WarpINArchiveInterface::script(){
    return QString();
}

WarpINArchiveInterface::~WarpINArchiveInterface(){
    for(auto i=this->packHeadersList.begin();i!=this->packHeadersList.end();)
        delete (*i), i=this->packHeadersList.erase(i);
}
