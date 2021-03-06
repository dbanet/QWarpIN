#ifndef WARPINARCHIVEINTERFACE_H
#define WARPINARCHIVEINTERFACE_H
#include <bzlib.h>

#include "../WAbstractArchiveInterface.h"
#include "WAIFileReader.h"
#include "WAIScriptParser/WAIScriptParser.h"

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

/*!
 * A structure representing a file in an archive.
 * This is stored in binary in the archive.
 *
 * In the archive, a WIFileHeader comes directly
 * before the actual file data. To extract files
 * for a package, we must find the first WIFileHeader
 * for a package (which is stored in WIPackHeader.pos)
 * and get the data which comes directly afterwards.
 *
 * The next WIFileHeader is then at after the file
 * data, whose size is stored in the "compsize" member.
 */
struct WIFileHeader{
	ushort magic           ; // must be WIFH_MAGIC for security
	short  checksum        ; // header checksum
	short  method          ; // compression method used  (0=stored (use WIArchive::Extract); 1=compressed (use WIArchive::Expand))
	short  package         ; // which package it belongs to
	long   origsize        ; // size of file (original)
	long   compsize        ; // size of file (compressed)
	ulong  crc             ; // file CRC checksum
	char   name[MAXPATHLEN]; // filename (*UM#3)
	ulong  lastwrite       ; // file's last write date/time (req. time.h) (*UM#3)
	ulong  creation        ; // file's creation date/time (req. time.h) (*UM#3)
	char   extended        ; // size of extended information (if any)
} __attribute((packed));

/*!
 * A structure representing a package in an archive.
 * This is stored in binary in the archive.
 */
struct WIPackHeader{
	short number  ; // Package number
	short files   ; // Number of files in package
	long  pos     ; // Position of first WIFileHeader for this package in archive
	long  origsize; // Size of package (original)
	long  compsize; // Size of package (compressed)
	char  name[32]; // Name of package
};

/*!
 * A structure representing a file in an archive.
 */
struct WIFile{
	char  *name  ; // Name of file
	char  *extra ; // Possible extra information
	short package; // Which package it belongs to
};

const unsigned char  WI_VERIFY1=0x77  ; // the first four
const unsigned char  WI_VERIFY2=0x04  ; // bytes of the
const unsigned char  WI_VERIFY3=0x02  ; // archive header
const unsigned char  WI_VERIFY4=0xBE  ; // (WIArcHeader.v1, .v2, .v3, .v4)
const unsigned short WIFH_MAGIC=0xF012; // any value for "magic" in file header

class WarpINArchiveInterface : public WAbstractArchiveInterface
{
public:
	WarpINArchiveInterface(QFile*);
	QString id() const;
	QString arcName() const;
	QFile* arcFile() const;
	WFileSystemTree* files();
	QString script();
	~WarpINArchiveInterface();

private:
	QFile*                    archive;
	WIArcHeader               ArcHeader;
	WIArcExt4Header           ArcExt4Header;
	char*                     extendedData;
	QString                   scr;
	QList<WIPackHeader*>      packHeadersList;

	void                      readArcHeaders();
	qint64                    readTailHeader();
	qint64                    readFrontHeader();
	qint64                    readExt4Header(qint64);
	qint64                    readExtendedData(qint64);
	bool                      verifyArcHeader();
	qint64                    readScript(qint64);
	qint64                    readPackageHeaders(qint64);

	WFileSystemTree*          createFileStructure();
	WFileSystemNode*          parseFilePathToFSNode(QString,QString,WFile*);
	QPointer<WFileSystemTree> fs;
};

#endif // WARPINARCHIVEINTERFACE_H
