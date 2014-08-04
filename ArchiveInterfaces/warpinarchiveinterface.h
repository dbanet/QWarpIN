#ifndef WARPINARCHIVEINTERFACE_H
#define WARPINARCHIVEINTERFACE_H
#include "wabstractarchiveinterface.h"

#define MAXPATHLEN 256
struct WIArcHeader{
/* 0x0000: */  uchar   v1, v2, v3, v4     ; // archive verification
/* 0x0004: */  short   wi_revision_needed ; // minimum WarpIN version required
/* 0x0006: */  char    szPath[MAXPATHLEN] ; // installation path                      // (presently not used)
/* 0x0106: */  char    szAppName[64]      ; // application name                       // (presently not used)
/* 0x0146: */  char    szDeveloper[64]    ; // delevoper name                         // (presently not used)
/* 0x0186: */  char    szInternetAddr[128]; // internet download address              // (presently not used)
/* 0x0206: */  short   sAppRevision       ; // application revision                   // (presently not used)
/* 0x0208: */  short   os                 ; // operating system to install to         // (presently not used)
/* 0x020A: */  short   sPackages          ; // number of packages in archive
/* 0x020C: */  ushort  usScriptOrig       ; // original size of installation script
/* 0x020E: */  ushort  usScriptCompr      ; // compressed size of installation script
/* 0x0210: */  long    lExtended          ; // extended data
};
struct WIArcExt4Header{
    long  cbSize                          ; // size of extended header
    long  stubSize                        ; // size of stub
    long  lReserved1,lReserved2,lReserved3,
          lReserved4,lReserved5,lReserved6,
          lReserved7,lReserved8           ;
};
const unsigned char WI_VERIFY1 = 0x77; // the first four
const unsigned char WI_VERIFY2 = 0x04; // bytes of the
const unsigned char WI_VERIFY3 = 0x02; // archive header
const unsigned char WI_VERIFY4 = 0xBE; // (WIArcHeader.v1, .v2, .v3, .v4)

class WarpinArchiveInterface : public WAbstractArchiveInterface
{
public:
    WarpinArchiveInterface(QFile*);
    QString id() const;
    QString arcName() const;
    QFile* arcFile() const;
    WFileSystemTree* getFiles();

private:
    QFile           *archive;
    WIArcHeader     ArcHeader;
    WIArcExt4Header ArcExt4Header;
    char            *extendedData;
    QString         script;

    void            readArcHeaders();
    qint64          readTailHeader();
    qint64          readFrontHeader();
    qint64          readExt4Header(qint64);
    qint64          readExtendedData(qint64);
    bool            verifyArcHeader();
    qint64          readScript(qint64);

    void            readArcFiles();
    QPointer<WFileSystemTree>
                    files;
};

#endif // WARPINARCHIVEINTERFACE_H
