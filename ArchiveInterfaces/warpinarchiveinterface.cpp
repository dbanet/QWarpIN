#include "warpinarchiveinterface.h"
#include <bzlib.h>
#include <QDebug>

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
    qint64 ext4HeaderOffset,scriptOffset,extendedDataOffset,archiveOffset;
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
        // this archive is extended, the script start after the extended area and the stub
        scriptOffset=sizeof(WIArcHeader)+this->ArcExt4Header.cbSize+this->ArcExt4Header.stubSize;
    }
    else
        throw new E_WPIAI_UnsupportedArchive;

    if(!~(extendedDataOffset=this->readScript(scriptOffset)))
        throw new E_WPIAI_ErrorDecompressingInstallationScript;

    this->readExtendedData(extendedDataOffset);
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

void WarpinArchiveInterface::readArcFiles(){
    auto test=new WFileSystemNode(new QFile());
    auto test2=new WFileSystemNode(new QFile("test"));
    QList<WFileSystemNode*> files;
    files<<test<<test2;

    auto root=new WFileSystemNode(new QDir(),files);
    this->files=new WFileSystemTree(root);
}

WFileSystemTree* WarpinArchiveInterface::getFiles(){
    if(this->files.isNull())
        this->readArcFiles();
    return this->files;
}
